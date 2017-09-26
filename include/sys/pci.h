#include<sys/defs.h>
#ifndef _PCI_H
#define _PCI_H

void outl (uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);
int  bruteForcePCIcheckAHCI();

#endif
