#include <core/lock.h>

#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/pmm.h"
#include "cpu.h"
#include "gdt.h"

static core::SpinLock gdt_lock;
static Gdt gdt;

extern "C" void update_gdt(const GdtDescriptor &desc);
extern "C" void update_tss();

static GdtEntry gdt_entry(uint8_t access, uint8_t granularity) {
    GdtEntry entry;

    entry.limit = 0;
    entry.base_low = 0;
    entry.base_mid = 0;
    entry.flags = access;
    entry.granularity = granularity;
    entry.base_high = 0;

    return entry;
}

static GdtExtendedEntry gdt_tss_entry(uint64_t address) {
    GdtExtendedEntry entry;

    entry.limit = sizeof(Tss) - 1;
    entry.base_low = (uint16_t) (address & 0xffff);
    entry.base_mid = (uint8_t) ((address >> 16) & 0xff);
    entry.flags = 0b10001001;       // yeah
    entry.granularity = 0b00100000; // that's a thing now
    entry.base_high = (uint8_t) ((address >> 24) & 0xff);
    entry.base_high_ex = (uint32_t) ((address >> 32) & 0xffff'ffff);
    entry.reserved = 0;

    return entry;
}

void initialize_gdt() {
    core::LockGuard lock(gdt_lock);

    __builtin_memset(&gdt, 0, sizeof(gdt));

    gdt.entries[1] = gdt_entry(0b10011000, 0b00100000);
    gdt.entries[2] = gdt_entry(0b10010110, 0b00100000);

    gdt.entries[4] = gdt_entry(0b11110110, 0b00100000);
    gdt.entries[5] = gdt_entry(0b11111000, 0b00100000);

    update_gdt({.limit = sizeof(Gdt) - 1, .base = (uint64_t) &gdt});

    log_info("Successfully loaded the GDT");
}

void initialize_tss() {
    core::LockGuard lock(gdt_lock);

    auto current_cpu = arch::get_current_cpu();

    gdt.tss_entry = gdt_tss_entry((uint64_t) &current_cpu->tss);

    update_tss();

    log_info("Successfully loaded the TSS for CPU #{}", current_cpu->ap_id);
}
