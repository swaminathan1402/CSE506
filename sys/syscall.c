#include <sys/defs.h>
#include<sys/kprintf.h>

void syscall_handler()
{

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
:
:
:
);
kprintf("Hello Syscall");
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

