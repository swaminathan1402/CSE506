#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/page_table.h>
#include<sys/page.h>
#include<sys/memory.h>
void page_fault()
{
uint64_t faulting_address;
__asm__ __volatile__(
	"mov %%cr2, %0"
	: "=r" (faulting_address)
	:
	:
);

kprintf("\nErrcode : %p", err_code);
if(!(err_code & 0x1 ) && (err_code& 0x4))
//kprintf("\nPage fault!!");
{
	kprintf("\nPage not present in user mode");
	setMap(faulting_address, faulting_address, 1);
}
else if(!(err_code & 0x1) && !(err_code & 0x4) )
{
		kprintf("Page not present in supervisor mode");
		setMap(faulting_address , faulting_address ,1);
}
else if(err_code& 0x2)
{
	kprintf("\nRead only page");
	if(checkCOW(faulting_address)==1)
	{
		kprintf("%p", faulting_address);
		kprintf("COW bit is set" );
	    	uint64_t copy_on_write_page=get_free_page();		
	   	memcpy(copy_on_write_page , (faulting_address & 0xfffffffffffff000)  ,4096 ) ;
		setWritable( faulting_address, copy_on_write_page);
		setCOW(faulting_address, 0);
	}
	reloadCR3();	
}

//while(1);

}
