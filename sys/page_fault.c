#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/page_table.h>
#include<sys/page.h>
#include<sys/memory.h>
#include<sys/task.h>
#include<sys/mm_struct.h>
#include<sys/vm_area_struct.h>
void page_fault()
{
uint64_t faulting_address;
__asm__ __volatile__(
	"mov %%cr2, %0"
	: "=r" (faulting_address)
	:
	:
);

//setMap(faulting_address, faulting_address, 1);
//kprintf("\nErrcode : %p", err_code);
if(!(err_code & 0x1 ) && (err_code& 0x4))
{
	//kprintf("\nPage not present in user mode %p ", faulting_address);
	int found = 0;
	mm_struct *mm = runningTask->mm;
	vm_area_struct *vmas = mm->head;
	while(vmas->next != mm->head){
		if(faulting_address >= vmas->vm_start && faulting_address <= vmas->vm_end){
			found = 1;
			//setMap(faulting_address, faulting_address, 1);
			break;
		}
		vmas = vmas->next;
	}
	if(faulting_address >= vmas->vm_start && faulting_address <= vmas->vm_end){
		found = 1;
		//setMap(faulting_address, faulting_address, 1);
	}
	if(!found){
		int kpid = kill_process(runningTask->pid);
		kprintf("Segmentation Fault! [%d]\n", kpid);
		temp_yield(1);
	}
	setMap(faulting_address, faulting_address, 1);
	reloadCR3();	
}
else if(!(err_code & 0x1) && !(err_code & 0x4) )
{
		kprintf("Page not present in supervisor mode %p", faulting_address);
		setMap(faulting_address , faulting_address ,1);
		reloadCR3();	
}
else if(err_code & 0x2)
{
	//kprintf("\nRead only page");
	if(checkCOW(faulting_address)==1)
	{
		//kprintf("%p\n", faulting_address);
		//kprintf("COW bit is set" );
	    	uint64_t copy_on_write_page=(uint64_t)get_free_page();		
	   	memcpy((void *)copy_on_write_page , (void *)faulting_address, 4096 ) ;
		setWritable( faulting_address, copy_on_write_page);
		setCOW(faulting_address, 0);
	   	memcpy((void *)faulting_address , (void *)copy_on_write_page, 4096 ) ;
	}
	reloadCR3();	
}


}
