#include <stdlib.h>

void _start(void) {
  // call main() and exit() here
  //char *env[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, "PATH=/home/nghosh/bin:/home/nghosh/.local/bin:/shared/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games", NULL, NULL, "HOME=/home/nghosh", NULL};
  int argc = -7;
  __asm__(
    "movq 3(%%rsp), %%rdi;"
    "movq %%rdi, %0;"
    :"=m"(argc)
    :
    :
  );
  main(argc, NULL, NULL);

  __asm__(
     "movl $60, %%eax;"
	"movl $1, %%ebx;"
	"syscall;"
	:
	:
	:
	);

}
