#include<stdio.h>


int syscall_write(int fd, char *buffer, int count){


    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    long long int ret;


    __asm__ __volatile__(
        "movq $1, %%rax;"
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
       	"int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rsi", "rdi", "rdx"
    );
    return ret;
}

int syscall_kill_pid(int pid){
	long long int ret;
	__asm__ __volatile__(
		"movq $124, %%rax;"
		"movq %1, %%rdi;"
		"int $0x80;"
		"movq %%rax, %0;"
		:"=r"(ret)
		:"m"(pid)
		:"rax","rdi"
	);
	return (signed)ret;
}

int main(int argc, char *argv[], char *envp[]){
  int kill_pid = argv[1] - '0';
  int status = syscall_kill_pid(kill_pid);
  if(status != -1)
  	syscall_write(0, "[User]: Killed!\n",17);
  return 0;
}
