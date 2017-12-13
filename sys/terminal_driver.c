#include <sys/kprintf.h>
#include <sys/memory.h>
#include <sys/terminal_driver.h>
#include <sys/defs.h>

terminal_buffer_len = 0;
volatile int done_flag=0;

int terminal_read(int fd, void* buffer, uint64_t buffer_len){
    __asm__ __volatile__ (
	"sti"
	:
	:
	:
    );
    while(!done_flag);
    memcpy(buffer, terminal_buffer, terminal_buffer_len);
    buffer_len = terminal_buffer_len;
    terminal_buffer_len = 0;
    done_flag=0;
    return buffer_len;
}


int terminal_write(int fd, void* buffer, uint64_t buffer_len){
    char* output = (char*)buffer;
    output[buffer_len] = '\0';
    kprintf("\n%s", output);
}

void terminal_enqueue(char c){
    if(c == '\b'){
        terminal_buffer[terminal_buffer_len] = '\0';
        terminal_buffer_len--;
        terminal_buffer[terminal_buffer_len] = '\0';
	return;
    }
    else if(c == ' ' && terminal_buffer_len == 0){
        //do nothing. Does not add leading spaces into the buffer
    }
    else if(c == '\n'){
	kprintf("\n");
        terminal_buffer[terminal_buffer_len] = '\0';
	done_flag= 1;
	return;
    }

    kprintf("%c", c);
    terminal_buffer[terminal_buffer_len++] = c; 
    done_flag=0;
    return;
}
