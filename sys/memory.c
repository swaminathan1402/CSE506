#include <sys/defs.h>
#include<sys/memory.h>
void *memset(void *array, int c, size_t n){
    unsigned char* temp = array;
    while(n--){
        *temp++ = (unsigned char)c;
    }
    return array;
}
