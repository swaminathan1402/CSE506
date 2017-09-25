#include<sys/defs.h>
#include<sys/pci.h>
#include <sys/kprintf.h>
#include<sys/ahci.h>
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
	uint16_t tmp=0; 
	address=(uint32_t)((lbus<<16)|(lslot<<11)|(lfunc<<8)|(offset&0xfc)|((uint32_t)0x80000000));

	outl (0xCF8, address);
	tmp =(uint16_t)((inl(0xCFC)>> ((offset&2)*8)) & 0xffff);
	return (tmp);
}

void pciConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func , uint8_t offset)
{
	uint32_t address;
	uint32_t lbus= (uint32_t)bus;
	uint32_t lslot =(uint32_t)slot;
	uint32_t lfunc =(uint32_t)func; 
	address=(uint32_t)((lbus<<16)|(lslot<<11)|(lfunc<<8)|(offset&0xfc)|((uint32_t)0x80000000));

	outl (0xCF8, address);
	outl(0xCFC, 0x10000000);  // 0x100000
}

uint16_t pciCheckVendor(uint8_t bus, uint8_t slot,uint8_t function)
{
	uint16_t vendor;
	vendor =pciConfigReadWord(bus,slot,function,0);
	return vendor;
}

uint16_t pciCheckDevice(uint8_t bus, uint8_t slot)
{
	uint16_t deviceID;
	deviceID =pciConfigReadWord(bus,slot,0,2);
	return deviceID;
}

uint8_t pciCheckBaseClass(uint8_t bus, uint8_t slot)
{
	uint8_t class;
	uint16_t classinfo;
	classinfo =pciConfigReadWord(bus,slot,0x0,0x0A); 
	class =(uint8_t)((classinfo & 0xff00)>>8);
	return class;
}

uint8_t pciCheckProgIF(uint8_t bus, uint8_t slot)
{
	uint8_t prog;
	uint16_t classinfo;
	classinfo =pciConfigReadWord(bus,slot,0x0,0x08); 
	prog =(uint8_t)((classinfo & 0xff00)>>8);
	return prog;
}



uint8_t pciCheckSubClass(uint8_t bus, uint8_t slot )
{
	uint8_t subclass;
	uint16_t classinfo;
	classinfo =pciConfigReadWord(bus,slot,0,0x0A); 
	subclass =(uint8_t)((classinfo & 0x00ff));
	return subclass;
}

uint32_t pciCheckBarFive(uint8_t bus, uint8_t device)
{
	uint32_t lbarfive_hi,lbarfive_lo;
	uint32_t barfive;
	lbarfive_lo=pciConfigReadWord(bus, device,0,0x24);
	lbarfive_hi= pciConfigReadWord(bus,device,0,0x26);
	barfive = (uint32_t)(lbarfive_lo |(lbarfive_hi<<16)); 	
	return barfive;
}

void pciCheckFunction (uint8_t bus, uint8_t device, uint8_t function, hba_mem_t **ahci_mem_base)
{
//kprintf( "\t%d\t%d" ,pciCheckBaseClass(bus, device, function), pciCheckSubClass(bus,device,function));

//kprintf("%d\t ",bus);
//kprintf("%d \t", device);
if(function==0)	
{	if(pciCheckBaseClass(bus, device)==0x01 &&  pciCheckSubClass(bus,device)==0x06)
	{
            	if(pciCheckProgIF(bus,device)==0x01 )
		 {	
				uint16_t deviceID= pciCheckDevice(bus,device);
				kprintf("\nAHCI DeviceID :%d",(int)deviceID);
				kprintf("\nBus: %d",(int)bus);
				kprintf("\nDevice: %d\n",(int)device);
				pciConfigWriteWord(bus, device, 0, 0x24);
				uint64_t abar5 = (uint64_t)pciCheckBarFive(bus,device);
				hba_mem_t *abar = (hba_mem_t *)abar5;	
				*ahci_mem_base = abar;
				//kprintf("%p\n", abar);
				probe_AHCI(abar);
		}
	}
}
//return;
}

uint8_t pciCheckHeader(uint8_t bus, uint8_t device)
{
uint8_t head;
uint16_t word =pciConfigReadWord(bus, device,0, 0x0E);
head = (uint8_t)(word & 0x00FF);
return head;

}


void bruteForcePCIcheckAHCI(hba_mem_t **ahci_mem_base)
{
uint32_t bus ;
uint8_t device;
uint8_t header;
//kprintf("Brute force started!!\n");
for (bus=0; bus<256; bus++)
	{

		for(device=0; device<32;device++)
		{
			uint8_t function=0;
			//kprintf("%d,%d\t" ,bus+1,device+1);
			if (pciCheckVendor(bus,device,function)!=0xffff)
			{
				pciCheckFunction (bus, device,function, ahci_mem_base);
				header=pciCheckHeader(bus, device);
				if((header & 0x80)==1)	
				{
					for(function =1 ;function <8 ;function++)
					{	
						if(pciCheckVendor(bus,device,function)!=0xFFFF)
					 		pciCheckFunction(bus, device,function, ahci_mem_base);		
					}
				}
			}	

		}		
	}
//kprintf("Brute Force ended\n");
//return;
}

