#include <stdlib.h>
#include<stdio.h>
void _start(void) {
 int argc = -7;
  char* argv[] = {"None"};
  char* envp[] = {"None"};

__asm__ __volatile__(
    //"addq $0x18,%%rsp;"
    "subq $0x10 , %%rsp ;"
    "movq (%%rsp),%%rdi;"
    "movq 8(%%rsp), %%rsi;"
  //  "movq 16(%%rsp,%%rdi,8 ),%%rdx;"
    "movq %%rdi, %0;"
    "movq %%rsi, %1;"
  //  "movq %%rdx, %2;"
    :"=m"(argc), "=m"(argv)
    :
    :"memory"
  );
printf("%d, %s", argc , argv[0]);
//while(1);	

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
