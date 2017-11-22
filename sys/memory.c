#include <sys/defs.h>
#include<sys/memory.h>
void *memset(void *array, int c, size_t n){
    unsigned char* temp = array;
    while(n--){
        *temp++ = (unsigned char)c;
    }
    return array;
}

void *memcpy(void *dst, void *src, size_t n )
{
    unsigned char *temp = dst;
    unsigned char *temp2 = src;
    while(n--) {
        //*dst++ = *src++;
	*temp++ = *temp2++;
    }
    return dst;	
}
