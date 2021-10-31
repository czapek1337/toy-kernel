#pragma once

#include <stdint.h>

enum GdtSelector : uint16_t {
    GDT_KERNEL_BASE = 0x00,
    GDT_KERNEL_CS64 = 0x08,
    GDT_KERNEL_DS64 = 0x10,
    GDT_USER_BASE = 0x18,
    GDT_USER_DS64 = 0x20,
    GDT_USER_CS64 = 0x28,
    GDT_TSS = 0x30,
};

struct [[gnu::packed]] Tss {
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
};

struct [[gnu::packed]] GdtEntry {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags;
    uint8_t granularity;
    uint8_t base_high;
};

struct [[gnu::packed]] GdtExtendedEntry : GdtEntry {
    uint32_t base_high_ex;
    uint32_t reserved;
};

struct [[gnu::packed]] GdtDescriptor {
    uint16_t limit;
    uint64_t base;
};

struct [[gnu::packed]] Gdt {
    GdtEntry entries[6];
    GdtExtendedEntry tss_entry;
};

void initialize_gdt();
void initialize_tss();
