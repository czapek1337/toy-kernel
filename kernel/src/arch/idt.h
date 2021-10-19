#pragma once

#include <stdint.h>

enum idt_vector_t : uint8_t {
    IDT_VEC_TIMER = 32,
};

struct [[gnu::packed]] idt_entry_t {
    uint16_t offset_low;
    uint16_t code_selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;

    idt_entry_t() = default;
    idt_entry_t(uint16_t idt_code_selector, uint8_t idt_ist, uint8_t idt_attributes, uint64_t offset) {
        offset_low = offset & 0xffff;
        code_selector = idt_code_selector;
        ist = idt_ist;
        attributes = idt_attributes;
        offset_mid = (offset >> 16) & 0xffff;
        offset_high = (offset >> 32) & 0xffff'ffff;
        reserved = 0;
    }
};

struct [[gnu::packed]] idt_descriptor_t {
    uint16_t limit;
    uint64_t base;

    idt_descriptor_t() = default;
    idt_descriptor_t(uint64_t idt_base, uint16_t idt_limit) {
        base = idt_base;
        limit = idt_limit;
    }
};

struct [[gnu::packed]] idt_t {
    idt_entry_t entries[256];
    idt_descriptor_t descriptor;
};

namespace arch {

void init_idt();

}
