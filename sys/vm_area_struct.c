#include <sys/vm_area_struct.h>
#include <sys/page.h>


struct vm_area_struct* get_vm_area_struct(){
   return (struct vm_area_struct*)get_free_page();
}

struct vm_area_struct* init_vm_area_struct(uint64_t start_address, 
    uint64_t end_address, uint64_t page_prot, int flags,
    struct vm_area_struct* prev, struct vm_area_struct* next){

   struct vm_area_struct* new_vma =  (struct vm_area_struct*)get_free_page();
   new_vma->vm_start      = start_address;
   new_vma->vm_end        = end_address;
   new_vma->vm_page_prot   = page_prot;
   new_vma->vm_flags       = flags;
   new_vma->prev           = prev;
   new_vma->next           = next;

   return new_vma;
   
}
