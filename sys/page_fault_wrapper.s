.global page_fault_wrapper
.align 8
page_fault_wrapper:
cli
push %rax
push %rbx
push %rcx
push %rdx
push %rbp 
push %rsi 
push %rdi 
cld 
call page_fault
pop %rdi  //Fault code 
pop %rdi 
pop %rsi 
pop %rbp 
pop %rdx
pop %rcx
pop %rbx
pop %rax
sti 
iretq

