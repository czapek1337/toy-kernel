#include <stddef.h>
#include <stdint.h>

#include "idt.h"

#define IDTE_P 1 << 7
#define IDTE_INT_GATE 0b1110

typedef struct __attribute__((packed)) {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t flags;
  uint16_t offset_mid;
  uint32_t offset_high;
  uint32_t reserved;
} interrupt_desc_t;

typedef struct __attribute__((packed)) {
  uint16_t limit;
  uint64_t base;
} idtr_t;

static interrupt_desc_t idt_desc[256];

void idt_init() {
  extern uint64_t isr_stubs[256];

  for (size_t i = 0; i < 256; i++) {
    idt_desc[i].offset_low = isr_stubs[i] & 0xffff;
    idt_desc[i].selector = 0x28;
    idt_desc[i].ist = 0;
    idt_desc[i].flags = IDTE_P | IDTE_INT_GATE;
    idt_desc[i].offset_mid = (isr_stubs[i] >> 16) & 0xffff;
    idt_desc[i].offset_high = (isr_stubs[i] >> 32) & 0xffffffff;
    idt_desc[i].reserved = 0;
  }

  idtr_t idtr;

  idtr.limit = sizeof(idt_desc) - 1;
  idtr.base = (uint64_t) idt_desc;

  asm("lidt %0" : : "m"(idtr) : "memory");
  asm("sti" ::: "memory");
}

void idt_handle_interrupt(isr_frame_t *frame) {
  (void) frame;

  while (1) {
    asm("hlt");
  }
}
