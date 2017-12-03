#include <sys/string.h>

int strcmp(char *a, char *b){
        int lenA = strlen(a);
        int lenB = strlen(b);
        if(lenA != lenB) return 0;
        int index = 0;
        while(a[index] != '\0'){
                if(a[index] != b[index])
                        return 0;
                index++;
        }   
        return 1;
}
