#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// works
int SYSCALL_write(int fd, const void *msg , size_t size)
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

int SYSCALL_open (const char *filename, int flags,int mode)
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


int SYSCALL_close(unsigned int fd)
{
     asm("movl $3,%%eax;"
	"movl %0,%%edi;"
        "syscall;"
	:
	:"ir"(fd)
	:
	);
}

size_t SYSCALL_read(int fd, void *msg ,size_t size)
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

int SYSCALL_unlink( const char *pathname)
{

     asm("movl $87, %%eax;"
	"movl %0,%%edi;"
	"syscall;"
	:
	: "m"(pathname)
	:
	);
}

int SYSCALL_chdir(const char *filename)
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

char* SYSCALL_getcwd ( char *buf , size_t size)
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

pid_t SYSCALL_fork()
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
int SYSCALL_execvpe( const char *filename , char *const argv[], char *const envp[]  )
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


//TODO
pid_t waitpid(pid_t pid, int *status, int options) {
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
	return 0;
}


// works
pid_t SYSCALL_getpid()
{
	asm ("movl $39, %%eax;"
	"syscall;"
	:
	:
	:
	);
}

pid_t SYSCALL_getppid()
{
	asm ("movl $110, %%eax;"
	"syscall;"
	:
	:
	:
	);
}


int SYSCALL_lseek(unsigned int fd , int offset,unsigned int origin)
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

int SYSCALL_mkdir(const char *pathname,int mode)
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

// works
int SYSCALL_pipe(int *filedes)
{
	asm ("movl $22 , %%eax;"
	"movl %0,%%ebx;"
	:
	:"m"(filedes)
	:
	);
}

int SYSCALL_exit(int status){
	
	asm(
	"movl $60, %%eax;"
	"movl %0, %%ebx;"
	"syscall;"
	:
	: "m"(status)
	:
	);
}

int main ()
{
	// pid works: verified
	pid_t pidOne = SYSCALL_getpid();
	printf("pid: %d\n", pidOne);

	// cwd works: verified
	/*
	char *buffer = (char *)malloc(1024);
	int cwd = SYSCALL_getcwd(buffer, 1024);
	printf("getcwd: %s %d\n", buffer, cwd);
	*/
	// execvpe works: verified
	/*
	char *ls[] = {"ls", "-ltr", NULL};
	char *cmd = "/bin/ls";
	int ok = SYSCALL_execvpe(cmd, ls, NULL);
	printf("\n execvpe: %d", ok);
	*/

	// pipe works: verified
	/*
	int fd[2];
	int p = SYSCALL_pipe(fd);
	printf("\npipe: %d", p);
	*/

	// write: working: verfied
	/* 
	char *q = "hello world\n";
	SYSCALL_write(1, q, strlen(q));
	*/

	// chdir: Works: verified
	/* 
	const char *filename = "/home";
	int s = SYSCALL_chdir(filename);
	printf("\nchdir: %d", s);
	*/ 
	// read: not working 
	/*
	char msg[1024] = "can you read me ?";
	int o = SYSCALL_read(1, msg, strlen(msg));
	printf("%d \n", o);
	*/

	pid_t pid = SYSCALL_fork();
	printf("%d is generated\n", pid);
	int status;
	if(pid == 0){
		char *ls[] = {"ls", "-ltr", NULL};
		char *cmd = "/bin/ls";
		int ret = SYSCALL_execvpe(cmd, ls, NULL);
		SYSCALL_exit(ret);
	} else if(pid > 0){
		printf("%d\n", pid);
		if(waitpid(pid, &status, 0) > 0){
			SYSCALL_write(1, "it worked\n", 10);
		} else {
			SYSCALL_write(1, "it didnt\n", 10);
		}
	}
return 0;
}
