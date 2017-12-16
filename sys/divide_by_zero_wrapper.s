.global divide_by_zero_wrapper
.align 8
divide_by_zero_wrapper:
	cli 
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rbp
	push %rsi
	push %rdi 
	cld
	call divide_by_zero_handler
	pop %rdi
	pop %rsi
	pop %rbp
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax
	sti
	iretq
