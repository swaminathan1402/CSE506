#ifndef __TERMINAL_DRIVER_H
#define __TERMINAL_DRIVER_H

char terminal_buffer[1024];
uint64_t terminal_buffer_len = 0;

int terminal_read(int, void*, uint64_t);
int terminal_write(int, void*, uint64_t);
void terminal_enqueue(char);

#endif
