#include "sched.h"
#include "../arch/gdt.h"
#include "../ds/vector.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"

static vector_t<task_t *> tasks;

static uint64_t current_task_idx;

void task::init_sched() {
    current_task_idx = -1;

    // TODO: Init scheduler
}

void task::create_task(uint64_t entry, uint64_t stack_size, bool is_user) {
    auto task = new task_t;

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

    __builtin_memset(&task->regs, 0, sizeof(registers_t));

    task->regs.rip = entry;
    task->regs.rsp = stack_virt;
    task->regs.rflags = CPU_FLAGS_INTERRUPT;

    if (is_user) {
        task->regs.cs = GDT_USER_CS64 | 3 /* Privilege level */;
        task->regs.ss = GDT_USER_DS64 | 3 /* Privilege level */;
    } else {
        task->regs.cs = GDT_KERNEL_CS64;
        task->regs.ss = GDT_KERNEL_DS64;
    }

    tasks.push(task);
}

task_t *task::reschedule() {
    // TODO: A more sophisticated rescheduling logic lol

    auto current_task = get_current_task();

    if (current_task && ++current_task->ticks_since_schedule < 3)
        return current_task;

    current_task_idx = (current_task_idx + 1) % tasks.size();

    auto new_task = get_current_task();

    new_task->ticks_since_schedule = 0;

    return new_task;
}

task_t *task::get_current_task() {
    if (tasks.size() == 0 || current_task_idx == -1)
        return nullptr;

    return tasks[current_task_idx];
}
