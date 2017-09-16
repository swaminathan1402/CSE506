.global idt_lidt 

idt_lidt:
movq 8(%rsp),%rax
lidt (%rax) 
retq
