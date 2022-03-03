#include <stdint.h>

#include "../mem/phys.h"
#include "../mem/virt.h"
#include "../proc/cpu.h"
#include "../utils/print.h"
#include "../utils/spin.h"
#include "../utils/utils.h"
#include "gdt.h"

typedef struct {
  uint16_t limit;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t flags;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
  gdt_entry_t low;
  uint32_t base_high_ex;
  uint32_t reserved;
} __attribute__((packed)) gdt_entry_ex_t;

typedef struct {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed)) gdtr_t;

static spin_lock_t gdt_lock;

// thanks @mintsuki (https://github.com/limine-bootloader/limine/blob/trunk/common/sys/gdt.s2.c)
static gdt_entry_t gdt_entries[] = {
  {0},
  {0xffff, 0x0000, 0x00, 0b10011010, 0b00000000, 0x00},
  {0xffff, 0x0000, 0x00, 0b10010010, 0b00000000, 0x00},
  {0xffff, 0x0000, 0x00, 0b10011010, 0b11001111, 0x00},
  {0xffff, 0x0000, 0x00, 0b10010010, 0b11001111, 0x00},
  {0x0000, 0x0000, 0x00, 0b10011010, 0b00100000, 0x00},
  {0x0000, 0x0000, 0x00, 0b10010010, 0b00000000, 0x00},
  {0x0000, 0x0000, 0x00, 0b11111010, 0b00100000, 0x00},
  {0x0000, 0x0000, 0x00, 0b11110010, 0b00000000, 0x00},
  {0}, // tss entries, filled at runtime
  {0},
};

// TODO: Instead of having a single TSS and locking to modify
// the TSS entry we should move the GDT itself to the per-CPU data
// and that way we can get rid of locking whenever we initialize
// load the TSS.

void gdt_init() {
  gdtr_t gdtr;

  gdtr.limit = sizeof(gdt_entries) - 1;
  gdtr.base = (uintptr_t) gdt_entries;

  asm("lgdt %0" : : "m"(gdtr) : "memory");
}

void gdt_init_tss() {
  cpu_info_t *cpu = cpu_get();

  gdt_entry_ex_t *tss_entry = (gdt_entry_ex_t *) &gdt_entries[GDT_TSS / 8];

  uintptr_t tss_address = (uintptr_t) &cpu->tss;

  spin_lock(&gdt_lock);

  tss_entry->low = (gdt_entry_t){
    .limit = sizeof(tss_t) - 1,
    .base_low = (uint16_t) tss_address,
    .base_mid = (uint8_t) (tss_address >> 16),
    .flags = 0b10001001,
    .granularity = 0x00,
    .base_high = (uint8_t) (tss_address >> 24),
  };

  tss_entry->base_high_ex = (uint32_t) (tss_address >> 32);
  tss_entry->reserved = 0;

  cpu->tss.rsp[0] = phys_to_virt(phys_alloc(2)) + KIB(8);

  asm("ltr %0" : : "r"((uint16_t) GDT_TSS));

  spin_unlock(&gdt_lock);
}
