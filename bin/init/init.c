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

int yield(){
	__asm__ __volatile__(
		
		"movq $24, %%rax;"
		"int $0x80;"
		:
		:
		:"rax"
	);
	return 1;
}



int main(){
	syscall_write(0,"sbush>",6);
	while(1){
		yield();
	}
	return 0;
}
