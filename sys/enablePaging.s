.text
.globl enablePaging
enablePaging:
push %rbp
movq %rsp, %rbp
movq %cr0, %rax
or $80000000, %rax
movq %rax, %cr0
movq %rbp, %rsp
pop %rbp
ret

