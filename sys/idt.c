#include<sys/defs.h>
#include<sys/idt.h>
//#include<sys/timer_wrapper.h> 

#define IDT_MAX 256

struct idt_entry
{
uint16_t offset_low; //Offset low bits
uint16_t selector; //code segment selector 
uint8_t zerob;  // 1 byte reseved as zero
uint8_t type ; // Type and attributes
uint16_t offset_mid; //Offset mid bits
uint32_t offset_high;//Offset high bits  
uint32_t zeroq; //Zero quad.
}idt[IDT_MAX];

struct idtr_t
{
uint16_t size;
uint64_t base; 
}idtr;

extern void timer_wrapper();

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


void init_idt()
{

//Initialize idt for timer interrupt:	
/*
	idt[0]->zerob=0;
	idt[0]->zeroq=0;
	idt[0]->type=0x8e   ;
 	idt[0]->offset_low= timer_wrapper & 0x000000000000ffff;
	idt[0]->offset_mid= (timer_wrapper & 0x00000000ffff0000)>>16;
	idt[0]->offset_high= (timer_wrapper & 0xffffffff00000000)>>32;	
	idt[0]->selector= 0x8;
*/

idt_entry_table(32,(uint64_t)timer_wrapper);

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



