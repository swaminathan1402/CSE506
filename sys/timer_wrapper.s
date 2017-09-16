.global timer_wrapper
.align 8
timer_wrapper:
	cli 
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rbp
	push %rsi
	push %rdi 
	cld
	call timer_handler
	pop %rdi
	pop %rsi
	pop %rbp
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax
	sti
	iretq
/*
timer_wrapper:
	iretq
*/
