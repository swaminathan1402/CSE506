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
 int SATA_PORT =bruteForcePCIcheckAHCI(&ahci_mem_base);
 kprintf("\nPort returned to main :%d", SATA_PORT); 
 ahci_mem_base->ghc = ahci_mem_base->ghc | 0x01;
 ahci_mem_base->ghc |= 0x02;
 for(int i=0; i<32; i++){
    rebase(&(ahci_mem_base->ports[i]) ,i);
 }
 kprintf("Rebased!\n");
 
 //uint64_t *c = 0x30000000;
 uint64_t *c = (uint64_t *)0x7009000;
 uint64_t *a = (uint64_t *)0x2500000;
 //*c = 5;
 /*
 memset1(c, 1, 1* 1024*sizeof(c));
 int status_write = write(&ahci_mem_base->ports[SATA_PORT], 9, 0, 16, c);
 int status_read = read(&ahci_mem_base->ports[SATA_PORT], 9, 0, 16, a);

 memset1(c, 2, 1* 1024*sizeof(c));
 status_write = write(&ahci_mem_base->ports[SATA_PORT], 1, 0, 16, c);
 status_read = read(&ahci_mem_base->ports[SATA_PORT], 1, 0, 16, a+1024);

 memset1(c, 3, 1* 1024*sizeof(c));
 status_write = write(&ahci_mem_base->ports[SATA_PORT], 3, 0, 16, c);
 status_read = read(&ahci_mem_base->ports[SATA_PORT], 3, 0, 16, a+1024+1024);

 memset1(c, 4, 1* 1024*sizeof(c));
 status_write = write(&ahci_mem_base->ports[SATA_PORT], 4, 0, 16, c);
 status_read = read(&ahci_mem_base->ports[SATA_PORT], 4, 0, 16, a+1024+1024+1024);
 */
 for(int i=0; i<100; i++){
  for(int j=0; j<4; j++){
	 memset1(c, i, 1* 1024*sizeof(c));
	 write(&ahci_mem_base->ports[SATA_PORT], 4*i+j, 0, 16, c);
	 //read(&ahci_mem_base->ports[SATA_PORT], 4*i+j, 0, 16, a+(1024*(4*i+j)));
  }
 }
 for(int i=0; i<100; i++){
  for(int j=0; j<4; j++){
    read(&ahci_mem_base->ports[SATA_PORT], 4*i+j, 0, 16, a+(1024*(4*i+j)));
  }
 }
 for(int i=0; i<400*1024; i++){
    kprintf("%d \t", (uint8_t)a[i]);
 }
 /*
for (int i=0 ;i < 90 ;i++ ){
 __asm__ ("int $32");
}
*/
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
 
 
 
 
 
//  bruteForcePCIcheckAHCI(); 
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
 //bruteForcePCIcheckAHCI();
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}
