#include <sys/mm_struct.h>
#include <sys/vm_area_struct.h>
#include <sys/page.h>
#include <sys/task.h>
#include <sys/memory.h>
/*
struct mm_struct* get_mm_struct(){
    struct mm_struct* new_mm_struct = (struct mm_struct*)get_free_page();
    new_mm_struct->head = NULL;
    return new_mm_struct;
}
*/


mm_struct* get_mm_struct(){
    mm_struct* new_mm_struct = (mm_struct*)get_free_page();
    new_mm_struct->head = NULL;
    return new_mm_struct;
}

void init_insert_vma(mm_struct *tasks_mm, vm_area_struct *new_vma){
	
	if(tasks_mm->head == NULL){
		tasks_mm->head = new_vma;
		tasks_mm->head->next = new_vma;
		tasks_mm->head->prev = new_vma;
	}
	new_vma->next = tasks_mm->head;
	new_vma->prev = tasks_mm->head->prev;
	tasks_mm->head->prev->next = new_vma;
	tasks_mm->head->prev = new_vma;
	tasks_mm->head = new_vma;
}

void deepCopy_mm_structs(uint64_t child_process){
	
	task *cp = (task *)child_process;
	task *p = (task *)(cp->parent);

	//mm_struct *child = (cp->mm);
	mm_struct *parent = p->mm;

	vm_area_struct *temp_parent = parent->head;
	
	int count =0;
	while(temp_parent->next != parent->head){
		count++;
		temp_parent = temp_parent->next;
	}

	temp_parent = parent->head;
	int index = 0;
	while(index <= count/*temp_parent->next != parent->head*/){

		uint64_t parent_vma_type = temp_parent->vma_type;
		//uint64_t parent_vma_start = temp_parent->vm_end;
		//uint64_t parent_vma_end = temp_parent->vm_start;
		uint64_t parent_vma_max_size = temp_parent->max_size;
	
		if(parent_vma_type == VMA_STACK_TYPE) {
			vm_area_struct *new_vma = create_new_vma(cp->regs.user_rsp- 4096, cp->regs.user_rsp, parent_vma_max_size, VMA_STACK_TYPE);
			memcpy((void *)(new_vma->vm_start), (void *)temp_parent->vm_start, 4096);
			init_insert_vma(cp->mm, new_vma);
		} else {
			vm_area_struct *new_vma = create_new_vma(temp_parent->vm_start, temp_parent->vm_end, parent_vma_max_size, VMA_TEXT_TYPE);
			memcpy((void *)(new_vma->vm_start), (void *)temp_parent->vm_start, temp_parent->vm_end - temp_parent->vm_start);
			init_insert_vma(cp->mm, new_vma);
		}

		temp_parent = temp_parent->next;
		index++;
	}
}

void update_heap_vma(vm_area_struct *heap_vma, vm_area_struct *anon){
	int reduced_size = anon->max_size;
	/*
	while(temp->next != heap_vma){
		
		if(temp->vma_type == VMA_ANON_TYPE){
			reduced_size += temp->max_size;
		}

		temp = temp->next;
	}
	*/
	//kprintf("[Kernel] Heap range was [%p-%p]\n", heap_vma->vm_start, heap_vma->vm_end);
	heap_vma->vm_start += reduced_size;
	kprintf("[Kernel] Heap size reducing by %d\n", reduced_size);
	//kprintf("[Kernel] Heap range is [%p-%p]\n", heap_vma->vm_start, heap_vma->vm_end);
}

uint64_t allocate_page(int bytes){
	
	int index = 0;
	int count = 0;
	vm_area_struct *temp = runningTask->mm->head;
	vm_area_struct *heap_vma;
	while(temp->next != runningTask->mm->head){
		count++;
		temp = temp->next;
	}
	
	int heap_vma_found = 0;
	while(index <= count){
		if(temp->vma_type == VMA_HEAP_TYPE){
			heap_vma = temp;
			heap_vma_found = 1;
			//kprintf("[Kernel]: Heap present \n");
		}
		index++;
		temp = temp->next;
	}

	if(!heap_vma_found){
		//kprintf("[Kernel]: New heap allocated \n");	
		vm_area_struct *new_vma = create_new_vma(HEAP_REGION_START, HEAP_REGION_START + 0x400000, 0x400000, VMA_HEAP_TYPE);
		heap_vma = new_vma;
		init_insert_vma(runningTask->mm, new_vma);
	}


	vm_area_struct *new_vma = create_new_vma(heap_vma->vm_start, heap_vma->vm_start + bytes, bytes, VMA_ANON_TYPE);
	init_insert_vma(runningTask->mm, new_vma);
	update_heap_vma(heap_vma, new_vma);
	kprintf("[Kernel]: generated page for malloc %p\n", new_vma->vm_start);
	return (new_vma->vm_start);
}

