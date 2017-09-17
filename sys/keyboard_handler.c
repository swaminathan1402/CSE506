#include <sys/kprintf.h>
#include <sys/pic.h>

/*
int Shiftflag=0;
int Controlflag=0;
struct scan_code_entry
{
int scan_code;
char mapping;
}sct[100]; 
*/
int shiftFlag = 0;
int ctrlFlag = 0;

int getCharfromCode(unsigned char scan_code1)
{
	if(scan_code == 0x2a || scan_code == 0x36){ // left shift and right shift pressed 
		shiftFlag = 1;
		return -1;
	}
	else if(scan_code == 0x1e){ // left ctrl
		ctrlFlag = 1;
		return -1;
	}

	else if(scane_code1 == 0x9e) { // left ctrl released
		ctrlFlag = 0;
		return -1;
	}

	else if(scan_code1 == 0xaa || scan_code1 == 0xb6) { // left shift or right shift released 
		shiftFlag = 0;
		return -1;
	}
	
	else if(scan_code1 == 0x02){
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
	if(code > 0) {
	  map = (char)code;
	  kprintf("hello %c", map);
	}
	end_of_interrupt(0x0);	
}
