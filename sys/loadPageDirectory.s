.text 
.globl loadPageDirectory
loadPageDirectory:
push %rbp
movq %rsp , %rbp
movq %rax , %cr3
movq %rbp , %rsp
pop %rbp
ret

