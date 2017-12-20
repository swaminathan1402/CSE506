#include <sys/defs.h>
#include<sys/kprintf.h>
#include<sys/syscall.h>
#include<sys/terminal_driver.h>
#include<sys/task.h>
#include<sys/filedirectory.h>
void cpuGetMSR(uint32_t msr, uint32_t*lo , uint32_t* hi)
{

__asm__ __volatile__(
"rdmsr;"
:"=a"(*lo), "=d"(*hi)
:"c"(msr)
:
);
}


void cpuSetMSR( uint32_t msr, uint32_t lo, uint32_t hi) 
{
__asm__ __volatile__(
"wrmsr;"
:
:"a"(lo), "c"(msr), "d"(hi)
:

);
}

void init_syscall()
{
	uint32_t syscall_low= (uint32_t)((uint64_t)syscall_handler&0x00000000ffffffff);
	uint32_t syscall_hi= (uint32_t)(((uint64_t)syscall_handler&0xffffffff00000000)>>32);
	uint32_t efer_low, efer_hi;
	uint32_t star_low, star_hi;
	cpuGetMSR(0xC0000080 , &efer_low , &efer_hi);  
	cpuSetMSR(0xC0000080,efer_low|0x1, efer_hi );//EFER SES SCE flag Set
	cpuGetMSR(0xC0000081, &star_low, &star_hi);
	cpuSetMSR(0xC0000081,star_low,(0x1b<<16|0x8));//Loading all K-CS . K_DS,U_CS,U_DS in STAR;
	cpuSetMSR(0xC0000082,syscall_low, syscall_hi);// Load syscall handler function in LSTAR.

}


