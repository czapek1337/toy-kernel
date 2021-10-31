global update_gdt:function
global update_tss:function

update_gdt:
    lgdt [rdi]

    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax

    lea rax, [rel .trampoline]

    push qword 0x08
    push rax

    retfq

.trampoline:
    ret

update_tss:
    mov ax, 0x30
    ltr ax
    ret
