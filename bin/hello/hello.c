#include<stdio.h>
int syscall_write(int fd, char *buffer, int count){


    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    long long int ret;


    __asm__(

        "movq $1, %%rax;"
        "movq %1, %%rsi;"
        "movq %2, %%rdi;"
        "movq %3, %%rdx;"
       	"int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rsi", "rdi", "rdx"
    );
    return ret;
}


int main(){
syscall_write(0, "hello world\n", 11); 
syscall_write(0, "hello world\n", 11); 
while(1);
return 0;
}
