#include <sys/kprintf.h>
#include <sys/pic.h>

int getCharfromCode(unsigned char scan_code1)
{
	if(scan_code1 == 0x02){
		return 49;
	}
	else if(scan_code1 == 0x03){
		return 50;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x05){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	else if(scan_code1 == 0x04){
		return 51;
	}
	return 0;
}


void keyboard_handler(){
	unsigned char scan = inb(0x60);
	//inb(0x60);
	char map;
	int code = getCharfromCode(scan);
	if(code != 0) {
	  map = (char)code;
	  kprintf("hello %c", map);
	}
	end_of_interrupt(0x0);	
}
/*
int Shiftflag=0;
int Controlflag=0;
struct scan_code_entry
{
int scan_code;
char mapping;
}sct[100]; 
*/
