#include <sys/kprintf.h>
#include <sys/pic.h>

int getCharfromCode(unsigned char scan_code1)
{
        if(scan_code1 == 0x01){
            return 48;
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
		return 52;
	}
	else if(scan_code1 == 0x06){
		return 53;
	}
	else if(scan_code1 == 0x07){
		return 54;
	}
	else if(scan_code1 == 0x08){
		return 55;
	}
	else if(scan_code1 == 0x09){
		return 56;
	}
	else if(scan_code1 == 0x0A){
		return 57;
	}
	else if(scan_code1 == 0x0B){
		return 58;
	}
	else if(scan_code1 == 0x10){
		return 59;
	}
	else if(scan_code1 == 0x1E){
		return 97;
	}
	else if(scan_code1 == 0x30){
		return 98;
	}
	else if(scan_code1 == 0x2E){
		return 99;
	}
	else if(scan_code1 == 0x12){
		return 100;
	}
	else if(scan_code1 == 0x21){
		return 101;
	}
	else if(scan_code1 == 0x22){
		return 102;
	}
	else if(scan_code1 == 0x23){
		return 103;
	}
	else if(scan_code1 == 0x17){
		return 104;
	}
	else if(scan_code1 == 0x24){
		return 105;
	}
	else if(scan_code1 == 0x25){
		return 106;
	}
	else if(scan_code1 == 0x26){
		return 107;
	}
	else if(scan_code1 == 0x32){
		return 108;
	}
	else if(scan_code1 == 0x31){
		return 109;
	}
	else if(scan_code1 == 0x18){
		return 110;
	}
	else if(scan_code1 == 0x19){
		return 111;
	}
	else if(scan_code1 == 0x10){
		return 112;
	}
	else if(scan_code1 == 0x13){
		return 113;
	}
	else if(scan_code1 == 0x1F){
		return 114;
	}
	else if(scan_code1 == 0x14){
		return 115;
	}
	else if(scan_code1 == 0x16){
		return 116;
	}
	else if(scan_code1 == 0x2F){
		return 117;
	}
	else if(scan_code1 == 0x11){
		return 118;
	}
	else if(scan_code1 == 0x2D){
		return 119;
	}
	else if(scan_code1 == 0x15){
		return 120;
	}
	else if(scan_code1 == 0x2C){
		return 121;
	}
	else if(scan_code1 == 0x04){
		return 122;
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
