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
int offset = 0;

int getCharfromCode(unsigned char scan_code1)
{
	if(scan_code1 == 0x2a || scan_code1 == 0x36){ // left shift and right shift pressed 
		shiftFlag = 1;
		offset = 32;
		return -1;
	}
	/*
	else if(scan_code1 == 0x1e){ // left ctrl
		ctrlFlag = 1;
		return -1;
	}
	
	else if(scan_code1 == 0x9e) { // left ctrl released
		ctrlFlag = 0;
		return -1;
	}
	*/
	else if(scan_code1 == 0xaa || scan_code1 == 0xb6) { // left shift or right shift released 
		shiftFlag = 0;
		offset = 0;
		return -1;
	}
        else if(scan_code1 == 0x01){ // escape is pressed
		return 0;
        }
	else if(scan_code1 == 0x02){ // 1
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
		return 48;
	}
	else if(scan_code1 == 0x10){
		return 113 - offset;
	}
	else if(scan_code1 == 0x1E){
		return 97 - offset;
	}
	else if(scan_code1 == 0x30){
		return 98- offset;
	}
	else if(scan_code1 == 0x20){
		return 99 - offset;
	}
	else if(scan_code1 == 0x2E){
		return 100- offset;
	}
	else if(scan_code1 == 0x12){
		return 101- offset;
	}
	else if(scan_code1 == 0x21){
		return 102- offset;
	}
	else if(scan_code1 == 0x22){
		return 103- offset;
	}
	else if(scan_code1 == 0x23){
		return 104- offset;
	}
	else if(scan_code1 == 0x17){
		return 105- offset;
	}
	else if(scan_code1 == 0x24){
		return 106- offset;
	}
	else if(scan_code1 == 0x25){
		return 107- offset;
	}
	else if(scan_code1 == 0x26){
		return 108- offset;
	}
	else if(scan_code1 == 0x32){
		return 109- offset;
	}
	else if(scan_code1 == 0x31){
		return 110- offset;
	}
	else if(scan_code1 == 0x18){
		return 111- offset;
	}
	else if(scan_code1 == 0x19){
		return 112- offset;
	}
	else if(scan_code1 == 0x10){
		return 113- offset;
	}
	else if(scan_code1 == 0x13){
		return 114- offset;
	}
	else if(scan_code1 == 0x1F){
		return 115- offset;
	}
	else if(scan_code1 == 0x14){
		return 116- offset;
	}
	else if(scan_code1 == 0x16){
		return 117- offset;
	}
	else if(scan_code1 == 0x2F){
		return 118- offset;
	}
	else if(scan_code1 == 0x11){
		return 119- offset;
	}
	else if(scan_code1 == 0x2D){
		return 120- offset;
	}
	else if(scan_code1 == 0x15){
		return 121- offset;
	}
	else if(scan_code1 == 0x2C){
		return 122- offset;
	}
	else if(scan_code1 == 0x04){
		return 123- offset;
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
	  kprintf("%c ", map);
	}
	end_of_interrupt(0x0);	
}
