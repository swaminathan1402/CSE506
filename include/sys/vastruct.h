#include<defs.h>
#include<process.h>

struct __attribute__(packed) logical_address
{
uint16_t page_offset:12;
uint16_t pte_index:9;
uint16_t pte_directory

};

struct vm_area_struct
{
uint64_t vm_start;
uint64_t vm_end;
//Address range for the VMA

struct file* vm_file;//Pointer to file struct associated with the area
struct mm_struct *vm_mm; //MM Struct corre
vm_flags

};

struct mm_struct 
{




}

