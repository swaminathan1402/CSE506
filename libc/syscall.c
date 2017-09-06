#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/mman.h>
// works
int syscall_write(int fd, const void *msg , size_t size)
{
	asm("movl $1,%%eax;"
	    "movl %0,%%ebx;"
	    "movl %1,%%ecx;"
	    "movl %2,%%edx;"
	    "syscall;"
	    :
   	    :"ir"(fd),"m"(msg) ,"m"(size)
	    :
		);

}

int syscall_open (const char *filename, int flags,int mode)
{
  asm(
	"movl $2, %%eax;"
       	"movl %0,%%edi;"
        "movl %1,%%esi;"
        "movl %2,%%edx;"
        "syscall;"
	:
	:"m"(filename),"m"(flags),"m"(mode)
	:
	);
}


int syscall_close(unsigned int fd)
{
     asm("movl $3,%%eax;"
	"movl %0,%%edi;"
        "syscall;"
	:
	:"ir"(fd)
	:
	);
}

size_t syscall_read(int fd, void *msg ,size_t size)
{
	asm(
	"movl $0, %%eax;"
       	"movl %0, %%ebx;"
	"movl %1,%%ecx;"
	"movl %2,%%edx;"
	"syscall;"
	:
	:"ir"(fd),"m"(msg),"m"(size)
	:
	);
}

int syscall_unlink( const char *pathname)
{

     asm("movl $87, %%eax;"
	"movl %0,%%edi;"
	"syscall;"
	:
	: "m"(pathname)
	:
	);
}

int syscall_chdir(const char *filename)
{
	asm (
	"movl $80, %%eax;"
	"movl %0,%%ebx;"
	"syscall;"
	:
	:"m"(filename)
	:
	);
}

char* syscall_getcwd ( char *buf , size_t size)
{
	asm (
	"movl $79, %%eax;"
	"movl %0,%%ebx;"
	"movl %1,%%ecx;"
	"syscall;"
	:"=m"(buf) 
	:"m"(buf), "m"(size)
	:
	);
}

void* syscall_mmap ( unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long offset){
    
        asm (
        "movl $9, %%eax;"
        "movl %0, %%ebx;"
        "movl %1, %%ecx;"
        "movl %2, %%edx;"
        "mov %3, %%r10;"
        "mov %4, %%r8;"
        "mov %5, %%r9;"
        "syscall;"
        :
        :"m" (addr), "m"(len), "m"(prot), "m"(flags), "m"(fd), "m"(offset)
        :
        );
}


//wrong
int syscall_munmap( unsigned long addr, size_t len)
{
    asm(
    "movl $11, %%eax;"
    "movl %0, %%ebx;"
    "movl %1, %%ecx;"
    "syscall;"
    :
    :"m"(addr), "m"(len)
    :
    );
}

pid_t syscall_fork()
{
	asm(
	"movl $57, %%eax;"
	"syscall;"
	:
	:
	:
	);
}

// doesnt work
int syscall_execvpe( const char *filename , char *const argv[], char *const envp[]  )
{

	asm (
	"movl $59, %%eax;"
	"movl %0,%%ebx;"
	"movl %1,%%ecx;"
	"movl %2,%%edx;"
	"syscall;"
	:
	:"m" (filename), "m"(argv), "m" (envp)
	:
	);
}


//TODO pid_t issue
pid_t syscall_waitpid(pid_t pid, int *status, int options) {
	asm(
		"movl $61,%%eax;"
		"movl %0,%%ebx;"
		"movl %1,%%ecx;"
		"movl %2, %%edx;"
		"mov $0, %%r10;"
		"syscall;"
		: 
		: "m"(pid), "m"(status), "m"(options)
		:
	);
}



pid_t syscall_getpid()
{
	asm ("movl $39, %%eax;"
	"syscall;"
	:
	:
	:
	);
}

pid_t syscall_getppid()
{
	asm ("movl $110, %%eax;"
	"syscall;"
	:
	:
	:
	);
}


int syscall_lseek(unsigned int fd , int offset,unsigned int origin)
{
	asm("movl $8 ,%%eax;" 
	"movl %0,%%edi;"
	"movl %1,%%esi;"
	"movl %2,%%edx;"
	"syscall;"
	:
	:"ir"(fd),"m" (offset),"m" (origin)
	:
	);
}

int syscall_mkdir(const char *pathname,int mode)
{
	asm(
	"movl $83, %%eax;"
	"movl %0,%%edi;"
	"movl %1,%%esi;"
	"syscall;"
	:
	:"m"(pathname), "m"(mode)
	:
	);
}


int syscall_pipe(int *filedes)
{
	asm ("movl $22 , %%eax;"
	"movl %0,%%ebx;"
	:
	:"m"(filedes)
	:
	);
}

int syscall_exit(int status){
	printf("Exit status %d", status);	
	asm(
	"movl $60, %%eax;"
	"movl %0, %%ebx;"
	"syscall;"
	:
	: "m"(status)
	:
	);
}

int syscall_dup2(unsigned int old_fd, unsigned int new_fd){
	asm(
		"movl $33, %%eax;"
		"movl %0, %%ebx;"
		"movl %1, %%ecx;"
		"syscall;"
		:
			: "m"(old_fd), "m"(new_fd)
		:
	);
}

void* malloc (size_t size){
    size_t *mem_pointer;
    mem_pointer = syscall_mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    *mem_pointer = size;
    return (void*)(&mem_pointer[1]);
}

/*
void free (void* mem_pointer){
    int *pointer_length = (int*)mem_pointer;
    size_t size;

    size = *pointer_length;
    syscall_munmap((void*)pointer_length, size);
    printf("Custom Free Call\n");
}
*/

int main ()
{
    char* test = (char*)malloc(1024*sizeof(char*));
    char* tester = "Copy This sjnc d noidcjcn ca jnocnvobuabdvubqubvaubvobjbvbjavb bub";
    strcpy(test, tester);
    printf("Test: %s\n", test);
    return 0;
}
