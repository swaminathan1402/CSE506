#include <stdlib.h>
#include <syscall.h>

void* malloc (size_t size){
    size_t *mem_pointer;
    mem_pointer = syscall_mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE/* | MAP_ANONYMOUS*/, 0, 0); 

    *mem_pointer = size;
    return (void*)(&mem_pointer[1]);
}

void free (void* mem_pointer){
    /*
    int *pointer_length = (int*)mem_pointer;
    size_t size;

    size = *pointer_length;
    //syscall_munmap((void*)pointer_length, size);
    */
}

