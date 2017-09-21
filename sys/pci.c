#include<sys/pci.h>
#include <sys/kprintf.h>

void outl(uint32_t port ,uint32_t val)
{
__asm__ __volatile__(
 "outl %0 , %1 " 
 :
 :"a"(val) , "d"(port)
 : 

);
}

uint32_t  inl(uint32_t port)
{
uint32_t ret;
__asm__ __volatile__(
"inl %0 , %1 "
:"=a"(ret)
:"d"(port)
:	

);
return ret;
}

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func , uint8_t offset)
{
	uint32_t address;
	uint32_t lbus= (uint32_t)bus;
	uint32_t lslot =(uint32_t)slot;
	uint32_t lfunc =(uint32_t)func; 
	uint16_t tmp; 

	address=(uint32_t)((lbus<<16)|(lslot<<11)|(lfunc<<8)|(offset&0xfc)|((uint32_t)0x80000000));

	outl (0xCF8, address);
	tmp =(uint16_t)((inl(0xCFC)>> ((offset&2)*8)) & 0xffff);
	return (tmp);
}


uint16_t pciCheckVendor(uint8_t bus, uint8_t slot)
{
	uint16_t vendor;
	vendor =pciConfigReadWord(bus,slot,0,0);
	return vendor;
}

uint8_t pciCheckBaseClass(uint8_t bus, uint8_t slot)
{
	uint8_t class;
	uint16_t classinfo;
	classinfo =pciConfigReadWord(bus,slot,0,6); 
	class =(uint8_t)((classinfo & 0xff00)>>16);
	return class;
}

uint8_t pciCheckSubClass(uint8_t bus, uint8_t slot)
{
	uint8_t subclass;
	uint16_t classinfo;
	classinfo =pciConfigReadWord(bus,slot,0,6); 
	subclass =(uint8_t)((classinfo & 0x00ff));
	return subclass;
}

void bruteForcePCIcheckAHCI()
{
uint8_t bus ;
uint8_t device;
for (bus= 0; bus<256; bus++)
	{

		for(device=0; device<32;device++)
		{
		
			if (pciCheckVendor(bus,device)!=0xffff)
			{
			 if (pciCheckBaseClass(bus,device)==0x01 && pciCheckSubClass(bus,device)==0x06 )
			 kprintf("\nAHCI Controller discovered , VendorID:%d", pciCheckVendor(bus,device));	
		         kprintf("\nAHCI present at bus no: %d ,device %d", bus, device);
			}
		}

	}

}


