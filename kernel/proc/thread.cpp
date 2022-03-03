#include <string.h>

#include <arch/gdt.h>
#include <mem/heap.h>
#include <mem/phys.h>
#include <proc/thread.h>
#include <utils/utils.h>

static uint64_t tid_counter = 0;

static void thread_common_setup(proc::thread_t *thread, vaddr_t entry, size_t stack_pages, bool is_user) {
  thread->tid = tid_counter++; // TODO: Replace with atomic operations
  thread->exit_code = 0;

  thread->regs->ip = entry;
  thread->regs->sp = 0x7fffffff0000 + stack_pages * KIB(4);
  thread->regs->flags = 0x202;

  if (is_user) {
    thread->regs->cs = GDT_USER_CODE_64 | 0x3;
    thread->regs->ss = GDT_USER_DATA_64 | 0x3;

    thread->vm->map(0x7fffffff0000, mem::phys_alloc(stack_pages), stack_pages * KIB(4), PTE_P | PTE_W | PTE_U | PTE_NX);
  } else {
    thread->regs->cs = GDT_CODE_64;
    thread->regs->ss = GDT_DATA_64;
    thread->regs->sp = mem::phys_to_virt(mem::phys_alloc(stack_pages)) + stack_pages * KIB(4);
  }
}

static vaddr_t thread_load_elf(proc::thread_t *thread, elf64_header_t *elf, vaddr_t load_offset, bool is_user) {
  auto phdrs = (elf64_program_header_t *) ((uintptr_t) elf + elf->e_phoff);
  auto entry = elf->e_entry;

  for (auto i = 0; i < elf->e_phnum; i++) {
    auto phdr = &phdrs[i];

    if (phdr->p_type == PT_LOAD) {
      auto virt_start = load_offset + ALIGN_DOWN(phdr->p_vaddr, 4096);
      auto virt_end = load_offset + ALIGN_UP(phdr->p_vaddr + phdr->p_memsz, 4096);
      auto virt_file_end = load_offset + ALIGN_UP(phdr->p_vaddr + phdr->p_filesz, 4096);

      auto size = virt_end - virt_start;
      auto file_size = virt_file_end - virt_start;
      auto map_flags = PTE_P | (is_user ? PTE_U : 0);

      if (IS_SET(phdr->p_flags, PF_W))
        map_flags |= PTE_W;

      if (IS_CLEAR(phdr->p_flags, PF_X))
        map_flags |= PTE_NX;

      auto phys_segment = mem::phys_alloc(ALIGN_UP(size, 4096) / 4096);
      auto virt_segment = mem::phys_to_virt(phys_segment);

      memset((void *) virt_segment, 0, size);
      memcpy((void *) virt_segment, (const void *) ((uintptr_t) elf + phdr->p_offset), file_size);

      thread->vm->map(virt_start, phys_segment, size, map_flags);
    } else if (phdr->p_type == PT_INTERP) {
      // TODO: Load the rtdl
    }
  }

  return entry;
}

proc::thread_t *proc::create_thread(vaddr_t entry, bool is_user) {
  auto thread = ALLOC_ZERO(thread_t);

  thread->vm = mem::new_vm();
  thread->regs = ALLOC_ZERO(interrupts::isr_frame_t);

  thread_common_setup(thread, entry, 2, is_user);

  return thread;
}

proc::thread_t *proc::create_thread(elf64_header_t *elf, bool is_user) {
  auto thread = ALLOC_ZERO(thread_t);

  thread->vm = mem::new_vm();
  thread->regs = ALLOC_ZERO(interrupts::isr_frame_t);

  auto entry = thread_load_elf(thread, elf, 0, is_user);

  thread_common_setup(thread, entry, 2, is_user);

  return thread;
}
