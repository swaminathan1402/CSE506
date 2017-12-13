#include <stdlib.h>
void _start(void) {
 int argc = -7;
  char* argv[] = {"None"};
  char* envp[] = {"None"};
/*
__asm__(
    "addq $0x28,%%rsp;"
    "movq (%%rsp),%%rdi;"
    "movq 8(%%rsp), %%rsi;"
    "movq 16(%%rsp,%%rdi,8 ),%%rdx;"
    "movq %%rdi, %0;"
    "movq %%rsi, %1;"
    "movq %%rdx, %2;"
    :"=m"(argc), "=m"(argv), "=m"(envp)
    :
    :"memory"
  );
*/
  main(argc, argv, envp);
  __asm__(
     "movq $60, %%rax;"
	"movq $1, %%rbx;"
	"int $0x80;"
	:
	:
	:
	);

}
















