#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <sys/defs.h>

void syscall_handler();
void init_syscall();
void cpuSetMSR(uint32_t msr, uint32_t lo , uint32_t hi);
#endif
