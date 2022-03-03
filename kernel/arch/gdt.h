#pragma once

#include <stdint.h>

#include <utils/utils.h>

#define GDT_NULL 0x0
#define GDT_CODE_16 0x08
#define GDT_DATA_16 0x10
#define GDT_CODE_32 0x18
#define GDT_DATA_32 0x20
#define GDT_CODE_64 0x28
#define GDT_DATA_64 0x30
#define GDT_USER_CODE_64 0x38
#define GDT_USER_DATA_64 0x40
#define GDT_TSS 0x48

namespace arch {

  struct packed tss_t {
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
  };

  struct packed gdt_entry_t {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags;
    uint8_t granularity;
    uint8_t base_high;
  };

  struct packed gdt_entry_ex_t {
    gdt_entry_t low;

    uint32_t base_high_ex;
    uint32_t reserved;
  };

  struct packed gdt_t {
    gdt_entry_t entries[GDT_TSS / 0x8];
    gdt_entry_ex_t tss_entry;
  };

  void init_gdt();
  void init_tss();

} // namespace arch
