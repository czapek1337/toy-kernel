#pragma once

#include <stdint.h>

#include "../intr/intr.h"
#include "../mm/vmm.h"

struct task_t {
    uint64_t ticks_since_schedule;

    page_table_t *pml4;
    registers_t regs;

    task_t();
    ~task_t();

    void save(registers_t *regs);
    void load(registers_t *regs);
};
