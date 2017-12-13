#include<sys/page_table.h>
#include<sys/memory.h> 
#include<sys/page.h>
#include<sys/kprintf.h>
#include<sys/task.h>

void changeCR3(PML4E *new_pml4e, PDPE *new_pdpe, PDE *new_pde, PTE *new_pte, int idmap) {
	pml4e = new_pml4e;
	pdpe = new_pdpe;
	pde = new_pde;
	pte = new_pte;

	if(idmap){
		for(uint64_t i=0; i<1024;i++)
		    setMap(i*4096, i*4096, 1); // kernel
	}


	__asm__ __volatile__(
        	"movq %0, %%cr3;"
        	:
        	:"r"(new_pml4e)
        );
	//kprintf("CHANGED CR3 %p %p %p %p\n", pml4e, pdpe, pde, pte);

}

void reloadCR3()
{
uint64_t current_CR3;
__asm__ __volatile__(
"movq %%cr3 ,%0; "
"movq %1 , %%cr3; "
: "=r"(current_CR3)
: "r"(current_CR3)
:
);

}




uint64_t checkCOW( uint64_t faulting_address)
{
	uint64_t temp;
	int pml4e_index = (faulting_address & 0x0000ff8000000000) >> 39;
 	int pdpe_index = (faulting_address & 0x0000007fc0000000) >> 30;
 	int pde_index = (faulting_address & 0x000000003fe00000) >> 21;  
 	int pte_index = (faulting_address & 0x00000000001ff000) >> 12;
	
	PML4E *parent_pml4e = runningTask->pml4e;
	PDPE *some_pdpe = (PDPE*)(uint64_t)((parent_pml4e+pml4e_index)->page_directory_pointer_base_address << 12);
	PDE *some_pde = (PDE *)(uint64_t)((some_pdpe + pdpe_index)->page_directory_base_address << 12);
	PTE *some_pte = (PTE *)(uint64_t)((some_pde + pde_index)->page_table_base_address << 12);      
        temp = (uint64_t)(some_pte + pte_index)->cow;
	return temp;
}	


void setCOW(uint64_t faulting_address ,uint64_t value)
{

 	int pml4e_index = (faulting_address & 0x0000ff8000000000) >> 39;
  	int pdpe_index = (faulting_address & 0x0000007fc0000000) >> 30;
  	int pde_index = (faulting_address & 0x000000003fe00000) >> 21;  
  	int pte_index = (faulting_address & 0x00000000001ff000) >> 12;
 	
 	PML4E *parent_pml4e = runningTask->pml4e;
 	PDPE *some_pdpe = (PDPE*)(uint64_t)((parent_pml4e+pml4e_index)->page_directory_pointer_base_address << 12);
 	PDE *some_pde = (PDE *)(uint64_t)((some_pdpe + pdpe_index)->page_directory_base_address << 12);
 	PTE *some_pte = (PTE *)(uint64_t)((some_pde + pde_index)->page_table_base_address << 12);      
 	(some_pte +pte_index)->cow= value &0x1;	
	return;
}


void setWritable(uint64_t faulting_address ,uint64_t copy_on_write_page)
{


 	int pml4e_index = (faulting_address & 0x0000ff8000000000) >> 39;
  	int pdpe_index = (faulting_address & 0x0000007fc0000000) >> 30;
  	int pde_index = (faulting_address & 0x000000003fe00000) >> 21;  
  	int pte_index = (faulting_address & 0x00000000001ff000) >> 12;
 	
 	PML4E *parent_pml4e = runningTask->pml4e;
 	PDPE *some_pdpe = (PDPE*)(uint64_t)((parent_pml4e+pml4e_index)->page_directory_pointer_base_address << 12);
 	PDE *some_pde = (PDE *)(uint64_t)((some_pdpe + pdpe_index)->page_directory_base_address << 12);
 	PTE *some_pte = (PTE *)(uint64_t)((some_pde + pde_index)->page_table_base_address << 12);      

       (some_pte+pte_index)->rw=1;
	(some_pte+pte_index)->us=1;
	(some_pte+pte_index)->p=1;
       (some_pte+pte_index)->physical_address = copy_on_write_page>>12;
}

