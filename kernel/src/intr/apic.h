#pragma once

#include <stdint.h>

enum LapicRegister {
    LAPIC_CPU_ID = 0x20,
    LAPIC_REG_EOI = 0xb0,
    LAPIC_REG_ICR0 = 0x300,
    LAPIC_REG_ICR1 = 0x310,
    LAPIC_REG_SPURIOUS = 0xf0,
    LVT_REG_INIT_COUNT = 0x380,
    LVT_REG_CURRENT_COUNT = 0x390,
    LVT_REG_TIMER = 0x320,
    LVT_REG_DIVIDE = 0x3e0,
};

namespace apic {

void init();
void send_eoi();
void send_ipi(uint64_t cpu_id, uint64_t interrupt_id);

uint64_t get_current_cpu();

} // namespace apic
