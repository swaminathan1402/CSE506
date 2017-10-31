#include<sys/page_table.h>
#include<sys/kprintf.h>

extern void loadPageDirectory(uint64_t *);
extern void enablePaging();

void idpaging(PTE* first_pte, uint64_t from, int size){
	// from = from & 0xfffffffffffff000;
	from = from & 0xffffffffff000;
	for(;size>0; from+=4096, size-=4096, first_pte++){
		first_pte->physical_address = from;
		first_pte->rw = 1;
		first_pte->p = 1;
		first_pte->us = 1;
		kprintf("%p <- %p\n", first_pte->physical_address, first_pte);
	}
	kprintf("From %p\n", from);
}

void init_pd(PTE* first_pte, uint64_t* pointer_to_pml4e, uint64_t from, int size){
	kprintf("Initializing page table \n");
	kprintf("Performing Kernel Identity mapping to first pte's 2 MB \n");
	idpaging(first_pte, from, size);
	
	__asm__ __volatile__(
		"movq %0, %%rax;"
		"movq %%rax, %%cr3;"
		:
		:"r"(pointer_to_pml4e)
		:"rax"
	);
	//loadPageDirectory(pointer_to_pml4e);
	enablePaging();
	//kprintf("%p %p %p\n", pointer_to_page_directory, page_table,
	//			page_directory[0]);
	kprintf("Page table has been initialized\n");
}


