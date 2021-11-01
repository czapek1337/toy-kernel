#pragma once

#include <stdint.h>

#include "../sched/process.h"
#include "gdt.h"

struct ProcessorState {
    uint64_t ap_id;
    uint64_t lapic_id;

    Tss tss;
    Thread *current_thread;

    bool is_present;
    bool retain_enable;

    uint64_t retain_depth;
};

namespace arch {

void init_bsp();
void init_cpu(uint64_t ap_id, uint64_t lapic_id);

ProcessorState *get_cpu(uint64_t index);
ProcessorState *get_current_cpu();

} // namespace arch
