#include <sys/defs.h>
#include <sys/memory.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pic.h>
#include <sys/idt.h>
#include <sys/pci.h>
#include <sys/page.h>
#include <sys/page_table.h>
#include <sys/task.h>
#include<sys/syscall.h>
#include <sys/elf64.h>
#define INITIAL_STACK_SIZE 4096
#define PAGE_SIZE 4096

uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

hba_mem_t *ahci_mem_base;

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

int readTFDRegsSuccess(int SATA_PORT){
	int busy = ahci_mem_base->ports[SATA_PORT].tfd & 0x80;
	int drq = ahci_mem_base->ports[SATA_PORT].tfd & 0x8;
	int err = ahci_mem_base->ports[SATA_PORT].tfd & 0x1;
	kprintf("\n ***Reading Error codes***\n");
	kprintf("BUSY: %d\nDRQ: %d\nERR: %d\n", busy, drq, err);
	kprintf("\n *** ***\n");
	if(!busy & !drq & !err)
		return 1;
	return 0;
}


void start(uint32_t *modulep, void *physbase, void *physfree)
{
    //__asm__("sti;");
    __asm__("sti;");
    struct smap_t {
      uint64_t base, length;
      uint32_t type;
    }__attribute__((packed)) *smap;
    while(modulep[0] != 0x9001) modulep += modulep[1]+2;
    first_free_page = (freelist *)(physfree);
    first_free_page->next = NULL;
    first_free_page->prev = NULL;
    first_free_page->reference_count = 0;
    first_free_page->page = (real_page*)(physfree);

    current_free_page = first_free_page;
    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
      if (smap->type == 1 /* memory */ && smap->length != 0) {
        kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
  	if((uint64_t)current_free_page > (uint64_t)physfree)
  	{
  	  freelist *newpage = (freelist *)smap->base;
  	  current_free_page->next = newpage;
  	  newpage->prev = current_free_page;
  	  current_free_page = newpage;
  	}
  	
	uint64_t base = (uint64_t)current_free_page + PAGE_SIZE;
  	while(base < smap->base + smap->length){
  	  freelist *newpage = (freelist *)base;
  	  newpage->prev = current_free_page;
  	  current_free_page->next = newpage;
  	  current_free_page = newpage;
  	  base += PAGE_SIZE;
  	  //kprintf("Adding %p to the free list\t", newpage);
  	}
      }
    }
    kprintf("physfree %p\n", (uint64_t)physfree);
    kprintf("physbase %p\n", (uint64_t)physbase);
    kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  
    pid = 0;  // pid generation
    uint64_t *pointer_to_pml4e = (uint64_t *)get_free_page();
    uint64_t *pointer_to_pdpe = (uint64_t *)get_free_page();
    uint64_t *pointer_to_pde = (uint64_t *)get_free_page();
    uint64_t *pointer_to_pte = (uint64_t *)get_free_page();
  
    pml4e = (PML4E *)pointer_to_pml4e;
    memset(pml4e, 0, 4096);
    pdpe = (PDPE *)pointer_to_pdpe;
    memset(pdpe, 0, 4096);
    pde = (PDE *)pointer_to_pde;
    memset(pde, 0, 4096);
    pte = (PTE *)pointer_to_pte;
    memset(pte, 0, 4096);
 
    kernel_pml4e = (uint64_t)pml4e;
    kernel_pdpe = (uint64_t)pdpe;
    kernel_pde = (uint64_t)pde;
    kernel_pte = (uint64_t)pte;
 
    (pml4e + 511)->page_directory_pointer_base_address = ((uint64_t)pointer_to_pdpe >> 12);
    (pml4e + 511)->p = 1; 
    (pml4e + 511)->rw = 1; 
    (pml4e + 511)->us = 1; 
  
    (pdpe + 510)->page_directory_base_address = (uint64_t)pointer_to_pde >> 12;
    (pdpe + 510)->p = 1;
    (pdpe + 510)->rw = 1; 
    (pdpe + 510)->us = 1;
  
    (pde + 1)->page_table_base_address = (uint64_t)pointer_to_pte >> 12;
    (pde + 1)->p = 1;
    (pde + 1)->rw = 1;
    (pde + 1)->us = 1;
  
    
    kprintf("pml4e points to %p %p\n pdpe points to %p %p\n pde points to %p %p\n %d size",
    	(pml4e + 511)->page_directory_pointer_base_address,
  	pointer_to_pdpe,
  	(pdpe + 510)->page_directory_base_address,
  	pointer_to_pde,
  	(pde+1)->page_table_base_address,
  	pointer_to_pte,
  	sizeof(pml4e));
    int size_of_kernel = (uint64_t)physfree - (uint64_t)physbase;
    kprintf("the size of kernel %x\n", size_of_kernel);
    
    init_pd(pte, pml4e, (uint64_t)physbase, size_of_kernel);
    //int *c = (int *)get_free_page();
    //kprintf("oh wow %p\n", c);
 
    /* Testing yield in different processes */

    uint64_t rflags, cr3;
    __asm__ __volatile__ (
    	"movq %%cr3, %%rax;"
	"movq %%rax, %0;"
	: "=m"(cr3)
	:
    );
    __asm__ __volatile__ (
	"pushf;"
	"pop %0;"
	: "=m"(rflags)
	:
    );
    /*
    mainTask = (task *)get_free_page();
    otherTask = (task *)get_free_page();
    idleTask = (task *)get_free_page();
    user_pml4e=(PML4E*)get_free_page();    
    user_pml4e->p =0;
    idleTask->regs.cr3= (uint64_t)user_pml4e;
	

    createTask(mainOne, rflags, cr3);
    createTask(mainTwo, rflags, cr3);
    createTask(switch_to_ring_3, rflags, cr3);
    createTask(beIdle, rflags, cr3);

    runningTask = idleTask;
    */

    /*
    __asm__ __volatile__ (
        "movq %0, %%rsi;"
    	"movq %1, %%r11;"
    	"movq %%r11, %%rsp;"
        "call %%rsi;"
        :
        : "m"(runningTask->regs.rip), "m"(mainTask->regs.rsp)
        :
    );
  kprintf("Trying page fault");
  uint64_t *ptr = (uint64_t *)0x51200000000000;
  uint64_t do_page_fault = *ptr;
  kprintf("%d" , do_page_fault);   
  */
							

  zombieProcessList= (tasklist*) get_free_page();
  runningProcessList= (tasklist*) get_free_page();
  waitProcessList= (tasklist*) get_free_page();	
  tarfs_read();
  
  read_elf(idle_elf,0);	// idle is elf type and global
