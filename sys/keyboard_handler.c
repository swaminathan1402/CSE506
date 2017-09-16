#include <sys/kprintf.h>
#include <sys/pic.h>
void keyboard_handler(){
	kprintf("hello\n");
	end_of_interrupt(0x0);	
}
