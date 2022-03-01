%macro push_gpr 0
  push rax
  push rbx
  push rcx
  push rdx
  push rbp
  push rdi
  push rsi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
%endmacro

%macro pop_gpr 0
  push r15
  push r14
  push r13
  push r12
  push r11
  push rsi
  push r10
  push r9
  push r8
  push rdi
  push rbp
  push rdx
  push rcx
  push rbx
  push rax
%endmacro

%macro isr_stub_ref 1
  dq isr_stub_%1
%endmacro

%macro isr_stub_with_err 1
isr_stub_%1:
  push %1
  jmp isr_stub_common
%endmacro

%macro isr_stub_without_err 1
isr_stub_%1:
  push 0
  push %1
  jmp isr_stub_common
%endmacro

section .text

extern idt_handle_interrupt

isr_stub_common:
  push_gpr

  mov rdi, rsp
  call idt_handle_interrupt

  pop_gpr
  add rsp, 16
  iretq

isr_stub_without_err 0
isr_stub_without_err 1
isr_stub_without_err 2
isr_stub_without_err 3
isr_stub_without_err 4
isr_stub_without_err 5
isr_stub_without_err 6
isr_stub_without_err 7
isr_stub_with_err 8
isr_stub_without_err 9
isr_stub_with_err 10
isr_stub_with_err 11
isr_stub_with_err 12
isr_stub_with_err 13
isr_stub_with_err 14
isr_stub_without_err 15
isr_stub_without_err 16
isr_stub_with_err 17
isr_stub_without_err 18
isr_stub_without_err 19
isr_stub_without_err 20
isr_stub_without_err 21
isr_stub_without_err 22
isr_stub_without_err 23
isr_stub_without_err 24
isr_stub_without_err 25
isr_stub_without_err 26
isr_stub_without_err 27
isr_stub_without_err 28
isr_stub_without_err 29
isr_stub_with_err 30
isr_stub_without_err 31

%assign i 32
%rep 256 - 32
  isr_stub_without_err i
%assign i i + 1
%endrep

section .data

global isr_stubs

isr_stubs:

%assign i 0
%rep 256
  isr_stub_ref i
%assign i i + 1
%endrep
