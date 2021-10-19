#include "idt.h"
#include "../ds/lock.h"
#include "../lib/log.h"
#include "gdt.h"

static lock_t idt_lock;
static idt_t idt;

extern "C" uint64_t interrupt_vectors[256];

extern "C" void update_idt(idt_descriptor_t *desc);

void arch::init_idt() {
    lock_guard_t lock(idt_lock);

    __builtin_memset(&idt, 0, sizeof(idt));

    for (auto i = 0; i < 256; i++) {
        idt.entries[i] = idt_entry_t(GDT_KERNEL_CS64, 0, 0x8e, interrupt_vectors[i]);
    }

    idt.descriptor = idt_descriptor_t((uint64_t) &idt, sizeof(idt.entries) - 1);

    update_idt(&idt.descriptor);

    log_info("Successfully loaded the IDT");
}