void syscall_handler()
{

uint64_t rax,rdx,rsi ,rdi ,r10,r8, r9;
__asm__ __volatile__(
"movq %%rax, %0;"
"movq %%rdi,%1;"
"movq %%rsi, %2;"
"movq %%rdx, %3;"
"movq %%r10, %4;"
"movq %%r9 , %5;"
"movq %%r8 , %6;"
:"=m"(rax),"=m"(rdi), "=m"(rsi), "=m"(rdx), "=m"(r10), "=m"(r9), "=m"(r8) 
:
:
);
unsigned int fd;
char* buf;
char** arguments;
size_t count;
char* filename; 
int ret;
int flags;
int mode, waiting_pid;
int ps_pid, process_to_kill;
off_t offset;
unsigned int origin;
unsigned long addr ;
unsigned long len ;
unsigned long prot;
unsigned long flag;
//unsigned long off ;
//size_t length;
//int* filedes;
int bytes_page;
uint64_t allocated_page;
int sleep_time, child_process_id;
switch (rax)
{
case 0: //Sys read 
fd= (uint64_t)rdi;
buf =(char *)rsi;
count = (size_t)rdx;
//kprintf("%d, %d, %d ", fd, buf, count);

if(fd==0)
terminal_read(fd, buf, count);
else
{
ret =readfromFile(fd, buf, count);
  __asm__ __volatile__(
	"movq %0, %%rax;"
	:
	:"m"(ret)
	:
  );
}
break;	

case 1: //sys write
fd = (uint64_t)rdi;
buf= (char *)rsi;
count = (size_t )rdx;
// kprintf("%s, %d, %d ", fd, buf, count);
terminal_write(0, buf, count);
break; 

case 2:// sys_open
filename = (char *)rdi; 
flags = (int)rsi;
mode =(int)rdx;
//kprintf("\n Opening:%s  Flags: %d , Mode: %d", filename, flags, mode);
if(mode==0)
{
//int len = strlen(filename);
//filename[len] = '\0';
//kprintf("Searching for : %s" ,filename); 
int fd =search(filename);
//kprintf("Fd: %d" ,fd); 
if(fd!=-1)
{
increase_ref_count(fd);
} 
__asm__ __volatile__("movq %0, %%rax;"
:
:"m"(fd)
:
);

}
break;	

case 3: //sys_close
fd =(uint64_t)rdi;
decrease_ref_count(fd);
//kprintf("Closing: %d", fd);
break;

case 6:
sleep_time = (uint64_t)rdi;
set_sleep_timer(sleep_time);
//kprintf("\nsleep timer done\n");
break;

case 8://sys_lseek 
fd= (uint64_t)rdi;
offset = (off_t)rsi;
origin= (uint64_t)rdx;
kprintf("%d, %d, %d ", fd, offset, origin);
break;

case 9: //sys_mmap
 /*
 addr= (uint64_t)rdi;
 len =(uint64_t)rsi;
 prot=(uint64_t)rdx;
 flag= (uint64_t)r10;
 fd = (uint64_t)r8;
 off =(uint64_t)r9;
 */
  bytes_page = (int)rdi;
  allocated_page = allocate_page(bytes_page);
  //kprintf("[Kernel]: Genuinely returning %p\n", allocated_page);
  __asm__ __volatile__(
	"movq %0, %%rax;"
	:
	:"m"(allocated_page)
	:
  );
// kprintf("%p, %d, %d, %d , %d, %d", addr, len, prot, flag, fd, off);
break;
                                                                
case 11: //sys_munmap
 addr = (uint64_t)rdi;
 //length= (size_t)rsi;
 int status_1 = unmap(addr);
 //kprintf("[Kernel] Unmap %p\n ", status_1);
  __asm__ __volatile__(
	"movq %0, %%rax;"
	:
	:"m"(status_1)
	:
  );
break;

case 22: //sys_ps
 getprocessList();
break ;


case 24: // sys_yield
  //yield();
    __asm__ __volatile__ (
	"sti"
	:
	:
	:
    );
  if(runningTask->child){
  	//kprintf("[Kernel] Parent %d getting rid of %d\n", runningTask->pid, runningTask->child->pid);
  }
  removeFromRunningList(runningTask);
  addtoReadyList(runningTask);
  runningTask->child = NULL;
  temp_yield(0);
  break;

case 57:
  //kprintf("forking");
  child_process_id = fork();
  __asm__ __volatile__(
	"movq %0, %%rax;"
	:
	:"m"(child_process_id)
	:
  );

  break;

case 59:
  filename = (char *)rdi;
  arguments = (char **)rsi;
  //kprintf("execvping %s %s %s \n", filename, arguments[0], arguments[1]);
  exec(filename,arguments);
  break;
  /*
  argv = (char *)rsi;
  envp = (char *)rdx;
  */


case 60:
  // kprintf("WE HAVE TO EXIT NOW\n");
  removeTask();
break;


case 61: // waitpid
  waiting_pid = (int )rdi;
  waiting_on_pid(rdi);
  break;

case 78 ://sys_getdents
fd= (uint64_t)rdi;
buf =(char *)rsi;
count = (size_t)rdx;
ret =readDents(fd,buf);
__asm__ __volatile__(
"movq %0 , %%rax;"
:
:"m"(ret)
:
);
break;

case 79: //sys_getcwd(); 
buf=(char*)rdi ; 
count=(size_t)rsi;
buf =getCurrentPath(buf, count);
__asm__ __volatile__(
"movq %0 , %%rax"
:
:"m"(buf)
:
);
break;

case 80: //sys_chdir();
buf= (char*)rdi;
ret =setCurrentPath(buf);
__asm__ __volatile__(
"movq %0 , %%rax"
:
:"m"(ret)
:
);
break;

case 123:  // ps
  ps_pid = runningTask->pid;
  //kprintf("[Kernel]: pid is %d\n", ps_pid);
  __asm__ __volatile__(
	"movq %0, %%rax;"
	:
	:"m"(ps_pid)
	:
  );
  break;

case 124: //kill
  process_to_kill = (int )rdi;
  //kprintf("[Kernel]: Killing %d\n", process_to_kill);
  int status = kill_process(process_to_kill);
  //kprintf("[Kernel]: Kill Status %d\n", status);
  __asm__ __volatile__(
	"movq %0, %%rax;"
	:
	:"m"(status)
	:
  );
  break;
  
case 130: // clean zombies
	clean_zombies();
	break;

default :
kprintf("Syscall not handled yet");
break;
}
}





