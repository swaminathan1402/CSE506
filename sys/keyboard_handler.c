#include <sys/kprintf.h>
#include <sys/pic.h>
void keyboard_handler(){
	//unsigned char scan_code = inb(0x60);
	inb(0x60);
	kprintf("hello");
	end_of_interrupt(0x0);	
}
