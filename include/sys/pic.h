#include <sys/defs.h>
#ifndef _PIC_H
#define _PIC_H

// Each PIC has (command, data) port 
#define PIC_MASTER 		0x20 // base addr of master pic
#define PIC_SLAVE 		0xA0  // base addr of slave pic
#define PIC_MASTER_COMMAND 	PIC_MASTER
#define PIC_MASTER_DATA 	PIC_MASTER + 1
#define PIC_SLAVE_COMMAND 	PIC_SLAVE
#define PIC_SLAVE_DATA 		PIC_SLAVE + 1

#define PIC_MASTER_OFFSET 	0x20
#define PIC_SLAVE_OFFSET 	0x28

#define PIC_EOI			0x20

#define ICW1			0x11 //0x00010001 // ICW1_INIT and ICW1_ICW4 required
#define ICW2_MASTER		0x20 //0x00100000 // send to data port: OFFSET 
#define ICW2_SLAVE		0x28 //0x00101000  // send to data port OFFSET
#define ICW3_MASTER		0x04//0x00000100  // Theres a slave PIC at IRQ2 
#define ICW3_SLAVE 		0x02//0x00000010 // slave lower 3 bits signifying masters interrupt request // TODO: CONFUSION

#define ICW4_8086		0x01



void outb(uint16_t port, unsigned char irq);
void inb(uint16_t port);
void end_of_interrupt(unsigned char irq);
void init_pic();

#endif