//  read_elf(idle_elf);	// idle is elf type and global
  read_elf(sbush_elf,1);  // sbush is elf type and global. We create Task here 
  kprintf("we are launching to ring3\n");
  /*
  __asm__ __volatile__ (
	"movq %1, %%cr3;"
  	"movq %0, %%r11;"
  	"movq %%r11, %%rsp;"
      :
      : "m"(runningTask->regs.rsp), "r"(runningTask->pml4e)
      :
  );
  kprintf("new cr3 %p\n", runningTask->pml4e);
  */
  switch_to_ring_3();


    /* End of yield testing */
    
   //showAllFreePages();
   //freelist *newpage = get_free_pages(5);
  
  /*
  int SATA_PORT;
  SATA_PORT = bruteForcePCIcheckAHCI(&ahci_mem_base, 0xa8000); // b0000
  kprintf("\nSATA PORT(using) :%d\n", SATA_PORT); 
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
 
 
  while(readTFDRegsSuccess(SATA_PORT) == 0);
  uint64_t *c = (uint64_t *)0x7009000;
  uint64_t *a = (uint64_t *)0x2500000;

  kprintf("Writing[Status]: Started\n");
  for(int i=0; i<100; i++){
          memset1(c, i, 1* 512*sizeof(c));
          write(&ahci_mem_base->ports[SATA_PORT], i*8, 0, 8, c);
  }
  kprintf("Writing[Status]: Completed\n");
  kprintf("Reading[Status]: Started\n");
  for(int i=0; i<100; i++){
     read(&ahci_mem_base->ports[SATA_PORT], i*8, 0, 8, a+(512*8*i));
  }
  for(int i=0; i<400*1024; i+=(4*1024)){
     //kprintf("%d \t", (uint8_t)a[i]); // PRINTING THE CONTENTS READ FROM THE DISK
  }
  kprintf("\n we have successfully read the contents from the disk\n");
  */
  while(1);
}


void boot(void)
{
   
  // note: function changes rsp, local stack variables can't be practically used
  register char  *temp2, *temp1;

  for(temp2 = (char*)VIDEO_MEM_BUF+1; temp2 < (char*)VIDEO_MEM_BUF+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt(); 
  init_syscall();	  
  init_idt();  
  initScreen();  
  init_pic();
  // init_pd();
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)VIDEO_MEM_BUF;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}
