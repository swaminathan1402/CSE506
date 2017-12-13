#include <stdio.h>
#include <string.h>
#include <syscall.h>


// works



void* syscall_mmap ( unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long offset){
	void *ret;    
        __asm__ (
        "movl $9, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "movl %3, %%edx;"
        "mov %4, %%r10;"
        "mov %5, %%r8;"
        "mov %6, %%r9;"
        "syscall;"
        :"=a" (ret)
        :"m" (addr), "m"(len), "m"(prot), "m"(flags), "m"(fd), "m"(offset)
        :
        );
	return ret;
}


//wrong
int syscall_munmap( unsigned long addr, size_t len)
{
    int ret;
    __asm__(
    "movl $11, %%eax;"
    "movl %1, %%ebx;"
    "movl %2, %%ecx;"
    "syscall;"
    :"=a"(ret)
    :"m"(addr), "m"(len)
    :
    );
    return ret;
}



int syscall_fork(){
    long long int ret;
    __asm__ __volatile__(
        "movq $57, %%rax;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :
        :"rax"
    );
    return ret;
}

int syscall_exit(int status) {
	long long int status1 = (long long int) status;
	long long int ret;
	__asm__(
	"movq $60, %%rax;"
	"movq %1, %%rbx;"
	"int $0x80;"
	"movq %%rax, %0;"
	: "=r"(ret)
	: "r"(status1)
	:"rax", "rbx"
	);
	return ret;

}


int syscall_waitpid(int pid, int *status, int options){
	long long int pid1 = (long long int) pid;
	long long int options1 = (long long int) options;
	long long int ret;

	__asm__ __volatile__(
		"movq $61,%%rax;"
		"movq %1,%%rdi;"
		"movq %2,%%rsi;"
		"movq %3, %%rdx;"
		"int $0x80;"
		"movq %%rax, %0;"
		: "=r" (ret)
		: "r"(pid1), "r"(status), "r"(options1)
		: "rax", "rdi", "rsi", "rdx"
	);
	return ret;

}

pid_t syscall_getpid()
{
	pid_t ret;
	__asm__ ("movl $39, %%eax;"
	"syscall;"
	:"=a"(ret)
	:
	:
	);
	return ret;
}

pid_t syscall_getppid()
{
	pid_t ret;
	__asm__ ("movl $110, %%eax;"
	"syscall;"
	:"=a"(ret)
	:
	:
	);
	return ret;
}


int syscall_lseek(unsigned int fd , int offset,unsigned int origin)
{
	int ret;
	__asm__("movl $8 ,%%eax;" 
	"movl %1,%%ebx;"
	"movl %2,%%ecx;"
	"movl %3,%%edx;"
	"syscall;"
	:"=a"(ret)
	:"ir"(fd),"m" (offset),"m" (origin)
	:
	);
	return ret;
}

int syscall_mkdir(const char *pathname,int mode)
{
	int ret;
	__asm__(
	"movl $83, %%eax;"
	"movl %1,%%ebx;"
	"movl %2,%%ecx;"
	"syscall;"
	:"=a"(ret)
	:"m"(pathname), "m"(mode)
	:
	);
	return ret;
}


int syscall_pipe(int *filedes)
{
	int ret;
	__asm__ ("movl $22 , %%eax;"
	"movl %1,%%ebx;"
	"syscall;"
	"movl %%eax, %0;"
	:"=m"(ret)
	:"m"(filedes)
	:
	);
	return ret;
}

int syscall_dup2(unsigned int old_fd, unsigned int new_fd){
	int ret;
	__asm__(
		"movl $33, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"syscall;"
		"movl %%eax, %0;"
		: "=a"(ret)
		: "m"(old_fd), "m"(new_fd)
		:
	);
	return ret;
}

int syscall_getdents(int fd, char *buffer, unsigned int count){
	int ret;
	__asm__(
		"movl $78, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"movl %3, %%edx;"
		"syscall;"
		:"=a"(ret)
		: "m"(fd), "m"(buffer), "m"(count)
		:
		);
	return ret;
}


int syscall_chdir(const char *filename){
	long long int filename1 = (long long int) filename;
	long long int ret;
	__asm__ (
	"movq $80, %%rax;"
	"movq %1,%%rbx;"
	"syscall;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(filename1)
	:"rax", "rbx"
	);
	return ret;
}
/*
size_t syscall_read(int fd, char *buffer, size_t count){
   
    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    
    long long int ret;
    __asm__(

        "movq $0, %%rax;"
        "movq %1, %%rbx;"
        "movq %2, %%rcx;"
        "movq %3, %%rdx;"
        "syscall;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rbx", "rcx", "rdx"
    );
    if(fd == 0){
	    int i = 0;
	    while(buffer[i] != '\n') i++;
	    buffer[i] = '\0';
    }
    return ret;
}
*/

int syscall_read(int fd, char *buffer, int count){
   
    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    
    long long int ret;
    __asm__ __volatile(
        "movq $0, %%rax;"
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rdi", "rsi", "rdx"
    );
    if(fd == 0){
	    int i = 0;
	    while(buffer[i] != '\n') i++;
	    buffer[i] = '\0';
    }
    return ret;
}


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
char* syscall_getcwd (char *buf , size_t size) {

	long long int buf1 = (long long int) buf;
	long long int size1 = (long long int) size;
	__asm__ __volatile__(

		"movq $79, %%rax;"
		"movq %1,%%rbx;"
		"movq %2,%%rcx;"
		"syscall;"
		"movq %%rax, %0;"
		:"=m"(buf1)
		:"r"(buf1), "r"(size1)
		:"rax", "rbx", "rcx"
	);
	return buf;
}



int syscall_execvpe(const char *filename , char *const argv[], char *const envp[] ){
	long long int filename1= (long long int)filename; // random_binary 
	long long int argv1 = (long long int) argv;
	long long int envp1= (long long int) envp;	
	long long int ret;
	__asm__ __volatile__(
		"movq $59, %%rax;"
		"movq %1,%%rdi;"
		"movq %2,%%rsi;"
		"movq %3,%%rdx;"
		"int $0x80;"
		"movq %%rax, %0;"
		:"=r" (ret)
		:"r" (filename1), "r"(argv1), "r" (envp1)
		:"rax", "rdi", "rsi", "rdx"
	);
	return ret;
	
}



int syscall_close(unsigned int fd)
{
    long long int ret;
    long long int fd1 = (long long int) fd;

     __asm__("movq $3,%%rax;"
	"movq %1,%%rbx;"
        "syscall;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(fd1)
	:"rax", "rbx"
	);
	return ret;
}

int syscall_open (const char* file, int flags) {
	// returns the file descriptor
	long long int file1 =(long long int) file;
	long long int flags1 = (long long int)flags;
	long long int ret;
	
	__asm__ __volatile__(
		"movq $2, %%rax;"
		"movq %1 ,%%rbx;"
		"movq %2, %%rcx;"
		"movq $0 ,%%rdx;"
		"syscall;"
		"movq %%rax, %0;"
		:"=r"(ret)
		:"r" (file1), "r"(flags1)
		:"rax","rbx","rcx","rdx"
	);
	return ret;
}
