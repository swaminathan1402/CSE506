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
    unsigned char* temp = array;
    while(n--){
        *temp++ = (unsigned char)c;
    }
    return array;

}

int get_argc(char* inp){
    int n_of_params = 0;
    for(int i=0; i<strlen(inp);i++){
        if(inp[i] == ' '){
            n_of_params++;
        }
    }
    n_of_params++;
    return n_of_params;
}


char**  parse_args_from_str(char* inp, char** params){
    int len = strlen(inp);
    int n_of_params = 0;
    for(int i=0; i<len;i++){
        if(inp[i] == ' '){
            n_of_params++;
        }
    }
    char param[5][64];
    n_of_params++;
    int index = 0;
    int c = 0;
    for(int i=0; i<len;i++){
        if(inp[i] == ' '){
            param[index][c] = '\0';
            index++;
            c=0;
        }else{
            param[index][c] = inp[i];
            c++;
        }
    }
    param[index][c] = '\0';
    return param;
}

