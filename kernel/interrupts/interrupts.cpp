#include <interrupts/apic.h>
#include <interrupts/interrupts.h>
#include <utils/print.h>
#include <utils/spin.h>

static size_t vector_alloc = 32;
static spin_lock_t vector_alloc_lock;
static interrupts::isr_handler_t isr_handlers[256 - 32];

extern "C" void interrupt_handle(interrupts::isr_frame_t *frame) {
  interrupts::handle(frame);
}

void interrupts::handle(isr_frame_t *frame) {
  if (frame->vec < 32)
    klog_panic("An unexpected exception occurred: %x", frame->vec);

  if (auto handler = isr_handlers[frame->vec - 32]) {
    handler(frame);
  } else {
    klog_panic("An unexpected interrupt arrived: %x", frame->vec);
  }

  apic::send_eoi();
}

void interrupts::register_handler(size_t vector, isr_handler_t handler) {
  assert_msg(isr_handlers[vector - 32] == 0, "Tried to register a handler for an existing interrupt vector %x", vector);

  isr_handlers[vector - 32] = handler;
}

size_t interrupts::alloc_vec() {
  spin_lock(&vector_alloc_lock);

alloc:
  assert_msg(vector_alloc < 0xf0, "Ran out of available interrupt vectors to allocate");

  auto vector = vector_alloc++;

  while (isr_handlers[vector] != 0)
    goto alloc;

  spin_unlock(&vector_alloc_lock);

  return vector;
}