/*
void syscall_handler()
{

//Push general purpose registers onto userspace stack
__asm__ __volatile__(
"pushq %%rax;"
"pushq %%rbx;"
"pushq %%rcx;"
"pushq %%rdx;"
"pushq %%rbp;"
"pushq %%rsi;"
"pushq %%rdi;"
:
:
:
);
//Switch to kernel stack and store user space stack on top of the kernel stack.u
__asm__ __volatile__(
"movq   %%rsp ,%0;"
"movq %1 , %%rsp;"
:"=m"(u_rsp)
:"m"(kernel_rsp)
:
);

//Use kernel stack to store the system call no. and return the 
//Perform  syscall based on rax
__asm__ __volatile__(
"movq %%rsp , %0;"
"movq %1 , %%rsp;"
:"=m"(u_rsp)
:"m"(kernel_rsp)
:
);

__asm__ __volatile__(
"pop %%rdi;"
"pop %%rsi;"
"pop %%rbp;"
"pop %%rdx;"
"pop %%rcx;"
"pop %%rbx;"
"pop %%rax;"
:
:
:
);
//Perform sysretq
__asm__ __volatile__(
"sysretq;"
:
:
:
);
	uint64_t rax,rdx,rsi ,rdi ,r10,r8, r9;
	//Push general purpose registers onto userspace stack
	__asm__ __volatile__(
	"pushq %%rax;"
	"pushq %%rbx;"
	"pushq %%rcx;"
	"pushq %%rdx;"
	"pushq %%rbp;"
	"pushq %%rsi;"
	"pushq %%rdi;"
	:
	:
	:
	);

	int syscall_number;
	int dude;
	__asm__ __volatile__(
		"movq %%rax, %0;"
		:"=m"(syscall_number)
		:
		:
	);
	dude = syscall_number;
	//Switch to kernel stack and store user space stack on top of the kernel stack.u
	//kprintf("II Kernel RSP:%p\nUser RSP:%p\n", kernel_rsp, u_rsp);
	__asm__ __volatile__(
	"movq  %%rsp ,%0;"
	
	"movq %1 , %%rsp;"
	:"=m"(u_rsp)
	:"m"(kernel_rsp)
	:
	);
	//kprintf("II Kernel RSP:%p\nUser RSP:%p\n", kernel_rsp, u_rsp);

	uint64_t rax,rdx,rsi ,rdi ,r10,r8, r9;
	//Use kernel stack to store the system call no. and return the 
	__asm__ __volatile__(
	"movq %%rax, %0;"
	"movq %%rdi,%1;"
	"movq %%rsi, %2;"
	"movq %%rdx, %3;"
	"movq %%r10,%4;"
	"movq %%r9 , %5;"
	"movq %%r8 , %6;"
	:"=m"(rax),"=m"(rdi), "=m"(rsi), "=m"(rdx), "=m"(r10), "=m"(r9), "=m"(r8) 
	:
	:
	);

	//Perform  syscall based on rax
	unsigned int fd;
	char* buf ;
	size_t count;
	const char* filename; 
	int flags;
	int mode;
	off_t offset;
	unsigned int origin;
	unsigned long addr ;
	unsigned long len ;
	unsigned long prot;
	unsigned long flag;
	unsigned long off ;
	size_t length;
	int* filedes;
	switch (dude)
	{
	case 0: //Sys read 
	fd= (uint64_t)rdi;
	buf =(char *)rsi;
	count = (size_t)rdx;
	//kprintf("%d, %d, %d ", fd, buf, count);
	break;	

	case 1: //sys write
	fd = (uint64_t)rdi;
	buf= (char *)rsi;
	count = (int )rdx;
	//kprintf("%s\n",  buf );
	terminal_write(0, buf, count);
	break; 

	case 2:// sys_open
	filename = (char *)rdi; 
	flags = (int)rsi;
	mode =(int)rdx;
	//kprintf("%s, %d, %d ", filename, flags, mode);
	break;	

	case 3: //sys_close
	fd =(uint64_t)rdi;
	//kprintf("%d", fd);
	break;

	case 8://sys_lseek 
	fd= (uint64_t)rdi;
	offset = (off_t)rsi;
	origin= (uint64_t)rdx;
	//kprintf("%d, %d, %d ", fd, offset, origin);
	break;

	case 9: //sys_mmap
	 addr= (uint64_t)rdi;
	 len =(uint64_t)rsi;
	 prot=(uint64_t)rdx;
	 flag= (uint64_t)r10;
	 fd = (uint64_t)r8;
	 off =(uint64_t)r9;
	//kprintf("%p, %d, %d, %d , %d, %d", addr, len, prot, flag, fd, off);
	break;


	case 11: //sys_munmap
	 addr = (uint64_t)rdi;
	 length= (size_t)rsi;
	//kprintf("%p %d ", addr,length);
	break;

	case 22: //sys_pipe
	 filedes= (int *)rdi;
	//kprintf("%p" , filedes);
	break ;


	default :
	kprintf("Syscall not handled yet");
	while(1);
	break;
	}

	//Swap to user space stack and  perform sysretq


	//kprintf("III Kernel RSP:%p\nUser RSP:%p\n", kernel_rsp, u_rsp);
	__asm__ __volatile__(
	"movq %%rsp , %0;"
	"movq %1 , %%rsp;"
	:"=m"(kernel_rsp)
	:"m"(u_rsp)
	:
	);
	//kprintf("IV Kernel RSP:%p\nUser RSP:%p\n", kernel_rsp, u_rsp);

	__asm__ __volatile__(
	"pop %%rdi;"
	"pop %%rsi;"
	"pop %%rbp;"
	"pop %%rdx;"
	"pop %%rcx;"
	"pop %%rbx;"
	"pop %%rax;"
	"addq $0x48, %%rsp;"
	:
	:
	:
	);
	//Perform sysretq
	__asm__ __volatile__(
	"sysretq;"
	:
	:
	:
	);

}
*/
