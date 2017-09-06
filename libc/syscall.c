#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>
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


//TODO
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


// works
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

// works
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

int main ()
{
	// pid works: verified
	pid_t pidOne = syscall_getpid();
	printf("pid: %d\n", pidOne);

	// cwd works: verified
	/*
	char *buffer = (char *)malloc(1024);
	int cwd = syscall_getcwd(buffer, 1024);
	printf("getcwd: %s %d\n", buffer, cwd);
	*/
	// execvpe works: verified
	/*
	char *ls[] = {"ls", "-ltr", NULL};
	char *cmd = "/bin/ls";
	int ok = syscall_execvpe(cmd, ls, NULL);
	printf("\n execvpe: %d", ok);
	*/

	// pipe works: verified
	/*
	int fd[2];
	int p = syscall_pipe(fd);
	printf("\npipe: %d", p);
	*/

	// write: working: verfied
	/* 
	char *q = "hello world\n";
	syscall_write(1, q, strlen(q));
	*/

	// chdir: Works: verified
	/* 
	const char *filename = "/home";
	int s = syscall_chdir(filename);
	printf("\nchdir: %d", s);
	*/ 
	// read: not working 
	/*
	char msg[1024] = "can you read me ?";
	int o = syscall_read(1, msg, strlen(msg));
	printf("%d \n", o);
	*/

	pid_t pid = syscall_fork();
	printf("%d is generated\n", pid);
	int status;
	if(pid == 0){
		char *ls[] = {"ls", "-ltr", NULL};
		char *cmd = "/bin/ls";
		int ret = syscall_execvpe(cmd, ls, NULL);
		syscall_exit(ret);
	} else if(pid > 0){
		printf("%d\n", pid);
		if(syscall_waitpid(pid, &status, 0) > 0){
			syscall_write(1, "it worked\n", 10);
		} else {
			syscall_write(1, "it didnt\n", 10);
		}
		//printf("%d\n", syscall_waitpid(pid, &status, 0));
	}
return 0;
}
