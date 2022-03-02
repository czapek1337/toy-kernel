#include <stdint.h>

#include "gdt.h"

static uint64_t gdt_entries[] = {
  0x0000000000000000, // null
  0x00009a000000ffff, // 16-bit code
  0x000093000000ffff, // 16-bit data
  0x00cf9a000000ffff, // 32-bit code
  0x00cf93000000ffff, // 32-bit data
  0x00af9b000000ffff, // 64-bit code
  0x00af93000000ffff, // 64-bit data
  0x00affb000000ffff, // user-mode 64-bit code
  0x00aff3000000ffff, // user-mode 64-bit data
};

typedef struct {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed)) gdtr_t;

void gdt_init() {
  gdtr_t gdtr;

  gdtr.limit = sizeof(gdt_entries) - 1;
  gdtr.base = (uintptr_t) gdt_entries;

  asm("lgdt %0" : : "m"(gdtr) : "memory");
}
