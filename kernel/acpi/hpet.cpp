#include "hpet.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "acpi.h"

static uint64_t hpet_base;
static uint64_t hpet_clock;

static void hpet_write(uint64_t offset, uint64_t value) {
    *(volatile uint64_t *) (hpet_base + offset) = value;
}

static uint64_t hpet_read(uint64_t offset) {
    return *(volatile uint64_t *) (hpet_base + offset);
}

void hpet::init() {
    auto hpet = (hpet_t *) acpi::find_table("HPET");

    if (!hpet)
        log_fatal("HPET table is not present on the system");

    if (hpet->address_space_id != 0)
        log_fatal("Unsupported HPET address space");

    hpet_base = phys_to_io(hpet->address);
    hpet_clock = hpet_read(HPET_REG_GENERAL_CAPS_AND_ID) >> 32;

    log_info("HPET base is located at {#016x}", hpet_base);
    log_info("HPET counter tick period is equal to {}ns", hpet_clock / 1000000);

    hpet_write(HPET_REG_GENERAL_CONFIG, 0);
    hpet_write(HPET_REG_MAIN_COUNTER, 0);
    hpet_write(HPET_REG_GENERAL_CONFIG, 1);
}

void hpet::sleep_for(uint64_t nanoseconds) {
    auto target = hpet_read(HPET_REG_MAIN_COUNTER) + (nanoseconds * 1000000) / hpet_clock;

    while (hpet_read(HPET_REG_MAIN_COUNTER) < target) {
        arch::pause();
    }
}
