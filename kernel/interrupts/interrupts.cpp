#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>

#include <interrupts/apic.h>
#include <interrupts/interrupts.h>
#include <utils/print.h>

static size_t vector_alloc = 32;
static frg::ticket_spinlock vector_alloc_lock;
static interrupts::isr_handler_t isr_handlers[256 - 32];

extern "C" void interrupt_handle(interrupts::isr_frame_t *frame) {
  interrupts::handle(frame);
}

void interrupts::handle(isr_frame_t *frame) {
  if (frame->vec < 32)
    kpanic("An unexpected exception occurred: 0x{:x}", frame->vec);

  if (auto handler = isr_handlers[frame->vec - 32]) {
    handler(frame);
  } else {
    kpanic("An unexpected interrupt arrived: 0x{:x}", frame->vec);
  }

  apic::send_eoi();
}

void interrupts::register_handler(size_t vector, isr_handler_t handler) {
  kassert_msg(isr_handlers[vector - 32] == 0, "Tried to register a handler for an existing interrupt vector 0x{:x}", vector);

  isr_handlers[vector - 32] = handler;
}

size_t interrupts::alloc_vec() {
  frg::unique_lock lock(vector_alloc_lock);

alloc:
  kassert_msg(vector_alloc < 0xf0, "Ran out of available interrupt vectors to allocate");

  auto vector = vector_alloc++;

  while (isr_handlers[vector] != 0)
    goto alloc;

  return vector;
}
