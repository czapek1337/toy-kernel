#include <stdatomic.h>
#include <string.h>

#include "../arch/gdt.h"
#include "../mem/heap.h"
#include "../mem/phys.h"
#include "../utils/utils.h"
#include "thread.h"

static uint64_t tid_counter = 0;

static void thread_common_setup(thread_t *thread, vaddr_t entry, size_t stack_pages, bool is_user) {
  thread->tid = __atomic_fetch_add(&tid_counter, 1, __ATOMIC_ACQ_REL);
  thread->exit_code = 0;

  thread->regs->ip = entry;
  thread->regs->sp = 0x7fffffff0000 + stack_pages * KIB(4);
  thread->regs->flags = 0x202;

  if (is_user) {
    thread->regs->cs = GDT_USER_CODE_64 | 0x3;
    thread->regs->ss = GDT_USER_DATA_64 | 0x3;

    vm_map(thread->vm, 0x7fffffff0000, phys_alloc(stack_pages), stack_pages * KIB(4), PTE_P | PTE_W | PTE_U | PTE_NX);
  } else {
    thread->regs->cs = GDT_CODE_64;
    thread->regs->ss = GDT_DATA_64;
    thread->regs->sp = phys_to_virt(phys_alloc(stack_pages)) + stack_pages * KIB(4);
  }
}

thread_t *thread_create(vaddr_t entry, bool is_user) {
  thread_t *thread = ALLOC_ZERO(thread_t);

  thread->vm = virt_new_vm();
  thread->regs = ALLOC_ZERO(isr_frame_t);

  thread_common_setup(thread, entry, 2, is_user);

  return thread;
}

thread_t *thread_create_elf(elf64_header_t *elf, bool is_user) {
  thread_t *thread = ALLOC_ZERO(thread_t);

  thread->vm = virt_new_vm();
  thread->regs = ALLOC_ZERO(isr_frame_t);

  elf64_program_header_t *phdrs = (elf64_program_header_t *) ((uintptr_t) elf + elf->e_phoff);

  for (size_t i = 0; i < elf->e_phnum; i++) {
    elf64_program_header_t *phdr = &phdrs[i];

    if (phdr->p_type != PT_LOAD)
      continue;

    size_t page_count = ALIGN_UP(phdr->p_memsz, 4096) / 4096;
    uint64_t map_flags = PTE_P | (is_user ? PTE_U : 0);

    if (IS_SET(phdr->p_flags, PF_W))
      map_flags |= PTE_W;

    if (IS_CLEAR(phdr->p_flags, PF_X))
      map_flags |= PTE_NX;

    paddr_t segment_phys = phys_alloc(page_count);
    vaddr_t segment_virt = phys_to_virt(segment_phys);

    memset((void *) segment_virt, 0, phdr->p_memsz);
    memcpy((void *) segment_virt, (void *) ((uintptr_t) elf + phdr->p_offset), phdr->p_filesz);

    vm_map(thread->vm, phdr->p_vaddr, segment_phys, page_count * KIB(4), map_flags);
  }

  thread_common_setup(thread, elf->e_entry, 2, is_user);

  return thread;
}
