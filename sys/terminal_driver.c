#include <sys/kprintf.h>
#include <sys/memory.h>
#include <sys/terminal_driver.h>


int terminal_read(int fd, void* buffer, uint64_t buffer_len){
    memcpy(buffer, terminal_buffer, terminal_buffer_len);
    buffer_len = terminal_buffer_len;
    terminal_buffer_len = 0;
    return buffer_len;
}


int terminal_write(int fd, void* buffer, uint64_t buffer_len){
    char* output = (char*)buffer;
    output[buffer_len] = '\0';
    kprintf(buffer);
}

void terminal_enqueue(char c){
    if(c == '\b'){
        terminal_buffer[terminal_buffer_len] = '\0';
        terminal_buffer_len--;
        terminal_buffer[terminal_buffer_len] = '\0';
    }
    else if(c == ' ' && terminal_buffer_len == 0){
        //do nothing. Does not add leading spaces into the buffer
    }
    else if(c == '\n'){
        terminal_buffer[terminal_buffer_len] = '\0';
    }

    kprintf("%c", c);
    terminal_buffer[terminal_buffer_len++] = c;
}
