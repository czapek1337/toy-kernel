#pragma once

#include <stddef.h>
#include <stdint.h>

#define IRQ_SYSCALL 128

typedef struct {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
  uint64_t vec, error, ip, cs, flags, sp, ss;
} isr_frame_t;

typedef void (*isr_handler_t)(isr_frame_t *);

void interrupt_handle(isr_frame_t *frame);
void interrupt_register(size_t vector, isr_handler_t handler);

size_t interrupt_alloc_vec();