void idpaging(PTE* first_pte, uint64_t from, int size){
	// from = from & 0xfffffffffffff000;
	from = from & 0xfffffffffffff000;
	int i = 0;
	for(;size>0; from+=4096, size-=4096, i++){
		(first_pte + i)->physical_address = from >> 12;
		(first_pte + i)->rw = 1;
		(first_pte + i)->p  = 1;
		(first_pte + i)->us = 0;
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
	setMap(0xb8000, 0xb8000, 0);
	int i;
	for(i=0; i<1024;i++)
	    setMap(i*4096, i*4096, 0); // kernel
	__asm__ __volatile__(
        	"movq %0, %%cr3;"
        	:
        	:"r"(first_pml4e)
        );
}

void changeUserPrivilegePage(uint64_t virtual_addr){
	int pml4e_index = (virtual_addr & 0x0000ff8000000000) >> 39;
 	int pdpe_index = (virtual_addr & 0x0000007fc0000000) >> 30;
 	int pde_index = (virtual_addr & 0x000000003fe00000) >> 21;
 	int pte_index = (virtual_addr & 0x00000000001ff000) >> 12;

		
	PDPE *some_pdpe = (PDPE*)(uint64_t)((pml4e+pml4e_index)->page_directory_pointer_base_address << 12);
	PDE *some_pde = (PDE *)(uint64_t)((some_pdpe + pdpe_index)->page_directory_base_address << 12);
	PTE *some_pte = (PTE *)(uint64_t)((some_pde + pde_index)->page_table_base_address << 12);
	(pml4e+pml4e_index)->us=1;
	(some_pdpe+pdpe_index)->us=1;
	(some_pde+pde_index)->us=1;
	(some_pte+pte_index)->us = 1;
}

void deepCopyPageTable(uint64_t child){
	task *child_task = (task *)child;
	// walk thru the page table of running task and deep copy stuff
	PML4E *parent_pml4e = runningTask->pml4e;
	PDPE *parent_pdpe = runningTask->pdpe;
	PDE *parent_pde = runningTask->pde;
	PTE *parent_pte = runningTask->pte;

	PML4E *child_pml4e = child_task->pml4e;
	PDPE *child_pdpe = child_task->pdpe;
	PDE *child_pde = child_task->pde;
	PTE *child_pte = child_task->pte;
	for(int pml4e_index=0; pml4e_index<511; pml4e_index++){
		if((parent_pml4e + pml4e_index)->p == 0){
			continue;
		} else{
			if((child_pml4e+ pml4e_index)->p == 0){
				uint64_t *some_page = (uint64_t *)get_free_page();
				memset(some_page, 0, 4096);
				(child_pml4e + pml4e_index)->page_directory_pointer_base_address = (uint64_t)some_page >> 12;
				(child_pml4e + pml4e_index)->p = 1;
				(child_pml4e + pml4e_index)->us = 1;
				(child_pml4e + pml4e_index)->rw = 1;  // set it to 0 TODO
			}

			parent_pdpe = (PDPE *)(uint64_t)((parent_pml4e + pml4e_index)->page_directory_pointer_base_address << 12);
			child_pdpe = (PDPE *)(uint64_t)((child_pml4e + pml4e_index)->page_directory_pointer_base_address << 12); 

			for(int pdpe_index=0; pdpe_index < 512; pdpe_index++){
				if((parent_pdpe + pdpe_index)->p == 0){
					continue;
				} else {

					if((child_pdpe + pdpe_index)->p == 0){
						uint64_t *temp_page = (uint64_t *)get_free_page();
						memset(temp_page, 4096, 0);
						(child_pdpe + pdpe_index)->page_directory_base_address =(uint64_t)temp_page >> 12;
						(child_pdpe + pdpe_index)->p = 1;
						(child_pdpe + pdpe_index)->rw = 1;
						(child_pdpe + pdpe_index)->us = 1;

					}
					parent_pde = (PDE *)(uint64_t)((parent_pdpe + pdpe_index)->page_directory_base_address << 12);
					child_pde = (PDE *)(uint64_t)((child_pdpe + pml4e_index)->page_directory_base_address << 12);
					for(int pde_index=0; pde_index < 512; pde_index++){
						if((parent_pde + pde_index)->p == 0){
							continue;
						} else {
							if((child_pde + pde_index)->p == 0){
								uint64_t *temp_page = (uint64_t *)get_free_page();
								memset(temp_page, 4096, 0);
								(child_pde + pde_index)->page_table_base_address =(uint64_t)temp_page >> 12;
								(child_pde + pde_index)->p = 1;
								(child_pde + pde_index)->rw = 1;
				;				(child_pde + pde_index)->us = 1;

							}
							parent_pte = (PTE *)(uint64_t)((parent_pde + pde_index)->page_table_base_address << 12);
							child_pte = (PTE *)(uint64_t)((child_pde + pde_index)->page_table_base_address << 12);
							
							for(int pte_index = 0; pte_index < 512; pte_index++){
								if((parent_pte + pte_index)->p == 0){
									continue;
								} else {
									if((child_pte + pte_index)->p == 0){
										(child_pte + pte_index)->p = 1;
										(child_pte + pte_index)->rw = 0;
										(child_pte + pte_index)->us = 1;
										(child_pte +pte_index)->cow=1;
										(parent_pte+ pte_index)->cow=1; 	
									}
									(child_pte + pte_index)->physical_address =(parent_pte + pte_index)->physical_address;
									
								}
							}
							

						}
					}
				}
			}
		}
	}
}

void setMap(uint64_t virtual_addr, uint64_t physical_addr, int user_accessible){
	int pml4e_index = (virtual_addr & 0x0000ff8000000000) >> 39;
 	int pdpe_index = (virtual_addr & 0x0000007fc0000000) >> 30;
 	int pde_index = (virtual_addr & 0x000000003fe00000) >> 21;
 	int pte_index = (virtual_addr & 0x00000000001ff000) >> 12;
	//kprintf("This is what happens %p, %d %d %d %d\n", virtual_addr, pml4e_index, pdpe_index, pde_index, pte_index);
	if((pml4e+pml4e_index)->p == 0 ){
		
		// get free page
		uint64_t *some_page = (uint64_t *)get_free_page();
		memset(some_page, 0, 4096);
		(pml4e + pml4e_index)->page_directory_pointer_base_address = (uint64_t)some_page >> 12;
                (pml4e + pml4e_index)->p = 1;
                (pml4e + pml4e_index)->us = user_accessible;
                (pml4e + pml4e_index)->rw = 1;

		pdpe = (PDPE *)some_page;
		if((pdpe + pdpe_index)->p == 0){
			uint64_t *temp_page = (uint64_t *)get_free_page();
			memset(temp_page, 4096, 0);
			(pdpe + pdpe_index)->page_directory_base_address =(uint64_t)temp_page >> 12;
                        (pdpe + pdpe_index)->p = 1;
                        (pdpe + pdpe_index)->rw = 1;
                        (pdpe + pdpe_index)->us = user_accessible;

			pde = (PDE *)temp_page;
			if((pde + pde_index)->p == 0){
				uint64_t *temp_page_1 = (uint64_t *)get_free_page();
                                (pde + pde_index)->page_table_base_address =(uint64_t) temp_page_1 >> 12 ;
                                (pde + pde_index)->p = 1;
                                (pde + pde_index)->rw = 1;
                                (pde + pde_index)->us = user_accessible;

				pte = (PTE *)temp_page_1;
				if((pte + pte_index)->p == 0){
					(pte + pte_index)->physical_address = physical_addr >> 12;
					(pte + pte_index)->rw = 1;
					(pte + pte_index)->p  = 1;
					(pte + pte_index)->us = user_accessible;
					
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
                (pdpe + pdpe_index)->us = user_accessible;
                                                                                       
                pde = (PDE *)temp_page;
		if((pde + pde_index)->p == 0){
	        	uint64_t *temp_page_1 = (uint64_t *)get_free_page();
                        (pde + pde_index)->page_table_base_address =(uint64_t) temp_page_1 >> 12;
                        (pde + pde_index)->p = 1;
                        (pde + pde_index)->rw = 1;
                        (pde + pde_index)->us = user_accessible;
                                                                                            
                	pte = (PTE *)temp_page_1;
                	if((pte + pte_index)->p == 0){
                		(pte + pte_index)->physical_address = physical_addr >> 12;
                		(pte + pte_index)->rw = 1;
                		(pte + pte_index)->p  = 1;
                		(pte + pte_index)->us = user_accessible;
                		
                	}
                }

	}

	if((pde + pde_index)->p == 0){
		
		// get free page
               uint64_t *temp_page_1 = (uint64_t *)get_free_page();
               (pde + pde_index)->page_table_base_address =(uint64_t) temp_page_1 >> 12;
               (pde + pde_index)->p = 1;
               (pde + pde_index)->rw = 1;
               (pde + pde_index)->us = user_accessible;
               pte = (PTE *)temp_page_1;
               if((pte + pte_index)->p == 0){
	     		(pte + pte_index)->physical_address = physical_addr >> 12;
               		(pte + pte_index)->rw = 1;
               		(pte + pte_index)->p  = 1;
               		(pte + pte_index)->us = user_accessible;
			//kprintf("setting pte %p\n", physical_addr >> 12);
               }

	}

	if ((pte + pte_index)->p == 0){
		(pte + pte_index)->physical_address = physical_addr >> 12;
                (pte + pte_index)->rw = 1;
                (pte + pte_index)->p  = 1;
                (pte + pte_index)->us = user_accessible;
	}
}
