#include <core/lock.h>
#include <core/vector.h>

#include "../arch/cpu.h"
#include "../arch/gdt.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/heap.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"
#include "sched.h"

static core::lock_t scheduler_lock;
static core::vector_t<Task *> task_queue;
static Task *idle_task;

static void kernel_idle_task() {
    while (true) {
        arch::halt();
    }
}

static Task *initialize_task(uint64_t entry, uint64_t stack_size, bool is_user) {
    auto task = new Task;
    auto stack_pages = align_up(stack_size, 4096) / 4096;
    auto stack_virt = (uint64_t) nullptr;

    if (is_user) {
        auto guard_page = pmm::alloc(1);
        auto stack_top = 0x00007fffffff0000; // User stack top

        stack_virt = stack_top - 0x1000;

        stack_top -= 0x1000; // Guard page
        task->pml4->map(stack_top, guard_page, 0x1000, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_USER);

        for (auto i = 0; i < stack_pages; i++) {
            auto stack_page = pmm::alloc(1);

            stack_top -= 0x1000;
            task->pml4->map(stack_top, stack_page, 0x1000, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE | PAGE_TABLE_ENTRY_USER);
        }

        stack_top -= 0x1000; // Bottom guard page
        task->pml4->map(stack_top, guard_page, 0x1000, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_USER);
    } else {
        auto stack_top = phys_to_io(gib(128)); // Kernel stack top

        stack_virt = stack_top;

        for (auto i = 0; i < stack_pages; i++) {
            auto stack_page = pmm::alloc(1);

            stack_top -= 0x1000;
            task->pml4->map(stack_top, stack_page, 0x1000, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE);
        }
    }

    auto syscall_stack = heap::alloc(kib(2));

    task->syscall_kernel_stack = syscall_stack + kib(2);
    task->syscall_user_stack = 0;

    __builtin_memset(&task->regs, 0, sizeof(Registers));

    task->regs.rip = entry;
    task->regs.rsp = stack_virt;
    task->regs.rflags = 0x200;

    if (is_user) {
        task->regs.cs = GDT_USER_CS64 | 3 /* Privilege level */;
        task->regs.ss = GDT_USER_DS64 | 3 /* Privilege level */;
    } else {
        task->regs.cs = GDT_KERNEL_CS64;
        task->regs.ss = GDT_KERNEL_DS64;
    }

    return task;
}

static Task *create_basic_task(const core::string_t &name, uint64_t entry, uint64_t stack_size, bool is_user) {
    auto task = initialize_task(entry, stack_size, is_user);

    task->exit_code = 0;
    task->fd_counter = 3;
    task->name = name;

    task->is_running = true;
    task->is_user = is_user;

    return task;
}

void task::init_sched() {
    idle_task = create_basic_task("idle", (uint64_t) kernel_idle_task, kib(4), false);
}

void task::create_task(const core::string_t &name, uint64_t entry, uint64_t stack_size, bool is_user) {
    core::lock_guard_t lock(scheduler_lock);

    task_queue.push(create_basic_task(name, entry, stack_size, is_user));
}

void task::create_task_from_elf(const core::string_t &name, Elf64 *elf, uint64_t stack_size, bool is_user) {
    core::lock_guard_t lock(scheduler_lock);

    auto task = create_basic_task(name, elf->entry, stack_size, is_user);
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

        task->pml4->map(virt_addr, phys_addr, page_count * 0x1000, flags);

        __builtin_memset((uint8_t *) phys_to_io(phys_addr) + misalignment, 0, phdr->memsz);
        __builtin_memcpy((uint8_t *) phys_to_io(phys_addr) + misalignment, (uint8_t *) ((uint64_t) elf + phdr->offset), phdr->filesz);
    }

    task_queue.push(task);
}

Task *task::reschedule(Registers *regs) {
    core::lock_guard_t lock(scheduler_lock);

    auto current_cpu = arch::get_current_cpu();
    auto current_task = current_cpu->current_task;

    if (current_task && !current_task->is_running) {
        delete current_task;

        current_cpu->current_task = nullptr;
        current_task = nullptr;
    }

    if (task_queue.size() > 0) {
        auto next_task = task_queue[0];

        task_queue.remove(next_task);

        if (current_task) {
            current_task->save(regs);

            task_queue.push(current_task);
        }

        next_task->load(regs);

        current_cpu->current_task = next_task;

        return next_task;
    }

    if (current_task)
        return current_task;

    idle_task->load(regs);

    current_cpu->current_task = idle_task;

    return idle_task;
}

Task *task::get_current_task() {
    return arch::get_current_cpu()->current_task;
}
