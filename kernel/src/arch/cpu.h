#pragma once

#include <stdint.h>

#include "../proc/task.h"
#include "gdt.h"

struct cpu_t {
    uint64_t ap_id;
    uint64_t lapic_id;

    tss_t tss;

    task_t *current_task;

    bool is_present;
    bool retain_enable;

    uint64_t retain_depth;
};

namespace arch {

void init_bsp();
void init_cpu(uint64_t ap_id, uint64_t lapic_id);

cpu_t *get_cpu(uint64_t index);
cpu_t *get_current_cpu();

} // namespace arch
