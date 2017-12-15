#ifndef _PAGE_H
#define _PAGE_H
#include <sys/defs.h>
#include <sys/kprintf.h>

typedef struct freelist {
  struct freelist *next;
  struct freelist *prev;
}__attribute__((aligned(4096))) freelist;

freelist *first_free_page;
freelist *current_free_page;

void showAllFreePages();
freelist *get_free_page();
freelist *get_free_pages(int numPages);
freelist *get_free_user_page();

uint64_t err_code;
#endif
