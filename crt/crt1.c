#include <stdlib.h>

void _start(void) {
  // call main() and exit() here
  //char *env[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, "PATH=/home/nghosh/bin:/home/nghosh/.local/bin:/shared/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games", NULL, NULL, "HOME=/home/nghosh", NULL};
  int argc = -7;
  char* argv[] = {"None"};
  char* envp[] = {"None"};

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
  main(argc, argv, envp);

  __asm__(
     "movl $60, %%eax;"
	"movl $1, %%ebx;"
	"syscall;"
	:
	:
	:
	);

}
