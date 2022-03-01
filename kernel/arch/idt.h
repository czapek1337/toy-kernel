#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
  uint64_t vec, error, ip, cs, flags, sp, ss;
} isr_frame_t;

void idt_init();
void idt_handle_interrupt(isr_frame_t *frame);
