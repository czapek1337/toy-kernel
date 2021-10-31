#include "hpet.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "acpi.h"

static uint64_t hpet_base;
static uint64_t hpet_clock;

inline static uint64_t hpet_read(uint64_t reg) {
    return arch::mmio_inq(hpet_base + reg);
}

inline static void hpet_write(uint64_t reg, uint64_t val) {
    arch::mmio_outq(hpet_base + reg, val);
}

void hpet::init() {
    auto hpet = (HpetHeader *) acpi::find_table("HPET");

    assert_msg(hpet, "HPET table is not present on the system");
    assert_msg(hpet->address_space_id == 0, "Unsupported HPET address space");

    hpet_base = hpet->address;
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
