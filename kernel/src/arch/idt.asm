global update_idt:function

; rdi - idtr pointer
update_idt:
    lidt  [rdi]
    ret
