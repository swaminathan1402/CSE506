#include<sys/page_table.h>
#include<sys/memory.h> 
#include<sys/page.h>
#include<sys/kprintf.h>


void idpaging(PTE* first_pte, uint64_t from, int size){
	// from = from & 0xfffffffffffff000;
	from = from & 0xfffffffffffff000;
	int i = 0;
	for(;size>0; from+=4096, size-=4096, i++){
		(first_pte + i)->physical_address = from >> 12;
		(first_pte + i)->rw = 1;
		(first_pte + i)->p  = 1;
		(first_pte + i)->us = 1;
		//kprintf("%p <- %p\n", (first_pte+i)->physical_address, first_pte + i);
		
	}
	//kprintf("From %p\n", from);
}


void init_pd(PTE* first_pte, PML4E* first_pml4e, uint64_t from, int size){
	kprintf("Initializing page table \n");
	kprintf("Performing Kernel Identity mapping to first pte's 2 MB \n");

	idpaging(first_pte, from, size);
	kprintf("Mapping from %p to %p\n", from+size, from+size+100*4096);
	idpaging(first_pte + 85, from+size, 100*4096);

	kprintf("pointer to pml4e pdpe addr:%p\n pointer to first pte: %p\n", (first_pml4e+511)->page_directory_pointer_base_address, first_pte);
	kprintf("pointer to pml4e:%p\n", first_pml4e);
	//setMap(0xffffffff800b8000, 0xb8000);
	setMap(0xb8000, 0xb8000);
	for(int i=0; i<1024;i++)
	    setMap(i*4096, i*4096);
	__asm__ __volatile__(
        	"movq %0, %%cr3;"
        	:
        	:"r"(first_pml4e)
        );
}

void setMap(uint64_t virtual_addr, uint64_t physical_addr){
	int pml4e_index = (virtual_addr & 0x0000ff8000000000) >> 39;
	int pdpe_index = (virtual_addr & 0x0000007fc0000000) >> 30;
	int pde_index = (virtual_addr & 0x000000003fe00000) >> 21;
	int pte_index = (virtual_addr & 0x00000000001ff000) >> 12;
	kprintf("This is what happens %p, %d %d %d %d\n", virtual_addr, pml4e_index, pdpe_index, pde_index, pte_index);
	if((pml4e+pml4e_index)->p == 0 ){
		
		// get free page
		uint64_t *some_page = (uint64_t *)get_free_page();
		memset(some_page, 0, 4096);
		(pml4e + pml4e_index)->page_directory_pointer_base_address = (uint64_t)some_page >> 12;
                (pml4e + pml4e_index)->p = 1;
                (pml4e + pml4e_index)->us = 1;
                (pml4e + pml4e_index)->rw = 1;

		pdpe = (PDPE *)some_page;
		if((pdpe + pdpe_index)->p == 0){
			uint64_t *temp_page = (uint64_t *)get_free_page();
			memset(temp_page, 4096, 0);
			(pdpe + pdpe_index)->page_directory_base_address =(uint64_t)temp_page >> 12;
                        (pdpe + pdpe_index)->p = 1;
                        (pdpe + pdpe_index)->rw = 1;
                        (pdpe + pdpe_index)->us = 1;

			pde = (PDE *)temp_page;
			if((pde + pde_index)->p == 0){
				uint64_t *temp_page_1 = (uint64_t *)get_free_page();
                                (pde + pde_index)->page_table_base_address =(uint64_t) temp_page_1 >> 12 ;
                                (pde + pde_index)->p = 1;
                                (pde + pde_index)->rw = 1;
                                (pde + pde_index)->us = 1;

				pte = (PTE *)temp_page_1;
				if((pte + pte_index)->p == 0){
					(pte + pte_index)->physical_address = physical_addr >> 12;
					(pte + pte_index)->rw = 1;
					(pte + pte_index)->p  = 1;
					(pte + pte_index)->us = 1;
					
				}
			}
		}

	}

	if ((pdpe + pdpe_index)->p == 0){
		
		// get free page
		uint64_t *temp_page = (uint64_t *)get_free_page();
                memset(temp_page, 4096, 0);
                (pdpe + pdpe_index)->page_directory_base_address =(uint64_t)temp_page >> 12;
                (pdpe + pdpe_index)->p = 1;
                (pdpe + pdpe_index)->rw = 1;
                (pdpe + pdpe_index)->us = 1;
                                                                                       
                pde = (PDE *)temp_page;
		if((pde + pde_index)->p == 0){
	        	uint64_t *temp_page_1 = (uint64_t *)get_free_page();
                        (pde + pde_index)->page_table_base_address =(uint64_t) temp_page_1 >> 12;
                        (pde + pde_index)->p = 1;
                        (pde + pde_index)->rw = 1;
                        (pde + pde_index)->us = 1;
                                                                                            
                	pte = (PTE *)temp_page_1;
                	if((pte + pte_index)->p == 0){
                		(pte + pte_index)->physical_address = physical_addr >> 12;
                		(pte + pte_index)->rw = 1;
                		(pte + pte_index)->p  = 1;
                		(pte + pte_index)->us = 1;
                		
                	}
                }

	}

	if((pde + pde_index)->p == 0){
		
		// get free page
               uint64_t *temp_page_1 = (uint64_t *)get_free_page();
               (pde + pde_index)->page_table_base_address =(uint64_t) temp_page_1 >> 12;
               (pde + pde_index)->p = 1;
               (pde + pde_index)->rw = 1;
               (pde + pde_index)->us = 1;
               pte = (PTE *)temp_page_1;
               if((pte + pte_index)->p == 0){
	     		(pte + pte_index)->physical_address = physical_addr >> 12;
               		(pte + pte_index)->rw = 1;
               		(pte + pte_index)->p  = 1;
               		(pte + pte_index)->us = 1;
			//kprintf("setting pte %p\n", physical_addr >> 12);
               }

	}

	if ((pte + pte_index)->p == 0){
		(pte + pte_index)->physical_address = physical_addr >> 12;
                (pte + pte_index)->rw = 1;
                (pte + pte_index)->p  = 1;
                (pte + pte_index)->us = 1;
	}
}
