#include <stdio.h>
#include <string.h>
#include <syscall.h>


// works
int syscall_write(int fd, char* msg , size_t size)  // works
{
	int ret;
	  __asm__(
	    "movl $1,%%eax;"
	    "movl %1,%%ebx;"
	    "movl %2,%%ecx;"
	    "movl %3,%%edx;"
	    "syscall;"
	    "movl %%eax, %0;"
	    :"=m"(ret)
   	    :"m"(fd),"m"(msg) ,"m"(size)
	    :
		);
	return ret;
}

int syscall_open (const char *filename, int flags, int mode)
{
	int ret;	
	__asm__(
		"movl $2, %%eax;"
		"movl %1,%%ebx;"
		"movl %2,%%ecx;"
		"movl %3,%%edx;"
		"syscall;"
		"movl %%eax, %0;"
		:"=m"(ret)
		:"m"(filename),"m"(flags),"m"(mode)
		:
		);
	return ret;
}


int syscall_close(unsigned int fd)
{
	int ret;
     __asm__("movl $3,%%eax;"
	"movl %1,%%ebx;"
        "syscall;"
	"movl %%eax, %0;"
	:"=m"(ret)
	:"m"(fd)
	:
	);
	return ret;
}

char* syscall_read(int fd, char *msg ,size_t size)  // done 
{
	size_t ret;
	__asm__(
	"movl $0, %%eax;"
       	"movl %1, %%ebx;"
	"movl %2,%%ecx;"
	"movl %3,%%edx;"
	"syscall;"
	"movl %%eax, %0;"
	:"=m"(ret)
	:"m"(fd),"m"(msg),"m"(size)
	:
	);
	int i =0;
	while(msg[i] != '\n') i++;
	msg[i] = '\0';
	return msg;
}

int syscall_unlink( const char *pathname)
{

	int ret;
     __asm__("movl $87, %%eax;"
	"movl %1,%%ebx;"
	"syscall;"
	"movl %%eax, %0;"
	: "=m"(ret)
	: "m"(pathname)
	:
	);
	return ret;
}

int syscall_chdir(const char *filename)
{
	int ret;
	__asm__ (
	"movl $80, %%eax;"
	"movl %1,%%ebx;"
	"syscall;"
	"movl %%eax, %0;"
	:"=m"(ret)
	:"m"(filename)
	:
	);
	return ret;
}

char* syscall_getcwd ( char *buf , size_t size)
{
	__asm__ (
	"movl $79, %%eax;"
	"movl %0,%%ebx;"
	"movl %1,%%ecx;"
	"syscall;"
	:"=m"(buf) 
	:"m"(buf), "m"(size)
	:
	);
	return buf;
}

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
	"movl %%eax, %0;"
        :"=m" (ret)
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
    "movl %%eax, %0;"
    :"=m"(ret)
    :"m"(addr), "m"(len)
    :
    );
    return ret;
}

pid_t syscall_fork()  // works
{
	pid_t ret;
	__asm__(
	"movl $57, %%eax;"
	"syscall;"
	"movl %%eax, %0;"
	:"=a"(ret)
	:
	:
	);
	return ret;
}

int syscall_execvpe( const char *filename , char *const argv[], char *const envp[]  ) // works
{

	int ret;
	__asm__ (
	"movl $59, %%eax;"
	"movl %1,%%ebx;"
	"movl %2,%%ecx;"
	"movl %3,%%edx;"
	"syscall;"
	"movl %%eax, %0;"
	:"=m" (ret)
	:"m" (filename), "m"(argv), "m" (envp)
	:
	);
	return ret;
}


pid_t syscall_waitpid(pid_t pid, int *status, int options) {  //works
	pid_t ret;
	__asm__(
		"movl $61,%%eax;"
		"movl %1,%%ebx;"
		"movl %2,%%ecx;"
		"movl %3, %%edx;"
		"syscall;"
		"movl %%eax, %0;"
		: "=m" (ret)
		: "m"(pid), "m"(status), "m"(options)
		:
	);
	return pid;
}



pid_t syscall_getpid()
{
	pid_t ret;
	__asm__ ("movl $39, %%eax;"
	"syscall;"
	"movl %%eax, %0;"
	:"=m"(ret)
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
	"movl %%eax, %0;"
	:"=m"(ret)
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
	"movl %%eax, %0;"
	:"=m"(ret)
	:"m"(fd),"m" (offset),"m" (origin)
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
	"movl %%eax, %0;"
	:"=m"(ret)
	:"m"(pathname), "m"(mode)
	:
	);
	return ret;
}


int syscall_pipe(int *filedes) // works
{
	int ret;
	__asm__ (
	"movl $22 , %%eax;"
	"movl %1,%%ebx;"
	"syscall;"
	"movl %%eax, %0;"
	:"=m"(ret)
	:"m"(filedes)
	:
	);
	return ret;
}

int syscall_exit(int status){  //works
	int ret;
	__asm__(
	"movl $60, %%eax;"
	"movl %1, %%ebx;"
	"syscall;"
	"movl %%eax, %0;"
	: "=m"(ret)
	: "m"(status)
	:
	);
	return ret;
}

int syscall_dup2(unsigned int old_fd, unsigned int new_fd){  //works
	int ret;
	__asm__(
		"movl $33, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"syscall;"
		"movl %%eax, %0;"
		: "=m"(ret)
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
		"movl %%eax, %0;"
		:"=m"(ret)
		: "m"(fd), "m"(buffer), "m"(count)
		:
		);
	return ret;
}

/*
int main ()
{
	// pid works: verified
	pid_t pidOne = syscall_getpid();
	printf("pid: %d\n", pidOne);

	// cwd works: verified
	char *buffer = (char *)malloc(1024);
	int cwd = syscall_getcwd(buffer, 1024);
	printf("getcwd: %s %d\n", buffer, cwd);
	// execvpe works: verified
	char *ls[] = {"ls", "-ltr", NULL};
	char *cmd = "/bin/ls";
	int ok = syscall_execvpe(cmd, ls, NULL);
	printf("\n execvpe: %d", ok);

	// pipe works: verified
	int fd[2];
	int p = syscall_pipe(fd);
	printf("\npipe: %d", p);

	// write: working: verfied
	char *q = "hello world\n";
	syscall_write(1, q, strlen(q));

	// chdir: Works: verified
	const char *filename = "/home";
	int s = syscall_chdir(filename);
	printf("\nchdir: %d", s);
	// read: not working 
	char msg[1024] = "can you read me ?";
	int o = syscall_read(1, msg, strlen(msg));
	printf("%d \n", o);

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
*/

/*
void* malloc (size_t size){
    size_t *mem_pointer;
    mem_pointer = syscall_mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    *mem_pointer = size;
    return (void*)(&mem_pointer[1]);
}
*/
/*
void free (void* mem_pointer){
    int *pointer_length = (int*)mem_pointer;
    size_t size;

    size = *pointer_length;
    syscall_munmap((void*)pointer_length, size);
    printf("Custom Free Call\n");
}
*/

/*
int main ()
{
    char* test = (char*)malloc(1024*sizeof(char*));
    char* tester = "Copy This sjnc d noidcjcn ca jnocnvobuabdvubqubvaubvobjbvbjavb bub";
    strcpy(test, tester);
    printf("Test: %s\n", test);
    return 0;
}
*/
