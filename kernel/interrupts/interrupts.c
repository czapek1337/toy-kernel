#include "interrupts.h"
#include "../utils/print.h"
#include "../utils/spin.h"
#include "apic.h"

static size_t vector_alloc = 32;
static spin_lock_t vector_alloc_lock;
static isr_handler_t isr_handlers[256 - 32];

void interrupt_handle(isr_frame_t *frame) {
  if (frame->vec < 32)
    klog_panic("An unexpected exception occurred: %x", frame->vec);

  isr_handler_t handler = isr_handlers[frame->vec - 32];

  if (!handler)
    klog_panic("An unexpected interrupt arrived: %x", frame->vec);

  handler(frame);

  apic_eoi();
}

void interrupt_register(size_t vector, isr_handler_t handler) {
  vector -= 32;

  assert_msg(isr_handlers[vector] == 0, "Tried to register a handler for an existing interrupt vector %d", vector);

  isr_handlers[vector] = handler;
}

size_t interrupt_alloc_vec() {
  spin_lock(&vector_alloc_lock);

  assert_msg(vector_alloc < 0xf0, "Ran out of available interrupt vectors to allocate");

  size_t vector = vector_alloc++;

  spin_unlock(&vector_alloc_lock);

  return vector;
}
