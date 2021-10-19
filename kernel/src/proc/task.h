#pragma once

#include <stdint.h>

#include "../intr/intr.h"
#include "../mm/vmm.h"

struct task_t {
    uint64_t exit_code;
    uint64_t ticks_since_schedule;

    uint64_t syscall_kernel_stack;
    uint64_t syscall_user_stack;

    bool is_running;
    bool is_user;
    bool is_in_syscall;

    page_table_t *pml4;
    registers_t regs;

    const char *name;

    task_t();
    ~task_t();

    void save(registers_t *regs);
    void load(registers_t *regs);
    void kill(uint64_t exit_code);
};
