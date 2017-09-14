#include<sys/defs.h>
#include<sys/idt.h>

#define IDT_MAX 20

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
};

//void _x86_64_asm_lidt(struct idtr_t *idtr)

void init_idt()
{
//Initialize idt for timer interrupt:
for (int i=0; i< IDT_MAX;i++)
{	idt[i]->zerob=0;
	idt[i]->zeroq=0;
	idt[i]->type=0xe;


}
idtr_t idtr;
idtr->size=8*IDT_MAX-1;
idtr->base=idt; 

//LIDT instruction arguments??
//_x86_64_asm_lidt(idtr);
__asm__(
"lidt %0"
:
:"m"(idtr)
:"memory"
);

}

