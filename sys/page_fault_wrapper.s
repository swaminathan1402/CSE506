.global page_fault_wrapper
.extern err_code
.align 8
page_fault_wrapper:
movq (%rsp), %r11
movq %r11, err_code
push %rax
push %rbx
push %rcx
push %rdx
push %rbp
push %rsi
push %rdi
call page_fault

pop %rdi
pop %rsi
pop %rbp 
pop %rdx
pop %rcx
pop %rbx
pop %rax
pop %r11
sti 
iretq

