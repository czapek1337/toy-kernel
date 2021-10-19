#pragma once

#include <stdint.h>

enum apic_reg_t {
    LAPIC_REG_EOI = 0xb0,
    LAPIC_REG_SPURIOUS = 0xf0,
    LVT_REG_INIT_COUNT = 0x380,
    LVT_REG_CURRENT_COUNT = 0x390,
    LVT_REG_TIMER = 0x320,
    LVT_REG_DIVIDE = 0x3e0,
};

namespace apic {

void init();
void send_eoi();

} // namespace apic
