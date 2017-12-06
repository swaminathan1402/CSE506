#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/page_table.h>
void page_fault()
{
uint64_t faulting_address;
__asm__ __volatile__(
	"mov %%cr2, %0"
	: "=r" (faulting_address)
	:
	:
);
//kprintf("\nPage fault!!");
//kprintf("%p", faulting_address);
setMap(faulting_address, faulting_address, 1);
//while(1);

}
