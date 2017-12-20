#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>

int strlen(char *string);
int strcmp(char *a, char *b);
void strcpy(char *dest, char *src);
void strcat(char *src, char *dest);
void *memset(void *array, int c, size_t n);
int get_argc(char*);
char**  parse_args_from_str(char* , char**);
#endif
