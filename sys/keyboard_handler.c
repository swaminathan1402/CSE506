#include <sys/kprintf.h>
#include <sys/pic.h>
void keyboard_handler(){
	//unsigned char scan_code = inb(0x60);
 	unsigned char scan_code = inb(0x60);
	if(scan_code == 0x04){
		kprintf("3 is pressed ");
	}
	//kprintf("hello %x ",scan_code);
	end_of_interrupt(0x0);	
}
