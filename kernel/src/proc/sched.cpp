#include "sched.h"
#include "../arch/gdt.h"
#include "../ds/vector.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/heap.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"

static vector_t<task_t *> tasks;

static uint64_t current_task_idx;

static void initialize_task(task_t *task, uint64_t entry, uint64_t stack_size, bool is_user) {
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
}

void task::init_sched() {
    current_task_idx = -1;

    // TODO: Init scheduler
}

void task::create_task(const string_t &name, uint64_t entry, uint64_t stack_size, bool is_user) {
    auto task = new task_t;

    task->exit_code = 0;
    task->name = name;

    task->is_running = true;
    task->is_user = is_user;
    task->is_in_syscall = false;

    initialize_task(task, entry, stack_size, is_user);

    tasks.push(task);
}

void task::create_task_from_elf(const string_t &name, elf64_t *elf, uint64_t stack_size, bool is_user) {
    auto task = new task_t;
    auto phdrs = (elf64_phdr_t *) ((uint64_t) elf + elf->ph_off);

    task->exit_code = 0;
    task->name = name;

    task->is_running = true;
    task->is_user = is_user;
    task->is_in_syscall = false;

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

        auto rbx = (uint8_t *) ((uint64_t) elf + phdr->offset);

        log_debug("Load segment with offset={#08x}, mem_size={#08x} and filesize={#08x} at {#016x}", phdr->offset, phdr->memsz,
                  phdr->filesz, virt_addr);
    }

    log_debug("Initializing task at {#016x} with entry={#016x}", task, elf->entry);

    initialize_task(task, elf->entry, stack_size, is_user);

    tasks.push(task);
}

task_t *task::reschedule() {
    // TODO: A more sophisticated rescheduling logic lol

    auto current_task = get_current_task();

    if (current_task) {
        if (!current_task->is_running) {
            log_info("Task {} exited with code {}", current_task->name, current_task->exit_code);

            delete current_task;

            tasks.remove(current_task);
        } else if (++current_task->ticks_since_schedule < 3) {
            return current_task;
        }
    }

    while (true) {
        current_task_idx = (current_task_idx + 1) % tasks.size();

        auto new_task = get_current_task();

        if (!new_task->is_running)
            continue;

        new_task->ticks_since_schedule = 0;

        return new_task;
    }
}

task_t *task::get_current_task() {
    if (tasks.size() == 0 || current_task_idx == -1)
        return nullptr;

    return tasks[current_task_idx];
}
