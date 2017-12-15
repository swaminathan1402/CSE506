#ifndef _VM_AREA_STRUCT_H
#define _VM_AREA_STRUCT_H

#include <sys/defs.h>

#define MAP_ANONYMOUS 0x100
#define MAP_GROWSDOWN 0x200
#define MAP_GROWSUP   0x400

#define STACK_REGION_START 0x4000000
#define STACK_REGION_END   0x0000000

#define HEAP_REGION_START  0x8000000

struct vm_area_struct{
    uint64_t vm_start;
    uint64_t vm_end;
    //Address range for the VMA
    
    uint64_t vm_page_prot; //Protection bits applied to PTE. Architecture specific
    
    int vm_flags; //Architecture independent. Refer https://stackoverflow.com/questions/8035504/vm-flags-vs-vm-page-prot

    uint64_t max_size; //Max size to which vma can grow. ????

    //struct file* vm_file;//Pointer to file struct associated with the area Not sure what to push

    //struct mm_struct *vm_mm; //MM Struct corre


    //Doubly linked list of VMAs
    struct vm_area_struct* next;

    struct vm_area_struct* prev;

};

struct vm_area_struct* get_vm_area_struct();
struct vm_area_struct* init_vm_area_struct(uint64_t, uint64_t,uint64_t, uint64_t, int, struct vm_area_struct*, struct vm_area_struct*);
void print_vma_content(struct vm_area_struct* vma);
#endif
