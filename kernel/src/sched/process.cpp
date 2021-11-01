#include "process.h"
#include "../arch/gdt.h"
#include "../lib/addr.h"
#include "../mm/heap.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"
#include "sched.h"

static uint64_t pid_counter = 0;

Thread *Thread::spawn(Process *process, uint64_t entry, bool is_user) {
    auto thread = new Thread;

    thread->process = process;
    thread->registers = new Registers;

    __builtin_memset(thread->registers, 0, sizeof(Registers));

    thread->id = process->tid_counter++;
    thread->exit_code = 0;
    thread->syscall_kernel_stack = heap::alloc(kib(4)) + kib(4);
    thread->syscall_user_stack = 0;
    thread->is_running = true;

    thread->registers->rip = entry;
    thread->registers->rsp = 0x7fffffff0000 + kib(4);
    thread->registers->rflags = 0x200;

    if (is_user) {
        thread->registers->cs = GDT_USER_CS64 | 3;
        thread->registers->ss = GDT_USER_DS64 | 3;

        process->pml4->map(thread->registers->rsp - kib(4), pmm::alloc(1), kib(4),
                           PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE | PAGE_TABLE_ENTRY_USER);
    } else {
        thread->registers->rsp += 0xffff800000000000;

        thread->registers->cs = GDT_KERNEL_CS64;
        thread->registers->ss = GDT_KERNEL_DS64;

        process->pml4->map(thread->registers->rsp - kib(4), pmm::alloc(1), kib(4), PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE);
    }

    process->threads.insert(thread->id, thread);

    return thread;
}

Process *Process::spawn(Process *parent_process, const core::String &name, uint64_t entry, bool is_user) {
    auto process = new Process;

    process->id = pid_counter++;
    process->exit_code = 0;
    process->tid_counter = 0;
    process->fd_counter = 0;
    process->parent_process = parent_process;

    process->is_running = true;
    process->is_user = is_user;
    process->pml4 = vmm::create_pml4();
    process->name = name;

    auto main_thread = Thread::spawn(process, entry, is_user);

    sched::queue(main_thread);

    return process;
}

Process *Process::spawn(Process *parent_process, const core::String &name, Elf64 *elf, bool is_user) {
    auto process = new Process;

    process->id = pid_counter++;
    process->exit_code = 0;
    process->tid_counter = 0;
    process->fd_counter = 0;
    process->parent_process = parent_process;

    process->is_running = true;
    process->is_user = is_user;
    process->pml4 = vmm::create_pml4();
    process->name = name;

    auto phdrs = (Elf64Phdr *) ((uint64_t) elf + elf->ph_off);

    for (auto i = 0; i < elf->ph_num; i++) {
        auto phdr = &phdrs[i];

        // Skip any non loadable segments
        if (phdr->type != 1)
            continue;

        auto misalignment = phdr->vaddr & 0xfff;
        auto page_count = ((misalignment + phdr->memsz + 4095) / 4096) + (((misalignment + phdr->memsz) > 4096) ? 1 : 0);
        auto flags = PAGE_TABLE_ENTRY_PRESENT | (is_user ? PAGE_TABLE_ENTRY_USER : 0);

        if (phdr->flags & 2)
            flags |= PAGE_TABLE_ENTRY_WRITE;

        if (!(phdr->flags & 1))
            flags |= PAGE_TABLE_ENTRY_NO_EXECUTE;

        auto phys_addr = pmm::alloc(page_count);
        auto virt_addr = align_down(phdr->vaddr, 4096);

        process->pml4->map(virt_addr, phys_addr, page_count * 0x1000, flags);

        __builtin_memset((uint8_t *) phys_to_io(phys_addr) + misalignment, 0, phdr->memsz);
        __builtin_memcpy((uint8_t *) phys_to_io(phys_addr) + misalignment, (uint8_t *) ((uint64_t) elf + phdr->offset), phdr->filesz);
    }

    auto main_thread = Thread::spawn(process, elf->entry, is_user);

    sched::queue(main_thread);

    return process;
}
