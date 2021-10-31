#include "apic.h"
#include "../acpi/acpi.h"
#include "../acpi/hpet.h"
#include "../acpi/madt.h"
#include "../lib/addr.h"
#include "../lib/log.h"

static uint64_t apic_base_addr = 0;

inline static uint32_t apic_read(uint32_t reg) {
    return arch::mmio_ind(apic_base_addr + reg);
}

inline static void apic_write(uint32_t reg, uint32_t val) {
    arch::mmio_outd(apic_base_addr + reg, val);
}

void apic::init() {
    auto madt = (MadtHeader *) acpi::find_table("APIC");

    if (!madt) {
        apic_base_addr = Msr::apic().read();

        log_warn("MADT table is not present on the system, defaulting to APIC MSR ({#016x})", apic_base_addr);
    } else {
        apic_base_addr = madt->lapic_address;

        log_info("APIC base address is {#016x}", apic_base_addr);
    }

    // enable APIC by settings the SPIV register
    apic_write(LAPIC_REG_SPURIOUS, 0x1ff);

    // calibrate and enable the APIC timer
    apic_write(LVT_REG_DIVIDE, 3 /* 16 */);
    apic_write(LVT_REG_INIT_COUNT, 0xffffffff);

    hpet::sleep_for(1000 * 1000 * 10);

    apic_write(LVT_REG_TIMER, 0x10000 /* masked */);

    auto ticks_in_10ms = 0xffffffff - apic_read(LVT_REG_CURRENT_COUNT);

    apic_write(LVT_REG_DIVIDE, 3 /* 16 */);
    apic_write(LVT_REG_TIMER, 32 | 0x20000 /* periodic on irq 32 */);
    apic_write(LVT_REG_INIT_COUNT, ticks_in_10ms / 10);
}

void apic::send_eoi() {
    apic_write(LAPIC_REG_EOI, 0);
}

void apic::send_ipi(uint64_t cpu_id, uint64_t interrupt_id) {
    apic_write(LAPIC_REG_ICR1, cpu_id << 24);
    apic_write(LAPIC_REG_ICR0, interrupt_id | 1 << 14);
}

uint64_t apic::get_current_cpu() {
    if (apic_base_addr == 0)
        return 0;

    return apic_read(LAPIC_CPU_ID);
}
