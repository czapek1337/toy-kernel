#pragma once

#include <stdint.h>

struct [[gnu::packed]] IdtEntry {
    uint16_t offset_low;
    uint16_t code_selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
};

struct [[gnu::packed]] IdtDescriptor {
    uint16_t limit;
    uint64_t base;
};

struct [[gnu::packed]] Idt {
    IdtEntry entries[256];
};

void initialize_idt();
