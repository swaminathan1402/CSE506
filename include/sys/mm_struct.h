#ifndef _MM_STRUCT_H
#define _MM_STRUCT_H

#include <sys/vm_area_struct.h>

typedef struct mm_struct{
    vm_area_struct* head;
}mm_struct;

//struct mm_struct* get_mm_struct();
//void print_all_vmas(mm_struct* input);
//void add_vma(struct mm_struct*, uint64_t, uint64_t, uint64_t, int);
void init_insert_vma(mm_struct*, vm_area_struct*);
mm_struct *get_mm_struct();
void deepCopy_mm_structs(uint64_t);
uint64_t allocate_page(int);
void update_heap_vma(vm_area_struct*, vm_area_struct*);
#endif