int unmap(uint64_t addr){

	// find the anon vma which falls under this address
	
	int index = 0;
	int count = 0;
	vm_area_struct *temp = runningTask->mm->head;
	vm_area_struct *heap_vma = NULL;
	//vm_area_struct *interested_vma= NULL;
	while(temp->next != runningTask->mm->head){
		count++;
		temp = temp->next;
	}
	// extract the heap vma
	while(index <= count){
		if(temp->vma_type == VMA_HEAP_TYPE){
			heap_vma = temp;
		}
		index++;
		temp = temp->next;
	}

	index = 0;
	int found = 0;
	while(index <= count){
		if(temp->vma_type == VMA_ANON_TYPE){
			if( addr >= temp->vm_start && addr <= temp->vm_end){
				//interested_vma = temp;
				temp->vm_start = -1;
				temp->vm_end = -1;
				found = 1;
				break;
			}
		}
		index++;
		temp = temp->next;
	}

	if(found){
		heap_vma->vm_start -= temp->max_size;
		kprintf("[Kernel] Heap has been increased to [%p-%p]\n", heap_vma->vm_start, heap_vma->vm_end);
	}
	return found;

}

/*
void print_vma_content(struct vm_area_struct* vma){
    kprintf("VMA Content ");
    kprintf("start address: %d ", vma->vm_start);
    kprintf("end address: %d \n", vma->vm_end);
}

void print_all_vmas(struct mm_struct* input){
    struct vm_area_struct* cursor = input->head;
    while(cursor != NULL){
        print_vma_content(cursor);
        cursor = cursor->next;
    }
}
*/
void init_stack_vma(struct mm_struct* in_mm_struct){
    //int page_size = 4096;
/*    struct vm_area_struct* new_stack_vma = 
    init_vm_area_struct(STACK_REGION_END+page_size, 
        STACK_REGION_END, 
        MAP_GROWSDOWN, 0, NULL, NULL);*/
//    in_mm_struct->head = new_stack_vma;
}
/*
void init_heap_vma(struct mm_struct* in_mm_struct){
    int page_size = 4096;
    add_vma(in_mm_struct, HEAP_REGION_START, 
        HEAP_REGION_START+page_size, 
        MAP_GROWSUP, 0);
}
*/

/*
void add_vma(struct mm_struct* in_mm_struct, uint64_t start_address, uint64_t end_address,
            uint64_t page_prot, int flags){
    
    struct vm_area_struct* head = in_mm_struct->head;

    
    
    if(head == NULL){
        kprintf("Head is null\n");
        //mm_struct is empty
        //struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, NULL, NULL);
        //in_mm_struct->head = new_vma;
        //print_vma_content(new_vma);
        //return;
    }

    //find the correct area to map vma
    struct vm_area_struct* cursor = head;
    kprintf("Cursor Content:\n");
    print_vma_content(cursor);
    //kprintf("Insert address: %d\n", start_address);
    //kprintf("Cursor VM Start: %d", cursor->vm_start);
    int value = start_address > cursor->vm_start;
    kprintf(" Value: %d ", value); 
    if(start_address > cursor->vm_start){
        kprintf("Insert after tail");
        //Insert after tail
        while(cursor != NULL){
            if(cursor->vm_end < start_address){
                if(cursor->next == NULL){
                    kprintf("Insert in 1\n");
                   //struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, cursor, NULL);
                    //cursor->next = new_vma;
                    //kprintf("Content of the added vma");
                    //print_vma_content(new_vma);
                    break;
                }
                else{
                    kprintf("Insert in 2\n");
                   //struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, cursor, cursor->next);
                    //cursor->next->prev = new_vma;
                    //cursor->next = new_vma;
                    //kprintf("Content of the added vma");
                    //print_vma_content(new_vma);
                    break;
                }
            }
            cursor = cursor->next;
        }
    }
    else{
        //Insert before head
        kprintf("Insert before head ");
       // struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, NULL, cursor);
        //cursor->prev = new_vma;
        //in_mm_struct->head = new_vma;
    }
}
*/
