

int write(int fd,char *msg , int size)
{
	asm("movl $1,%%eax;"
	    "movl %0,%%edi;"
	    "movl %1,%%esi;"
	    "movl %2,%%edx;"
	    "syscall;"
	    :
   	    :"ir"(fd),"m"(msg) ,"m"(size)
	    :
		);
}

int open (const char *filename, int flags,int mode)
{
  asm("movl $2, %%eax;"
       "movl %0,%%edi;"
         "movl %1,%%esi;"
         "movl %2,%%edx;"
         "syscall;"
	:
	:"m"(filename),"m"(flags),"m"(mode)
	:
	);
}


int close(unsigned int fd)
{
     asm("movl $3,%%eax;"
	"movl %0,%%edi;"
        "syscall;"
	:
	:"ir"(fd)
	:
	);
}

int read( int fd,char *msg ,int size)
{
	asm("movl $0, %%eax;"
       	"movl %0, %%edi;"
	"movl %1,%%esi;"
	"movl %2,%%edx;"
	"syscall;"
	:
	:"ir"(fd),"m"(msg),"m"(size)
	:
	);
}

int unlink( const char *pathname)
{

     asm("movl $87, %%eax;"
	"movl %0,%%edi;"
	"syscall;"
	:
	: "m"(pathname)
	:
	);
}

int chdir(const char *filename)
{
	asm (
	"movl $80, %%eax;"
	"movl %0,%%edi;"
	"syscall;"
	:
	:"m"(filename)
	:
	);
}

int getcwd ( char *buf , unsigned long size)
{
	asm ("movl $79, %%eax;"
	"movl %0,%%edi;"
	"movl %1,%%esi;"
	"syscall;"
	:
	:"m"(buf),"m"(size)
	:
	);
}

int fork()
{
	asm(
	"movl $57, %%eax;"
	"syscall;"
	:
	:
	:
	);
}


int execve( const char *filename , const char *const argv[],const char *const envp[]  )
{
	asm ("movl $59, %%eax;"
	"movl %0,%%edi;"
	"movl %1,%%esi;"
	"movl %2,%%edx;"
	"syscall;"
	:
	:"m" (filename), "m"(argv),"m"(envp)
	:"memory"
	);
}


//Ask nirvik
/*waitpid()
"movl %0,%%edi;"
"movl %1,%%esi;"
"movl %2,%%edx;"
"syscall;"
*/

int getpid()
{
	asm ("movl $39, %%eax;"
	"syscall;"
	:
	:
	:
	);
}

int getppid()
{
	asm ("movl $110, %%eax;"
	"syscall;"
	:
	:
	:
	);
}


int lseek(unsigned int fd , int offset,unsigned int origin)
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

int mkdir(const char *pathname,int mode)
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
int pipe(int *filedes)
{
	asm ("movl $22 , %%eax;"
	"movl %0,%%edi;"
	:
	:"m"(filedes)
	:
	);
}
int main ()
{
return 0;
}


