#include<sys/page_table.h>
#include<sys/kprintf.h>

extern void loadPageDirectory(uint64_t *);
extern void enablePaging();

void init_pt()
{
	kprintf("Initializing page table \n");
	for(int i=0;i<1024; i++){
		 // page_table[i].present=0x1;
		 // page_table[i].read_write=0x1;
		 // page_table[i].phy_addr = i;
		page_table[i] = (i * 0x1000) | 3;
	}

}


void init_pd(){
	kprintf("Initializing page directory \n");
	for (int i=0; i<1024;i++){
		// page_directory[i].present =0x0;
		// page_directory[i].read_write=0x1;
		page_directory[i] = 0x2;
	}
	init_pt();
	page_directory[0] = ((uint64_t)page_table); //| 3;
	loadPageDirectory(page_directory);
	enablePaging();
	kprintf("Page table has been initialized\n");
}


	
