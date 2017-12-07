#include<stdio.h>
#include <stdlib.h>
int syscall_execvpe(const char *filename , char *const argv[], char *const envp[] ){
	long long int filename1= (long long int)filename;
	long long int argv1 = (long long int) argv;	
	long long int envp1= (long long int) envp;	
	long long int ret;
	__asm__ __volatile__(
		"movq $59, %%rax;"
		"movq %1,%%rbx;"
		"movq %2,%%rcx;"
		"movq %3,%%rdx;"
		"syscall;"
		"movq %%rax, %0;"
		:"=r" (ret)
		:"r" (filename1), "r"(argv1), "r" (envp1)
		:"rax", "rbx", "rcx", "rdx"
	);
	return ret;
	
}
int syscall_write(int fd, char *buffer, size_t count){


    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    long long int ret;


    __asm__(

        "movq $1, %%rax;"
        "movq %1, %%rbx;"
        "movq %2, %%rcx;"
        "movq %3, %%rdx;"
        "syscall;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rbx", "rcx", "rdx"
    );
    return ret;
}


int main(int args, char *argv[], char *envp[]){

	char *args_1[] = {"./hello", NULL, NULL};
	syscall_execvpe(args_1[0], args_1, NULL);

	return 0;
}
