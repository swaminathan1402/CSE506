#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int syscall_read(int fd, char *buffer, size_t count){
   
    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    
    long long int ret;
    __asm__ __volatile__(

        "movq $0, %%rax;"
        "movq %1, %%rbx;"
        "movq %2, %%rcx;"
        "movq %3, %%rdx;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rbx", "rcx", "rdx"
    );
    /*
    int i = 0;
    while(buffer[i] != '\n') i++;
    buffer[i] = '\0';
    */
    return ret;
}

int syscall_write(int fd, char *buffer, size_t count){


    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    long long int ret;


    __asm__ __volatile__(

        "movq $1, %%rax;"
        "movq %1, %%rbx;"
        "movq %2, %%rcx;"
        "movq %3, %%rdx;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rbx", "rcx", "rdx"
    );
    return ret;
}

int syscall_open (const char *filename, int flags, int mode)
{
        long long int filename1= (long long int)filename ;
        long long int flags1=(long long int)flags;
        long long int mode1= (long long int)mode;
         long long int ret;
        __asm__(
                "movq $2, %%rax;"
                "movq %1,%%rbx;"
                "movq %2,%%rcx;"
                "movq %3,%%rdx;"
                "int $0x80;"
                "movq %%rax,%0"
                :"=r"(ret)
                :"r"(filename1),"r"(flags1),"r"(mode1)
                :"rax","rbx","rcx","rdx"
                );
        return ret;
}

int syscall_close(unsigned int fd)
{
    long long int ret;
    long long int fd1 = (long long int) fd;

     __asm__("movq $3,%%rax;"
	"movq %1,%%rbx;"
        "int $0x80;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(fd1)
	:"rax", "rbx"
	);
	return ret;
}

int main (int argc, char *argv[], char *envp[])
{
	char str[4096];
	char *strin = str;
	const char* filename=argv[1];
	int size;
	int fd;
	fd = syscall_open(filename, 0, 0);
	size=syscall_read(fd,strin, 4096);
	syscall_write(1,strin, size);
	syscall_close(fd);
	return 0;
}


