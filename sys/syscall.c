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
kprintf("Hello Syscall");
int option ;

/*__asm__ __volatile __(
"swapgs;"
"movq %%rsp , %%gs:8; "
"movq %%gs:0, %%rsp;"
"push %%gs:8;"
"swapgs;"
:
:
:
);
*/

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
"movq %%rax, %0"
:"=m"(option)
:
:
);
//kprintf("Hello Syscall");
switch (option)
{
 case 0: //Sys read 
 break;	
case 1: //sys write
break; 
case 2:// sys_open
break;	
case 3: //sys_close
break;
case 8://sys_lseek 
break;

case 9: //sys_mmap
break;
case 11: //sys_munmap
break;
case 22: //sys_pipe
break ;
default :
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

