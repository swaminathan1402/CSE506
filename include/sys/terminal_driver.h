#ifndef __TERMINAL_DRIVER_H
#define __TERMINAL_DRIVER_H
#include<sys/defs.h>

char terminal_buffer[1024];
int terminal_buffer_len;

int terminal_read(int, void*, uint64_t);
int terminal_write(int, void*, uint64_t);
void terminal_enqueue(char);

#endif
