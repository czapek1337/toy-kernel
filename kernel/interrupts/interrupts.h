#pragma once

#include <stddef.h>
#include <stdint.h>

#define IRQ_TIMER 32
#define IRQ_SYSCALL 128
#define IRQ_SPURIOUS 255

namespace interrupts {

  struct isr_frame_t {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
    uint64_t vec, error, ip, cs, flags, sp, ss;
  };

  using isr_handler_t = void (*)(isr_frame_t *);

  void handle(isr_frame_t *frame);
  void register_handler(size_t vector, isr_handler_t handler);

  size_t alloc_vec();

} // namespace interrupts
