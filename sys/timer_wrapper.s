.global timer_wrapper
.align 8

timer_wrapper:
	pushad
	cld
	call timer_handler
	popad
	iretq

