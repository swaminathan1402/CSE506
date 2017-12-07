.global syscall_wrapper
.align 8 
syscall_wrapper:

push %rax
push %rbx
push %rcx
push %rdx
push %rbp
push %rsi
push %rdi

call syscall_handler

pop %rdi
pop %rsi
pop %rbp
pop %rdx
pop %rcx
pop %rbx
pop %r11
sti 
iretq

