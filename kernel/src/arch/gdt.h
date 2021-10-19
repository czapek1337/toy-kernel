#pragma once

#include <stdint.h>

enum gdt_selector_t : uint16_t {
    GDT_KERNEL_BASE = 0x00,
    GDT_KERNEL_CS64 = 0x08,
    GDT_KERNEL_DS64 = 0x10,
    GDT_USER_BASE = 0x18,
    GDT_USER_DS64 = 0x20,
    GDT_USER_CS64 = 0x28,
    GDT_TSS = 0x30,
};

struct [[gnu::packed]] tss_t {
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
};

struct [[gnu::packed]] gdt_entry_t {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags;
    uint8_t granularity;
    uint8_t base_high;

    gdt_entry_t() = default;
    gdt_entry_t(uint8_t gdt_flags, uint8_t gdt_granularity) {
        limit = 0;
        base_low = 0;
        base_mid = 0;
        flags = gdt_flags;
        granularity = gdt_granularity;
        base_high = 0;
    }
};

struct [[gnu::packed]] tss_entry_t {
    uint16_t length;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high;
    uint32_t base_high32;
    uint32_t reserved;

    tss_entry_t() = default;
    tss_entry_t(uint64_t address) {
        length = 104;
        base_low = address & 0xffff;
        base_mid = address >> 16 & 0xff;
        flags1 = 0b10001001;
        flags2 = 0;
        base_high = address >> 24 & 0xff;
        base_high32 = address >> 32 & 0xffff'ffff;
        reserved = 0;
    }
};

struct [[gnu::packed]] gdt_descriptor_t {
    uint16_t limit;
    uint64_t base;

    gdt_descriptor_t() = default;
    gdt_descriptor_t(uint64_t gdt_base, uint16_t gdt_limit) {
        base = gdt_base;
        limit = gdt_limit;
    }
};

struct [[gnu::packed]] gdt_t {
    gdt_entry_t entries[6];
    tss_entry_t tss_entry;

    gdt_descriptor_t descriptor;
};

namespace arch {

void init_gdt();
void init_tss();

} // namespace arch
