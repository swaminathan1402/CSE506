#ifndef _PAGE_TABLE_H
#define _PAGE_TABLE_H

#include<sys/defs.h>
/*
struct pd_entry {
	uint8_t present:1;
	uint8_t read_write:1;
	uint8_t user_mode:1;
	uint8_t write_through:1;
	uint8_t cache_disabled:1;
	uint8_t accessed:1;
	uint8_t zero:1;
	uint8_t page_size:1;
	uint8_t ignored:1;
	uint8_t available:3;
	uint32_t pt: 20;

}__attribute__((packed));

struct pt_entry {
	uint8_t present:1;
        uint8_t read_write:1;
        uint8_t user_mode:1;
        uint8_t write_through:1;
        uint8_t cache_disabled:1;
        uint8_t accessed:1;
        uint8_t dirty:1;
	uint8_t zero:1;
	uint8_t global:1;
        uint8_t available:3;
        uint32_t phy_addr: 20;
}__attribute__((packed));
*/
uint64_t  page_directory[1024] __attribute__((aligned(4096)));
uint64_t  page_table[1024] __attribute__((aligned(4096)));

void init_pd();
void init_pt();
#endif
