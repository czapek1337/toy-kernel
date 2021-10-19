global update_gdt:function
global update_tss:function

; rdi - gdtr pointer
; rsi - code selector
; rdx - data selector
update_gdt:
    lgdt [rdi]

    mov ss, dx
    mov ds, dx
    mov es, dx

    lea rax, [rel .trampoline]

    push rsi
    push rax

    retfq

.trampoline:
    ret

; rdi - tss selector
update_tss:
    ltr di
    ret
