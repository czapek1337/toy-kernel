#include "apic.h"
#include "../acpi/acpi.h"
#include "../acpi/hpet.h"
#include "../acpi/madt.h"
#include "../lib/addr.h"
#include "../lib/log.h"

static uint64_t apic_base_addr;

static uint32_t read_reg(uint32_t reg) {
    return *(volatile uint32_t *) (apic_base_addr + reg);
}

static void write_reg(uint32_t reg, uint32_t val) {
    *(volatile uint32_t *) (apic_base_addr + reg) = val;
}

void apic::init() {
    auto madt = (madt_t *) acpi::find_table("APIC");

    if (!madt) {
        apic_base_addr = 0xfec00000;

        log_warn("MADT table is not present on the system, defaulting APIC base to {#016x}", apic_base_addr);
    } else {
        apic_base_addr = madt->lapic_address;

        log_info("APIC base address is {#016x}", apic_base_addr);
    }

    apic_base_addr = phys_to_io(apic_base_addr);

    // enable APIC by settings the SPIV register
    write_reg(LAPIC_REG_SPURIOUS, 0x1ff);

    // calibrate and enable the APIC timer
    write_reg(LVT_REG_DIVIDE, 3 /* 16 */);
    write_reg(LVT_REG_INIT_COUNT, 0xffffffff);

    hpet::sleep_for(1000 * 1000 * 10);

    write_reg(LVT_REG_TIMER, 0x10000 /* masked */);

    auto ticks_in_10ms = 0xffffffff - read_reg(LVT_REG_CURRENT_COUNT);

    write_reg(LVT_REG_DIVIDE, 3 /* 16 */);
    write_reg(LVT_REG_TIMER, 32 | 0x20000 /* periodic on irq 32 */);
    write_reg(LVT_REG_INIT_COUNT, ticks_in_10ms / 10);
}

void apic::send_eoi() {
    write_reg(LAPIC_REG_EOI, 0);
}
