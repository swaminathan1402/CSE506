#include <sys/defs.h>
#include<sys/kprintf.h>
#include<sys/syscall.h>
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
/*
__asm__ __volatile __(
"swapgs;"
"movq %%rsp , %%gs:8; "
"movq %%gs:0, %%rsp;"
"push %%gs:8;"
"swapgs;"
:
:
:
);*/

__asm__ __volatile__(
"cli;"
"push %%rax;"
"push %%rbx;"
"push %%rcx;"
"push %%rdx;"
"push %%rbp;"
"push %%rsi;"
"push %%rdi;"
"cld;"
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
//kprintf("Hello Syscall %d %d %s %d\n", rax, rdi, rsi, rdx);
switch (rax)
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
kprintf("%s",  buf );
break; 

case 2:// sys_open
filename = (char *)rdi; 
flags = (int)rsi;
mode =(int)rdx;
//kprintf("%s, %d, %d ", filename, flags, mode);
break;	

case 3: //sys_close
fd =(uint64_t)rdi;
kprintf("%d", fd);
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
break;
}


__asm__ __volatile__(
"pop %%rdi;"
"pop %%rsi;"
"pop %%rbp;"
"pop %%rdx;"
"pop %%rcx;"
"pop %%rbx;"
"pop %%rax;"
"sysretq;"
:
:
:
);

}

