#ifndef _PAGE_TABLE_H
#define _PAGE_TABLE_H

#include<sys/defs.h>
#define NO_OF_ENTRIES 512

uint64_t  page_directory[1024] __attribute__((aligned(4096)));
uint64_t  page_table[1024] __attribute__((aligned(4096)));

uint64_t *pointer_to_page_directory;
uint64_t *pointer_to_page_table;
void init_pt();
void setMap(uint64_t, uint64_t, int);
void changeUserPrivilegePage(uint64_t);

typedef struct {
	uint8_t p:1;
	uint8_t rw:1;
	uint8_t us:1;
	uint8_t pwt:1;
	uint8_t pcd:1;
	uint8_t a:1;
	uint8_t ign:1;
	uint8_t mbz:2;
	uint8_t avl:3;
	uint64_t page_directory_pointer_base_address: 40;
	uint16_t available: 11;
	uint8_t nx:1;
}__attribute__((packed)) PML4E;


typedef struct {
	uint8_t p:1;
	uint8_t rw:1;
	uint8_t us:1;
	uint8_t pwt:1;
	uint8_t pcd:1;
	uint8_t a:1;
	uint8_t ign:1;
	uint8_t zero:1;
	uint8_t mbz:1;
	uint8_t avl:3;
	uint64_t page_directory_base_address: 40;
	uint16_t available: 11;
	uint8_t nx:1;
}__attribute__((packed)) PDPE;


typedef struct {
	uint8_t p:1;
	uint8_t rw:1;
	uint8_t us:1;
	uint8_t pwt:1;
	uint8_t pcd:1;
	uint8_t a:1;
	uint8_t ign_part_2:1;
	uint8_t zero:1;
	uint8_t ign_part_1:1;
	uint8_t avl:3;
	uint64_t page_table_base_address: 40;
	uint16_t available: 11;
	uint8_t nx:1;
}__attribute__((packed)) PDE;

typedef struct {
	uint8_t p:1;
	uint8_t rw:1;
	uint8_t us:1;
	uint8_t pwt:1;
	uint8_t pcd:1;
	uint8_t a:1;
	uint8_t d:1;
	uint8_t pat:1;
	uint8_t g:1;
	uint8_t avl:2;
	uint8_t cow:1;
	uint64_t physical_address: 40;
	uint16_t available: 11;
	uint8_t nx:1;
}__attribute__((packed)) PTE;



PML4E *pml4e;
PDPE *pdpe;
PDE *pde ;
PTE *pte;

uint64_t kernel_pml4e;
uint64_t kernel_pdpe;
uint64_t kernel_pde ;
uint64_t kernel_pte;

void init_pd(PTE*,PML4E *, uint64_t, int);
void changeCR3(PML4E*, PDPE*, PDE*, PTE*, int);
void deepCopyPageTable(uint64_t);
uint64_t checkCOW();
void setCOW(uint64_t , uint64_t);
void setWritable(uint64_t , uint64_t);
void reloadCR3();
#endif
