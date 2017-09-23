#include<sys/defs.h>
#include<sys/ahci.h>
#include<sys/kprintf.h>

static int check_type(hba_port_t *port)
{
 uint32_t ssts= port->ssts;
 uint8_t ipm= (ssts>>8)&0x0F;
 uint8_t det= ssts&0x0F; 

 if (det && ipm) //Check drive status register
{
	switch (port->sig)
	{
          case AHCI_DEV_SATAPI:
		return AHCI_DEV_SATAPI;
	  case AHCI_DEV_SEMB:
		return AHCI_DEV_SEMB;
	   case AHCI_DEV_PM:
		return AHCI_DEV_PM;
	   default:
		return AHCI_DEV_SATA;
	}	
}
return 0;
}


void probe_AHCI(hba_mem_t 	*abar)
{
uint32_t pi= abar->pi;
int i=0;
      while (i<32)
       {       
     		 if(pi & 1)
		{
			int dt =check_type(&abar ->ports[i]);		
			if (dt ==AHCI_DEV_SATA)

				kprintf("SATA drive found at port %d\n",i);
				return;
			}
			else if(dt ==AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n",i);	
	                	return;
			}
			else if(dt==AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n",i);
				 return;
			}
			else if(dt ==AHCI_DEV_PM )
			{
				kprintf("PM drive found at port %d\n",i);	 
                                return;
			}
		}
		pi>>=1;
		i++;
	}
}
