.global syscall_wrapper
.extern parent_rsp 
.align 8 
syscall_wrapper:
movq %rsp, parent_rsp 
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

