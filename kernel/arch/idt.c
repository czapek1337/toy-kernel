#include <stddef.h>
#include <stdint.h>

#include "gdt.h"
#include "idt.h"

#define IDTE_P 1 << 7
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

void idt_init() {
  extern uint64_t isr_stubs[256];

  for (size_t i = 0; i < 256; i++) {
    idt_entries[i].offset_low = isr_stubs[i] & 0xffff;
    idt_entries[i].selector = GDT_CODE_64;
    idt_entries[i].ist = 0;
    idt_entries[i].flags = IDTE_P | IDTE_INT_GATE;
    idt_entries[i].offset_mid = (isr_stubs[i] >> 16) & 0xffff;
    idt_entries[i].offset_high = (isr_stubs[i] >> 32) & 0xffffffff;
    idt_entries[i].reserved = 0;
  }

  idtr_t idtr;

  idtr.limit = sizeof(idt_entries) - 1;
  idtr.base = (uintptr_t) idt_entries;

  asm("lidt %0" : : "m"(idtr) : "memory");
  asm("sti" ::: "memory");
}
