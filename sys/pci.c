#include<sys/defs.h>
#include<sys/pci.h>
#include <sys/kprintf.h>

void outl(uint16_t port ,uint32_t val)
{
__asm__ __volatile__(
 "outl %0 , %1 " 
 :
 :"a"(val) , "d"(port)
 : 

);
}

uint32_t  inl(uint16_t port)
{
uint32_t ret;

__asm__ __volatile__(
"inl %1 , %0 "
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


uint16_t pciCheckVendor(uint8_t bus, uint8_t slot,uint8_t function)
{
	uint16_t vendor;
	vendor =pciConfigReadWord(bus,slot,function,0);
	return vendor;
}

uint8_t pciCheckBaseClass(uint8_t bus, uint8_t slot,uint8_t func)
{
	uint8_t class;
	uint16_t classinfo;
	classinfo =pciConfigReadWord(bus,slot,func,0x0A); 
	class =(uint8_t)((classinfo & 0xff00)>>16);
	return class;
}

uint8_t pciCheckSubClass(uint8_t bus, uint8_t slot ,uint8_t func)
{
	uint8_t subclass;
	uint16_t classinfo;
	classinfo =pciConfigReadWord(bus,slot,func,0xA); 
	subclass =(uint8_t)((classinfo & 0x00ff));
	return subclass;
}

void pciCheckFunction (uint8_t bus, uint8_t device, uint8_t function)
{
kprintf( "\t%d\t%d" ,pciCheckBaseClass(bus, device, function), pciCheckSubClass(bus,device,function));
if(pciCheckBaseClass(bus, device, function)==0x1 &&  pciCheckSubClass(bus,device,function)==0x6)
{

	kprintf("\n AHCI discovered");
	kprintf("\n %d, %d", bus, device);
}

}

uint8_t pciCheckHeader(uint8_t bus, uint8_t device)
{
uint8_t head;
uint16_t word =pciConfigReadWord(bus, device,0, 0x0C);
head = (uint8_t)(word & 0x00FF);
return head;

}


void bruteForcePCIcheckAHCI()
{
uint8_t bus ;
uint8_t device;
uint8_t header;
kprintf("Brute force started!!\n");
for (bus= 0; bus<256; bus++)
	{

		for(device=0; device<32;device++)
		{
			uint8_t function=0;
			if (pciCheckVendor(bus,device,function)!=0xffff)
			{
				pciCheckFunction (bus, device,function);
				header=pciCheckHeader(bus, device);
				if((header & 0x80)==1)	
				{
					for(function =1 ;function <8 ;function++)
					{	
						if(pciCheckVendor(bus,device,function)!=0xFFFF)
						pciCheckFunction(bus, device,function);		
					}
				}
			}	

		}		
	}
kprintf("Brute Force ended\n");
}

