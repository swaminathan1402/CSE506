#include <sys/kprintf.h>
#include <sys/pic.h>

void outb(uint16_t port, unsigned char irq){
	__asm__ __volatile__ (
		"outb %0, %1;"
		:
		: "a"(irq), "Nd"(port)
		:
	);
}

void inb(uint16_t port){
	/*
	__asm__ __volatile__ (
		"inb %0, %;" // dump the value arrived on port to eax
		:
		:"Nd"(port)
		:
	);
	*/
}

void init_pic() {
	
	// intialize PIC_MASTER and PIC_SLAVE with ICW1
	/*
	http://stanislavs.org/helppc/8259.html
	|7|6|5|4|3|2|1|0|  ICW1
	 | | | | | | | `---- 1=ICW4 is needed, 0=no ICW4 needed
	 | | | | | | `----- 1=single 8259, 0=cascading 8259's
	 | | | | | `------ 1=4 byte interrupt vectors, 0=8 byte int vectors
	 | | | | `------- 1=level triggered mode, 0=edge triggered mode
	 | | | `-------- must be 1 for ICW1 (port must also be 20h or A0h)
	 `------------- must be zero for PC systems


	*/
	outb(PIC_MASTER, ICW1);
	outb(PIC_SLAVE, ICW1);

	// ICW2: write to data port about the vector offset
	outb(PIC_MASTER_DATA, ICW2_MASTER);  // lower 3 bits will be the interrupt number
	outb(PIC_SLAVE_DATA, ICW2_SLAVE);

	// ICW3: defining the master slave relationship
	outb(PIC_MASTER_DATA, ICW3_MASTER);
	outb(PIC_SLAVE_DATA, ICW3_SLAVE);  // TODO: Check if it actually connects to the master

	// ICW4: telling them about 8086 architecture
	outb(PIC_MASTER_DATA, ICW4_8086);
	outb(PIC_SLAVE_DATA, ICW4_8086);
}
