#include <string.h>

int strlen(char *string){
	if(string == NULL) return 0;
	int index = 0;
	while(string[index] != '\0'){
		index++;
	}

	return index;
}

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

void strcpy(char *dest, char *src){
	int index = 0;
	while(src[index] != '\0'){
		dest[index] = src[index];
		index++;
	}
	dest[index] = '\0';
}

void strcat(char *dest, char *src){

	int index = 0;
	while(dest[index] != '\0'){
		index++;
	}

	int i=0;
	while(src[i] != '\0'){
		dest[index] = src[i];
		i++;
		index++;
	}
	dest[index] = '\0';
}

void *memset(void *array, int c, size_t n){
    unisigned char* temp = array;
    while(n--){
        *temp++ = (unsigned char)c;
    }
    return array;

}
