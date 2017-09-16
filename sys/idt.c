#include<sys/defs.h>
#include<sys/idt.h>
#include <sys/kprintf.h>
//#include<sys/timer_wrapper.h> 

#define IDT_MAX 256

struct idt_entry
{
	uint16_t offset_low:16; //Offset low bits
	uint16_t selector:16; //code segment selector 
	uint8_t zerob:8;  // 1 byte reseved as zero
	uint8_t type:8 ; // Type and attributes
	//uint16_t offset_low; //Offset low bits
	uint16_t offset_mid:16; //Offset mid bits
	uint32_t offset_high:32;//Offset high bits  
	uint32_t zeroq:32; //Zero quad.
}idt[IDT_MAX];

struct idtr_t
{
	uint16_t size:16;
	uint64_t base:64; 
}__attribute__((packed));

extern void timer_wrapper();
/*
void sample_handler() {
	kprintf("holla do you hear me ? ");
}
*/

//void _x86_64_asm_lidt(struct idtr_t *idtr)


void idt_entry_table(int index, uint64_t function)
{
	idt[index].zerob=0;
	idt[index].zeroq=0;
	idt[index].type=0x8e;
	idt[index].selector=0x8;
	idt[index].offset_low= (function & 0xffff);
	idt[index].offset_mid= (function & 0xffff0000)>>16;
	idt[index].offset_high=(function & 0xffffffff00000000)>>32;
}


void init_idt() {
	for(int i=0; i<256; i++){
		idt_entry_table(i, (uint64_t)timer_wrapper);
	}

	struct idtr_t idtr;
	idtr.size=sizeof(struct idt_entry)*IDT_MAX-1;
	idtr.base=(uint64_t)idt; 
	//LIDT instruction arguments??
	//_x86_64_asm_lidt(idtr);
	__asm__(
		"lidt %0"
		:
		:"m"(idtr)
		:"memory"
	);
}



