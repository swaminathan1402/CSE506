#include <sys/mm_struct.h>
#include <sys/vm_area_struct.h>
#include <sys/page.h>


struct mm_struct* get_mm_struct(){
    struct mm_struct* new_mm_struct = (struct mm_struct*)get_free_page();
    new_mm_struct->head = NULL;
    return new_mm_struct;
}

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

void init_stack_vma(struct mm_struct* in_mm_struct){
    int page_size = 4096;
    struct vm_area_struct* new_stack_vma = 
    init_vm_area_struct(STACK_REGION_END+page_size, 
        STACK_REGION_END, 
        MAP_GROWSDOWN, 0, NULL, NULL);
    in_mm_struct->head = new_stack_vma;
}

void init_heap_vma(struct mm_struct* in_mm_struct){
    int page_size = 4096;
    add_vma(in_mm_struct, HEAP_REGION_START, 
        HEAP_REGION_START+page_size, 
        MAP_GROWSUP, 0);
}


void add_vma(struct mm_struct* in_mm_struct, uint64_t start_address, uint64_t end_address,
            uint64_t page_prot, int flags){
    
    struct vm_area_struct* head = in_mm_struct->head;
    /*
    if(head == NULL){
        kprintf("Head is NULL\n");
    }
    else{
        kprintf("Head is not NULL\n");
    }*/

    
    
    if(head == NULL){
        kprintf("Head is null\n");
        //mm_struct is empty
        struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, NULL, NULL);
        in_mm_struct->head = new_vma;
        print_vma_content(new_vma);
        return;
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
                    struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, cursor, NULL);
                    cursor->next = new_vma;
                    kprintf("Content of the added vma");
                    print_vma_content(new_vma);
                    break;
                }
                else{
                    kprintf("Insert in 2\n");
                    struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, cursor, cursor->next);
                    cursor->next->prev = new_vma;
                    cursor->next = new_vma;
                    kprintf("Content of the added vma");
                    print_vma_content(new_vma);
                    break;
                }
            }
            cursor = cursor->next;
        }
    }
    else{
        //Insert before head
        kprintf("Insert before head ");
        struct vm_area_struct* new_vma = init_vm_area_struct(start_address, end_address, page_prot, flags, NULL, cursor);
        cursor->prev = new_vma;
        in_mm_struct->head = new_vma;
    }
}
