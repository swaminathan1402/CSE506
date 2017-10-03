#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pic.h>
#include <sys/idt.h>
#include <sys/pci.h>
#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

hba_mem_t *ahci_mem_base;
void *memset1(void *array, int c, size_t n){
    unsigned char* temp = array;
    while(n--){
        *temp++ = (unsigned char)c;
    }
    return array;

}

void wait(){
    for(int i=0; i<10000; i++){
        for(int j=0; j<10000;j++){
	}
    }
}

void enableAHCI(){
    if((ahci_mem_base->ghc & 0x80000000) == 0) {
        ahci_mem_base->ghc |= 0x80000000; // ahci enabled
        wait();
    }
}

void resetGHC(){

    ahci_mem_base->ghc |= 1; // hard reset of ghc
    while(ahci_mem_base->ghc & 1) wait(); // Make sure reset is done completely
    wait();
}

void forcePortIdle(int SATA_PORT){
    // Make sure that the port is not performing any command lists
    if((ahci_mem_base->ports[SATA_PORT].cmd & (HBA_PxCMD_ST | HBA_PxCMD_CR | HBA_PxCMD_FR | HBA_PxCMD_FRE))){
	stop_cmd(&(ahci_mem_base->ports[SATA_PORT]));
    }
}

void setSCTL(int SATA_PORT){
	ahci_mem_base->ports[SATA_PORT].sctl = 0x301; // COMRESET has been sent to the interface
	wait(); // wait for 1 milisecond
	ahci_mem_base->ports[SATA_PORT].sctl = 0x300;
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  __asm__("sti;");
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
    }
  }
 kprintf("physfree %p\n", (uint64_t)physfree);
 kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

 int SATA_PORT;
 SATA_PORT = bruteForcePCIcheckAHCI(&ahci_mem_base, 0xa8000); // b0000
 kprintf("\nSATA PORT(using) :%d\n", SATA_PORT); 
 kprintf("IPM BEFORE: %x\n",  (ahci_mem_base->ports[SATA_PORT].ssts >> 8));
 kprintf("DET BEFORE: %x\n",  (ahci_mem_base->ports[SATA_PORT].ssts & 0x0F));	
 enableAHCI();
 resetGHC();
 enableAHCI();
 forcePortIdle(SATA_PORT);
 kprintf("starting rebase\n");
 rebase(&(ahci_mem_base->ports[SATA_PORT]) ,SATA_PORT);
 kprintf("rebase ended\n");
 setSCTL(SATA_PORT);
 if(ahci_mem_base->cap & (ahci_mem_base->cap >> 27)) {
    kprintf("Staggered spin up is supported\n");
    ahci_mem_base->ports[SATA_PORT].cmd |= (0x2 | 0x4 | 1<<28); // setting Spinup(SUD), POD, ICC
    wait();
 }
 ahci_mem_base->ports[SATA_PORT].serr_rwc = 0xffffffff;
 ahci_mem_base->ports[SATA_PORT].is_rwc = 0xffffffff;

			
 kprintf("IPM AFTER: %x\n",  (ahci_mem_base->ports[SATA_PORT].ssts >> 8));
 kprintf("DET AFTER: %x\n",  (ahci_mem_base->ports[SATA_PORT].ssts & 0x0F));
 
 
 // disbale transition to partial and slumber states
 uint64_t *c = (uint64_t *)0x7009000;
 uint64_t *a = (uint64_t *)0x2500000;

 kprintf("Writing[Starting]\n");
 for(int i=0; i<100; i++){
	 memset1(c, i, 1* 512*sizeof(c));
	 write(&ahci_mem_base->ports[SATA_PORT], i*8, 0, 8, c);
 }

 for(int i=0; i<100; i++){
    read(&ahci_mem_base->ports[SATA_PORT], i*8, 0, 8, a+(512*8*i));
 }
 for(int i=0; i<400*1024; i+=(4*1024)){
    kprintf("%d \t", (uint8_t)a[i]); // PRINTING THE CONTENTS READ FROM THE DISK
 }
 kprintf("\n we have successfully read the contents from the disk\n");
 while(1);
}

void boot(void)
{
   
  // note: function changes rsp, local stack variables can't be practically used
  register char  *temp2, *temp1;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt(); 
  init_idt();  
  initScreen();  
  init_pic();
 
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}
