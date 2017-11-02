#include<sys/defs.h>
#include<sys/page.h>
#include<sys/kprintf.h>

void showAllFreePages(){
  freelist *temp = first_free_page;
  while(temp != NULL){
    kprintf("%p\n", temp);
    temp = temp->next;
  }
}

freelist *get_free_page(){
	freelist *page = first_free_page;
	first_free_page = first_free_page->next;
	first_free_page->prev = NULL;
	return page;
}


freelist *get_free_pages(int numPages){
	freelist *temp = first_free_page;
	int counter = 0;
	while(counter < numPages){
		temp = temp->next;
		counter++;
	}

	freelist *newpage = first_free_page;
	first_free_page = temp->next;
	temp->next = NULL;
	first_free_page->prev = NULL;
	
	kprintf("Newpage returned is %p\n First free page now points to %p", newpage, first_free_page);
	return newpage;
}
