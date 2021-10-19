%macro push_all 0
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

%macro pop_all 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro int_name 1
    dq stub_%1
%endmacro

%macro int_error 1
stub_%1:
    push %1
    jmp  stub_common
%endmacro

%macro int_no_error 1

stub_%1:
    push 0
    push %1
    jmp  stub_common
%endmacro

section .text

extern interrupt_handler

stub_common:
    cld
    push_all

    mov  rdi, rsp
    call interrupt_handler

    pop_all
    add rsp, 16

    iretq

int_no_error 0
int_no_error 1
int_no_error 2
int_no_error 3
int_no_error 4
int_no_error 5
int_no_error 6
int_no_error 7
int_error 8
int_no_error 9
int_error 10
int_error 11
int_error 12
int_error 13
int_error 14
int_no_error 15
int_no_error 16
int_error 17
int_no_error 18
int_no_error 19
int_no_error 20
int_no_error 21
int_no_error 22
int_no_error 23
int_no_error 24
int_no_error 25
int_no_error 26
int_no_error 27
int_no_error 28
int_no_error 29
int_error 30
int_no_error 31

%assign i 32
%rep 256 - 32
    int_no_error i
%assign i i + 1
%endrep

section .data

global interrupt_vectors

interrupt_vectors:

%assign i 0
%rep 256
    int_name i
%assign i i + 1
%endrep
