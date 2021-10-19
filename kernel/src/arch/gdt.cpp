#include "gdt.h"
#include "../ds/lock.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/pmm.h"

#define ENTRY_INDEX(entry) (entry / 8)

static lock_t gdt_lock;
static tss_t tss;
static gdt_t gdt;

extern "C" void update_gdt(gdt_descriptor_t *desc, uint16_t code_selector, uint16_t data_selector);
extern "C" void update_tss(uint16_t selector);

void arch::init_gdt() {
    lock_guard_t lock(gdt_lock);

    __builtin_memset(&gdt, 0, sizeof(gdt));

    gdt.entries[ENTRY_INDEX(GDT_KERNEL_BASE)] = gdt_entry_t(0, 0);
    gdt.entries[ENTRY_INDEX(GDT_KERNEL_CS64)] = gdt_entry_t(0b10011000, 0b00100000);
    gdt.entries[ENTRY_INDEX(GDT_KERNEL_DS64)] = gdt_entry_t(0b10010110, 0b00100000);

    gdt.entries[ENTRY_INDEX(GDT_USER_BASE)] = gdt_entry_t(0, 0);
    gdt.entries[ENTRY_INDEX(GDT_USER_DS64)] = gdt_entry_t(0b11110110, 0b00100000);
    gdt.entries[ENTRY_INDEX(GDT_USER_CS64)] = gdt_entry_t(0b11111000, 0b00100000);

    gdt.descriptor = gdt_descriptor_t((uint64_t) &gdt, sizeof(gdt.entries) + sizeof(gdt.tss_entry) - 1);

    update_gdt(&gdt.descriptor, GDT_KERNEL_CS64, GDT_KERNEL_DS64);

    log_info("Successfully loaded the GDT");
}

void arch::init_tss() {
    lock_guard_t lock(gdt_lock);

    __builtin_memset(&tss, 0, sizeof(tss));

    tss.rsp[0] = phys_to_io(pmm::alloc(2) + kib(8));
    tss.ist[0] = phys_to_io(pmm::alloc(2) + kib(8));

    gdt.tss_entry = tss_entry_t((uint64_t) &tss);

    update_tss(GDT_TSS);

    log_info("Successfully loaded the TSS");
}
