#ifndef _MM_STRUCT_H
#define _MM_STRUCT_H

#include <sys/vm_area_struct.h>

struct mm_struct{
    struct vm_area_struct* head;
};

struct mm_struct* get_mm_struct();
void print_all_vmas(struct mm_struct* input);
void add_vma(struct mm_struct*, uint64_t, uint64_t, uint64_t, int);

#endif
