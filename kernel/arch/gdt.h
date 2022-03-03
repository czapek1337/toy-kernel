#pragma once

#include <stdint.h>

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

typedef struct {
  uint32_t reserved;
  uint64_t rsp[3];
  uint64_t reserved0;
  uint64_t ist[7];
  uint32_t reserved1;
  uint32_t reserved2;
  uint16_t reserved3;
  uint16_t iopb_offset;
} __attribute__((packed)) tss_t;

void gdt_init();
void gdt_init_tss();
