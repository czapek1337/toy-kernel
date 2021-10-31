global update_idt:function

update_idt:
    lidt [rdi]
    ret
