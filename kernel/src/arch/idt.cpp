#include <core/lock.h>

#include "../lib/log.h"
#include "gdt.h"
#include "idt.h"

static core::SpinLock idt_lock;
static Idt idt;

extern "C" uint64_t interrupt_vectors[256];

extern "C" void update_idt(const IdtDescriptor &desc);

static IdtEntry idt_entry(uint64_t address, uint16_t selector, uint8_t ist, uint8_t flags) {
    IdtEntry entry;

    entry.offset_low = (uint16_t) (address & 0xffff);
    entry.code_selector = selector;
    entry.ist = ist;
    entry.attributes = flags;
    entry.offset_mid = (uint16_t) ((address >> 16) & 0xffff);
    entry.offset_high = (uint32_t) ((address >> 32) & 0xffffffff);
    entry.reserved = 0;

    return entry;
}

void initialize_idt() {
    core::LockGuard lock(idt_lock);

    __builtin_memset(&idt, 0, sizeof(idt));

    for (auto i = 0; i < 256; i++) {
        idt.entries[i] = idt_entry(interrupt_vectors[i], GDT_KERNEL_CS64, 0, 0x8e);
    }

    update_idt({.limit = sizeof(Idt) - 1, .base = (uint64_t) &idt});

    log_info("Successfully loaded the IDT");
}
