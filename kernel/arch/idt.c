#include <stddef.h>
#include <stdint.h>

#include "gdt.h"
#include "idt.h"

#define IDTE_P 1 << 7
#define IDTE_U 3 << 5
#define IDTE_INT_GATE 0b1110

typedef struct {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t flags;
  uint16_t offset_mid;
  uint32_t offset_high;
  uint32_t reserved;
} __attribute__((packed)) interrupt_desc_t;

typedef struct {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed)) idtr_t;

static interrupt_desc_t idt_entries[256];

static interrupt_desc_t simple_entry(uint64_t offset, uint8_t ist, uint8_t flags) {
  interrupt_desc_t entry;

  entry.offset_low = offset & 0xffff;
  entry.selector = GDT_CODE_64;
  entry.ist = ist;
  entry.flags = flags;
  entry.offset_mid = (offset >> 16) & 0xffff;
  entry.offset_high = (offset >> 32) & 0xffffffff;
  entry.reserved = 0;

  return entry;
}

void idt_init() {
  extern uint64_t isr_stubs[256];

  for (size_t i = 0; i < 256; i++) {
    idt_entries[i] = simple_entry(isr_stubs[i], 0, IDTE_P | IDTE_INT_GATE);
  }

  idt_entries[0x80] = simple_entry(isr_stubs[0x80], 0, IDTE_P | IDTE_U | IDTE_INT_GATE);

  idtr_t idtr;

  idtr.limit = sizeof(idt_entries) - 1;
  idtr.base = (uintptr_t) idt_entries;

  asm("cli" ::: "memory");
  asm("lidt %0" : : "m"(idtr) : "memory");
}
