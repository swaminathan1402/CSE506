#include<sys/defs.h>
#include<sys/kprintf.h>
void page_fault()
{
uint64_t faulting_address;
__asm__ __volatile__("mov %%cr2, %0": "=r" (faulting_address));
kprintf("Page fault!!");
kprintf("%x", faulting_address);
while(1);

}
