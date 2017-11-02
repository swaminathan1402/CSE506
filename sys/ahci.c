#include<sys/defs.h>
#include<sys/memory.h>
#include<sys/ahci.h>
#include<sys/kprintf.h>
#define AHCI_BASE 0x400000 
int find_cmdslot(hba_port_t *port);



// void *memset(void *array, int c, size_t n){
//     unsigned char* temp = array;
//     while(n--){
//         *temp++ = (unsigned char)c;
//     }
//     return array;
// 
// }

static int check_type(hba_port_t *port)
{
 //uint32_t ssts= port->ssts;
 //uint8_t ipm= (ssts>>8) & 0x0F;
 //uint8_t det= ssts & 0x0F; 

// if (det && ipm) //Check drive status register
//{
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
//}
return 0;
}


int probe_AHCI(hba_mem_t *abar)
{
uint32_t pi = abar->pi;
kprintf("THE PI VALUE: %p\n", abar->pi);
int count = 0;
int i=0;
      while (i<32)
       {       
     		 if(pi & 1)
		{
			int dt =check_type(&abar ->ports[i]);		
			if (dt ==AHCI_DEV_SATA){
				
				kprintf("SATA drive found at port %d\n",i);
				count++;
		
			}
			else if(dt ==AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n",i);	
				count++;
	                 //	return;
			}
			else if(dt==AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n",i);
				count++;
				// return;
			}
			else if(dt ==AHCI_DEV_PM )
			{
				kprintf("PM drive found at port %d\n",i);	 
				count++;
                             //   return;
			}
		}
		if(count == 1){
			return i;
		}
		pi>>=1;
		i++;
	}
return -1;
}

void start_cmd(hba_port_t *port){
	while(port->cmd & HBA_PxCMD_CR);
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;	
}

void stop_cmd(hba_port_t *port){
	port->cmd &= ~HBA_PxCMD_ST;
	while(1){
		if(port->cmd & HBA_PxCMD_FR)
			continue;
		if(port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
	port->cmd &= ~HBA_PxCMD_FRE;
}

void rebase(hba_port_t *port, int port_number){
	stop_cmd(port); // telling the port to not to listen for any FIS 	
	// each port should be 1K aligned =>
	// Each port has 32 command headers 
	// and each command header size 32 bytes
	// size => 32 * 32 = 1K
	port->clb = AHCI_BASE + (port_number << 10);
	memset((void *)(port->clb), 0, 1024);
	// each port size is 1K
	// our FIS offset will be 32 * 1K = 32K
	// each fis port size is 256 bytes: tagHBA_FIS in osdev
	port->fb = AHCI_BASE + (32 << 10) + (port_number << 8);
	memset((void *)(port->fb), 0, 256);

	// since there are 32 ports and each port_fis size is 256
	// total size = 32 * 256 = 8192 => 8KB
	// there is already 32K occupied by clb.
	// therefore offset is 32 + 8 = 40KB

	hba_cmd_header_t *cmdHeader = (hba_cmd_header_t *) port->clb;
	// there are 32 command headers in a command_list / clb
	for(int i=0; i<32; i++){
		cmdHeader[i].prdtl = 8;  // Number of entries in prdt
		// (port << 13) is done because, each commandHeader has 1 command tables. Each command table is 256 bytes
		// and there are 32 command headers
		// Therefore, 32 * 256 => 8K
		// also the size of command table is 256.. how did we arrive there ? 64+16+48+(sizeof(PRDT) * #entries)
		// size of prdt is 16 bytes and number of entries is 8
		cmdHeader[i].ctba = AHCI_BASE + (40 << 10) + (port_number << 13) + (i << 8);
		memset((void *)cmdHeader[i].ctba, 0, 256);
	}
	start_cmd(port);
}
  
 // Find a free command list slot
 int find_cmdslot(hba_port_t *port)
 {
 	// If not set in SACT and CI, the slot is free
 	uint32_t slots = (port->sact | port->ci);
 	int cmdslots = 32;
 	for (int i=0; i<cmdslots; i++)
 	{
 		if ((slots&1) == 0)
 			return i;
 		slots >>= 1;
 	}
 	kprintf("Cannot find free command list entry\n");
 	return -1;
 }

int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t *buf)
{
	port->is_rwc = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return -1;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
	cmdheader->w = 0; // Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
	// 8K bytes (16 sectors) per PRDT
        int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024; // 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 1024;
		count -= 16;	
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->count =count; 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return -1;
		}
	}
 
	
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return -1;
	}
 
	return 1;
}

int write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t *buf)
{
    	port->is_rwc= (uint32_t)-1;
	int spin = 0;	 
	int slot = find_cmdslot(port);
	if (slot == -1)
		return -1;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 1;		// Write to device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	
        int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;
		cmdtbl->prdt_entry[i].i = 1;
		buf += 1024;	
		count -= 16;	
	}
	
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->count =count; 
	
	while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}
 
	port->ci = 1<<slot;	 
	
	while (1)
	{
		
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return -1;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return -1;
	}
 
	return 1;
}
