#include <sys/task.h>
#include <sys/kprintf.h>
#include <sys/page.h>
#include <sys/memory.h>
#include <sys/page_table.h>
#include <sys/gdt.h>
#include <sys/syscall.h>
#include <sys/tarfs.h>
#include <sys/elf64.h>
#include <sys/mm_struct.h>
#include <sys/string.h>

void yield(){

	/*
		1. Populate rdi, rsi with me and next respectively.
		2. get the rip and store it in runningTask ->regs.rip

		3. Push rax ,rbx, rcx, rdx, flags
		4. Change the stack pointer of runningTask to next stack by accessing task*(rsi)->regs.rsp
		5. Change the rip with the next Task by accessing (task*(rsi))->regs.rip
		6. Pop rax rbx rcx rdx for the next task
	*/
	uint64_t fn_to_execute = (uint64_t)switch_to_ring_3;
	__asm__ __volatile__ (
		"movq %0, %%rdi;"
		"movq %1, %%rsi;"
		:
		:"m"(runningTask), "m"(runningTask->next)
		:
	);
	runningTask = runningTask->next;


	// storing the rip in runningTask
	//__asm__ __volatile__ (
	//	"pop %%r13;"
	//	"movq %%r13, 16(%%rdi);"
	//	:/*"=m"(runningTask->regs.rip)*/
	//	:
	//	:
	//);

	// storing the regs in runningTask
	__asm__  __volatile__ (
		"push %%rax;"
		"push %%rbx;"
		"push %%rcx;"
		"push %%rdx;"
		"pushf;"
		:
		:
		:
	);
	// updating the value of runningTask (rsp)
	__asm__ __volatile__ (
		"movq %%rsp, (%%rdi);"
		:/*"=m"(runningTask->regs.rsp)*/
		:
		:
	);

	// shift to next task's stack
	__asm__ __volatile__(
		"movq (%%rsi), %%rsp;"
		"popf;"
		"pop %%rdx;"
		"pop %%rcx;"
		"pop %%rbx;"
		"pop %%rax;"
		:
		: 
		:"r11"
	);
	/*
	__asm__ __volatile__ (
		"movq %%rsi, %%r13;"
		"movq %%r13, %0;"
		:"=m"(runningTask)
		:
		:
	);
	*/
	__asm__ __volatile__ (
		"movq %%rsp, (%%rsi);"
		:/*"=m"(runningTask->regs.rsp)*/
		:
		:
	);

	/*
	__asm__ __volatile__ (
		"call 16(%%rsi);"
		:
		: 
		:
	);
	*/
	__asm__ __volatile__ (
		"call %0;"
		:
		:"m"(fn_to_execute)
		:
	);
}


/*
void mainTwo() {
	kprintf("@MainTwo ---- 1\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}
	yield();
	kprintf("@MainTwo ---- 2\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}
	yield();
	
}

void mainOne() {
	kprintf("@MainOne ---- 1\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}	
	yield();
	kprintf("@MainOne ---- 2\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}
	yield();
}
void beIdle(){
	kprintf("Awesome");
	while(1){
		//yield();
	}

}
*/


void remove_page(uint64_t rm_page){
	//kprintf("removing this page %p\n", rm_page);
	memset(rm_page, 0, 4096);
	freelist *newpage = (freelist *)rm_page;
	newpage->next = first_free_page;
	newpage->prev = first_free_page->prev;
	first_free_page->prev->next = newpage;

	first_free_page->prev = newpage;
	first_free_page = newpage;
	//kprintf("first_free_page is %p %p\n", first_free_page, rm_page);

}


void free(task *zombie_process){
	// walk down the page tables
	// zero down the pages 
	// add it to the free list 
	// make zombie process null
	//changeCR3(runningTask->pml4e, runningTask->pdpe, runningTask->pde, runningTask->pte, 0);

	int pages_to_free=0;
	PML4E *zpml4e = zombie_process->pml4e;
	PDPE *zpdpe = zombie_process->pdpe;
	PDE *zpde = zombie_process->pde;
	PTE *zpte = zombie_process->pte;
	for(int pml4e_index=0; pml4e_index<510; pml4e_index++){
		
		if((zpml4e + pml4e_index)->p == 1){
			zpdpe = (PDPE *)(uint64_t)((zpml4e + pml4e_index)->page_directory_pointer_base_address << 12);
			for(int pdpe_index=0; pdpe_index < 512; pdpe_index++){
				if((zpdpe + pdpe_index)->p == 1){
					zpde = (PDE *)(uint64_t)((zpdpe + pdpe_index)->page_directory_base_address << 12);
					for(int pde_index=0; pde_index < 512; pde_index++){
						if((zpde + pde_index)->p == 1){
							//kprintf("bulls eye %d %d %d\n", pml4e_index, pdpe_index, pde_index);
							zpte = (PTE *)(uint64_t)((zpde + pde_index)->page_table_base_address << 12);
							for(int pte_index = 0; pte_index < 512; pte_index++){
								if((zpte + pte_index)->p == 1){
									uint64_t free_this_page = (uint64_t)((zpte + pte_index)->physical_address << 12);
									/*
									kprintf("Free this page %p\n", free_this_page);
									//memset(free_this_page, 0, 4096);
									freelist *newpage = (freelist *)free_this_page;
									newpage->next = first_free_page;
									first_free_page->prev = newpage;
									first_free_page = newpage;
									pages_to_free++;
									*/
									
								}
							}
							remove_page((uint64_t)zpte);
							pages_to_free++;
						}
						
					}
					remove_page((uint64_t)zpde);
					pages_to_free++;
					
				}

			}
			remove_page((uint64_t)zpdpe);
			pages_to_free++;
		}
	}

	remove_page((uint64_t)zpml4e);
	pages_to_free++;

	// reap VMA : TODO
	int index = 0;
	int count = 0;
	mm_struct *mm_to_clean = zombie_process->mm;
	vm_area_struct *vms_to_clean = mm_to_clean->head;
	while(vms_to_clean->next != mm_to_clean->head){
		count++;
		vms_to_clean = vms_to_clean->next;
		//kprintf("[Kernel]: VMA %d\n", count);
	}	
	while(index <= count){
		index++;
		vm_area_struct *temp = vms_to_clean->next;
		kprintf("[Kernel] Removing VMA %d %p\n", index+1, vms_to_clean);
		pages_to_free++;
		remove_page((uint64_t)vms_to_clean);
		vms_to_clean = temp;
	}

	
	remove_page((uint64_t)zombie_process->mm);
	pages_to_free++;
	remove_page((uint64_t)zombie_process->regs.rsp);
	pages_to_free++;
	remove_page((uint64_t)zombie_process->regs.user_rsp);
	pages_to_free++;
	//uint64_t freak_page = (uint64_t)get_free_page();
	//kprintf("freak page is %p \n", freak_page);
	//changeCR3((PML4E *)kernel_pml4e, (PDPE *)kernel_pdpe, (PDE *)kernel_pde, (PTE *)kernel_pte, 0);

	// look for that zombie in the zombie queue and remove it
	tasklist *zom = zombieProcessList;
	tasklist *back_zombie = zombieProcessList;

	if(zom->entry == zombie_process){
		zombieProcessList = zom->next;
		remove_page((uint64_t)zombie_process);
		remove_page((uint64_t)zom);
		pages_to_free+=2;
	}
	else {
		while(zom->entry != zombie_process){
			back_zombie = zom;
			zom = zom->next;
		}

		back_zombie->next = zom->next;
		remove_page((uint64_t)zombie_process);
		remove_page((uint64_t)zom);
		pages_to_free+=2;
	}

	kprintf("[Kernel] No of pages freed for %p: (%d)\n", zombie_process, pages_to_free);

}

int fork() {
	task *child = createChildTask();

	if(runningTask->child == NULL){
		runningTask->child = child;
	}

	int child_pid = child->pid;
	// add the new child to the task queue;
	return child_pid;
}

int exec(char *filename, char** arguments){
	
	
	kprintf("%s, %s, %s", arguments[0], arguments[1], arguments[2]);
	Elf64_Ehdr *the_elf = findElfByName(filename);
 	runningTask->arguments= arguments;	
	load_arguments(runningTask->regs.user_rsp);
	load_binary(the_elf, 3);
	
	//char args[1024];
	//int len = strlen(arguments[1]);
	//			for(int i=0; i<len; i++){
	//				args[i] = arguments[1][i];
	//			}	
	
//	kprintf( "%s \n" ,args);
//	char *something = arguments;
//	kprintf("something %s \n", something);
	/*__asm__ __volatile__
	(
	"movq %0,  %%rsi;"
	:
	:"m"(something)
	:		
	);*/
	
	switch_to_ring_3(runningTask->regs.rip);
	return 0;
}

void waiting_on_pid(int child_pid){
	
	runningTask->status = SLEEPING_PROCESS_STATUS;
	/*
	task *child = runningTask->next;
	task *prev = runningTask->prev;
	task *nextTask = child->next;

	prev->next = child;
	child->prev = prev;
	child->next = runningTask;
	runningTask->prev = child;
	runningTask->next = nextTask;
	nextTask->prev = runningTask;
	*/
	temp_yield(0);
}

task* createChildTask(){
	// create new child task
	// deep copy the page tables with parents page tables
	// create a kernel stack
	// the kernel stack will contain the contents of parent kernel stack[contains the stuff we push before jumping to ring 3]
	// the copied stack also contains the rsp of the user
	// question is, do i need to copy the contents of the user stack as well
	pid++;
	task *childTask = (task *)get_free_page();
	childTask->status = RUNNING_PROCESS_STATUS;
	childTask->regs.rip = NULL; 
	childTask->regs.cr3 = 0;
	childTask->regs.rsp = (uint64_t)get_free_page() + 4096;  // create stack at the top of the page, so that it can grow downwards and not go to the previous page
	childTask->regs.user_rsp = (uint64_t)get_free_user_page() + 4096;	
	childTask->pid = pid;
	childTask->isChild = 1;
	childTask->parent = runningTask;
	childTask->child = NULL;

        uint64_t *pointer_to_pml4e = (uint64_t *)((uint64_t)get_free_page() + 0x2000);
        uint64_t *pointer_to_pdpe = (uint64_t *)((uint64_t)get_free_page());
        uint64_t *pointer_to_pde = (uint64_t *)((uint64_t)get_free_page());
	uint64_t a = (uint64_t)get_free_page();
	uint64_t b = (uint64_t)get_free_page();
        uint64_t *pointer_to_pte = (uint64_t *)get_free_page();

        childTask->pml4e = (PML4E *)pointer_to_pml4e;
        memset(childTask->pml4e, 0, 4096);
        childTask->pdpe = (PDPE *)pointer_to_pdpe;
        memset(childTask->pdpe, 0, 4096);
        childTask->pde = (PDE *)pointer_to_pde;
        memset(childTask->pde, 0, 4096);
        childTask->pte = (PTE *)pointer_to_pte;
        memset(childTask->pte, 0, 4096);
        childTask->pml4e[511] = pml4e[511];
        childTask->regs.cr3 = (uint64_t)childTask->pml4e;
	
	
	a = (uint64_t)get_free_page();
	b = (uint64_t)get_free_page();
	childTask->mm = get_mm_struct();
	//vm_area_struct *new_stack_vma = create_new_vma(me->regs.user_rsp, me->regs.user_rsp + 4096, 8192, VMA_STACK_TYPE);
	//init_insert_vma(me->mm, new_stack_vma);


	deepCopyPageTable((uint64_t)childTask);
	deepCopy_mm_structs((uint64_t)childTask);
	
	//Copy kernel stack of runningTask  on child and move rsp to align with parent's current rsp

	uint64_t start_address = (parent_rsp >>12)<<12;  //aligning purpose
	memcpy (childTask->regs.rsp - 4096 ,start_address  , 4096);
	
	uint64_t offset =(start_address+4096 - parent_rsp);  
	childTask->regs.rsp = childTask->regs.rsp- offset;


	uint64_t *new_rip = *(uint64_t *)parent_rsp;
	childTask->regs.rip = (uint64_t)new_rip;

	// Copy the complete user stack
	uint64_t user_rsp_start_addr = *((uint64_t *)(parent_rsp + 24));
	int size_of_user_stack = runningTask->regs.user_rsp - user_rsp_start_addr;

	childTask->regs.user_rsp -= size_of_user_stack;
	//memcpy(childTask->regs.user_rsp, user_rsp_start_addr, size_of_user_stack);

	task *nextTask = runningTask->next;
	childTask->next = nextTask;
	childTask->prev = runningTask;
	runningTask->next = childTask;
	nextTask->prev = childTask;

	__asm__ __volatile__(
        "movq %%rsp, %%r11;"
        "movq %1, %%rsi;"
        "movq %2, %%rdi;"
        "movq %0 ,%%rsp;"
        "movq $0 ,%%rax;"	
        "push %%rax;"
        "movq $2 ,%%rbx;"	
        "push %%rbx;"
        "movq $3 ,%%rcx;"	
        "push %%rcx;"
        "movq $4 , %%rdx;" 	
        "push %%rdx;"
        "push %%rdi;"
        "pushf;"
        "push %%rsi;"
        "movq %%r11, %%rsp;"
        :
        :"m"(childTask->regs.rsp), "m"(childTask->next), "m"(childTask)
        :"memory"
        );
        childTask->regs.rsp -= 56;
	//return childTask->pid;
	return childTask;
	
}

void addVMAtoTask(task* me ,struct vm_area_struct* new_vma )
{
	/*
	struct vm_area_struct* vm_area_temp1;
	struct vm_area_struct*  vm_area_temp;
	if(me->mm ==NULL)
	{
		me-> mm->head= new_vma;
		new_vma->prev= NULL;
		new_vma->next =NULL;
	} 
	else
	{
	//Scenario 1:Nothing is present in the memory needed by the  VMA
		vm_area_temp1 = ( struct vm_area_struct*)me->mm ->head;
		if(vm_area_temp1->vm_start < new_vma->vm_end)
		{	
			me->mm->head= new_vma;
			new_vma->next = vm_area_temp1;
			new_vma->prev= NULL;
			return ;
		} 
		while(vm_area_temp1->vm_start <new_vma->vm_start)
		 {    
			vm_area_temp1= vm_area_temp1->next ;	 
		 }	
		vm_area_temp = vm_area_temp1-> next;
		vm_area_temp1->next= new_vma;
		new_vma->prev= vm_area_temp1;
		if(vm_area_temp->vm_start >new_vma->vm_end && vm_area_temp1->vm_end< new_vma->vm_end)
		{
		//Scenario 1:Insert in middle
		new_vma->next = vm_area_temp;
		vm_area_temp->prev= new_vma;
		}
		else if(vm_area_temp->vm_start >new_vma->vm_end &&vm_area_temp1->vm_end> new_vma->vm_start) 
		{
		//Scenario 2: Something is present			
		//Check permissions  if same
			if(vm_area_temp1->vm_page_prot == new_vma->vm_page_prot)
			{
			//Case 3: Replace pages of previous vma
			 vm_area_temp1->vm_end = ( vm_area_temp1->vm_end > new_vma->vm_end ) ?vm_area_temp1->vm_end: new_vma->vm_end;
			}	
			else
			{
			//Case 2:Insert at end by truncating  and assume  that pages of not same permissions	
				vm_area_temp1->vm_end = new_vma->vm_start;
				vm_area_temp1->next = new_vma;
				new_vma->prev= vm_area_temp1;
				new_vma->next= vm_area_temp;
				vm_area_temp-> prev= new_vma;
			}
		}
	}
	*/
}

void createTask(
		void (*main)(),  // function pointer
		uint64_t rflags, 
		uint64_t page_dir
		){
	pid++;
	task *me = (task *)get_free_page();
	me->status = RUNNING_PROCESS_STATUS;
	me->regs.rip = (uint64_t)main;
	me->regs.cr3 = page_dir;
	me->regs.rsp = (uint64_t)get_free_page() + 4096;  // create stack at the top of the page, so that it can grow downwards and not go to the previous page
	me->regs.user_rsp = (uint64_t)get_free_user_page() + 4096;


	uint64_t a = (uint64_t)get_free_page();
	uint64_t b = (uint64_t)get_free_page();
	me->mm = get_mm_struct();
	vm_area_struct *new_stack_vma = create_new_vma(me->regs.user_rsp-4096, me->regs.user_rsp, 8192, VMA_STACK_TYPE);
	init_insert_vma(me->mm, new_stack_vma);
	kprintf("%p %p %p %p %pnew vma\n", new_stack_vma->vm_start, new_stack_vma->vm_end, new_stack_vma, runningTask, runningTask->mm);

	me->pid = pid;
	me->isChild = 0;
       uint64_t *pointer_to_pml4e = (uint64_t *)get_free_page();
       uint64_t *pointer_to_pdpe = (uint64_t *)get_free_page();
       uint64_t *pointer_to_pde = (uint64_t *)get_free_page();
       uint64_t *pointer_to_pte = (uint64_t *)get_free_page();

	me->pml4e = (PML4E *)pointer_to_pml4e;
	memset(me->pml4e, 0, 4096);
	me->pdpe = (PDPE *)pointer_to_pdpe;
	memset(me->pdpe, 0, 4096);
	me->pde = (PDE *)pointer_to_pde;
	memset(me->pde, 0, 4096);
	me->pte = (PTE *)pointer_to_pte;
	memset(me->pte, 0, 4096);

	me->pml4e[511] = pml4e[511];
	me->regs.cr3 = (uint64_t)me->pml4e;

	if(runningTask == NULL){
		runningTask = me;
		lastTask = me;
	}

	me->next = runningTask;
	runningTask->prev = me;
	runningTask = me;
	lastTask->next = runningTask;
	runningTask->prev = lastTask;

	__asm__ __volatile__(
	"movq %%rsp, %%r11;"
	"movq %1, %%rsi;"
	"movq %2, %%rdi;"
	"movq %0 ,%%rsp;"
	"movq $1 ,%%rax;"	
	"push %%rax;"
	"movq $2 ,%%rbx;"	
	"push %%rbx;"
	"movq $3 ,%%rcx;"	
	"push %%rcx;"
	"movq $4 , %%rdx;" 	
	"push %%rdx;"
	"push %%rdi;"
	"pushf;"
	"push %%rsi;"
	"movq %%r11, %%rsp;"
        :
	:"m"(me->regs.rsp), "m"(me->next), "m"(me)
	:"memory"
	);
	me->regs.rsp -= 56;

	//return me;
}

void test_user_function()
{
	

}

void load_arguments(uint64_t user_rsp)
{
int argc =0;
while(runningTask->arguments[argc]!= NULL && strcmp(runningTask->arguments[argc], "")!=0){
    argc++;
}
kprintf("Argc: %d", argc);
kprintf("args: %s", runningTask->arguments);
__asm__  __volatile__
(
"movq %%rsp ,%%r11;"
"movq %0 ,%%r8;" 
"movq %%r8 , %%rsp;"
"movq %1 , %%rdi;"
"movq %%rdi ,(%%rsp);"
"movq %%r11, %%rsp;"
:
: "m"(user_rsp), "m"(argc)
);

if(argc==0)
{
return;
}
for( uint64_t i=0; i<argc; i++)
{
memcpy( user_rsp-i*64 ,runningTask->arguments[i] ,64);
kprintf("\n%s", runningTask->arguments[i]);
}
if(argc >0)
{
user_rsp= user_rsp-argc*64;
runningTask->regs.user_rsp= user_rsp;
}
}

/*
__asm__ __volatile__(
"movq %0 , %%rsi;"
"movq %1,%%rdi;"
:
:"m"(runningTask->arguments), "m"(argc)
:
);

__asm__ __volatile__(




"subq $0x28, %%rsp;"
"movq %%rdi,(%%rsp);"
"movq %%rsi ,8(%%rsp);"

:
: "m"(argc) , "m"(user_rsp)
:"memory"
);
<<<<<<< HEAD
*/

void switch_to_ring_3()
{

	if(runningTask->child && runningTask->child->status == ZOMBIE_PROCESS_STATUS){
		kprintf("[Kernel]: removing its child junk %p\n", runningTask->child);
		free((uint64_t)runningTask->child);
	}
//	load_arguments(runningTask->regs.user_rsp);




	changeCR3(runningTask->pml4e, runningTask->pdpe, runningTask->pde, runningTask->pte, 0);
        //kprintf("new cr3 %p and pointing to %p\n", runningTask->pml4e, runningTask->regs.rip);	
	__asm__ __volatile__ (
        	"movq %0, %%r11;"
        	"movq %%r11, %%rsp;"
            :
            : "m"(runningTask->regs.rsp)
            :
        );

	uint64_t fn_to_execute =  runningTask->regs.rip;
	//uint64_t* user_rsp= (uint64_t*)get_free_user_page();
	//user_rsp += 0x1000;
	uint64_t user_rsp = runningTask->regs.user_rsp;
	uint64_t current_rsp;
	__asm__ __volatile__("movq %%rsp, %0;"
	:"=m" (current_rsp)
	:
	:
	);
	//kernel_rsp=current_rsp; 
	//u_rsp=(uint64_t) user_rsp;				
	set_tss_rsp((void*)current_rsp);
	runningTask->regs.rsp= current_rsp;
	uint32_t current_rsp_lo = current_rsp & 0x00000000FFFFFFFF;
	uint32_t current_rsp_hi =(current_rsp & 0xFFFFFFFF00000000)>>32;
	cpuSetMSR(0xC0000102,current_rsp_lo, current_rsp_hi);
	//kprintf("we are all set 3, 2, 1 ... Launch !\n");
	__asm__ __volatile__ (
	"movq %0, %%r13;"
	"movq $0x23 , %%rax;"
	"movq %%rax , %%ds;"
	"movq %%rax , %%es;"
	"movq %%rax , %%fs;"
	"movq %%rax , %%gs;"
	"movq %1, %%rax;"  // change to user rsp 

        "push $0x23;"  // data segment is at offset 32.. last two bits should be 2. 32 or 3
	"push %%rax;"
	"pushf;"
	"push $0x2B;" // data segment is at offset 40... last two bits should be 2. 40 or 3
	"push %%r13;"
	:
	:"m"(fn_to_execute), "m"(user_rsp)
	:
	);
	
	if(runningTask->isChild == 1){
		__asm__ __volatile__(
			"movq $0, %%rax;"
			:
			:
			:
		);
	}
	if(runningTask->status == SLEEPING_PROCESS_STATUS){
		__asm__ __volatile__(
			"movq $8, %%rax;"
			:
			:
			:
		);

	}
	__asm__ __volatile__(
	"iretq; "
	:	
	:
	:
	);
	
}

void removefromOtherLists(task * temp)
{






}

void addtoZombieList(task* temp )
{
	temp->status = ZOMBIE_PROCESS_STATUS;
	tasklist *new_zombie = (tasklist *)get_free_page();
	new_zombie->pid = temp->pid;
	new_zombie->entry = temp;
	new_zombie->next = NULL;
	if(zombieProcessList->entry == NULL){
		kprintf("zombie is empty!\n");
		zombieProcessList = new_zombie;
	} else {
		new_zombie->next = zombieProcessList;
		zombieProcessList = new_zombie;
	}
	kprintf("added zombie %p!\n", new_zombie);
	/*
	*/
/*
*/
}


void addtoRunningList(task* temp)
{
	temp->status =RUNNING_PROCESS_STATUS;
	tasklist *running_process = (tasklist *)get_free_page();	
	running_process->pid = temp->pid;
	running_process->entry = temp;
	running_process->next = NULL;

	if(runningProcessList == NULL){
		runningProcessList = running_process;
	} else {
		running_process->next = runningProcessList;
		runningProcessList = running_process;
	}
	kprintf("added running process %p!\n", running_process);
/*
tasklist* current_running= (tasklist*) runningProcessList;
current_running->pid =temp->pid;
current_running->next =NULL;
current_running->entry =temp;
runningProcessList= current_running;
removefromOtherLists(temp);
*/
}

void addtoWaitList(task* temp)
{
temp->status= SLEEPING_PROCESS_STATUS;
tasklist* current_waiting= (tasklist*) waitProcessList;
while(current_waiting != NULL)
{
current_waiting = current_waiting->next;
}
current_waiting->pid =temp->pid;
current_waiting ->entry=temp;
current_waiting->next =NULL;
removefromOtherLists(temp);
}



void getprocessList()
{
	kprintf("\n PID \t STATUS ");
	tasklist* current_zombie= (tasklist*) zombieProcessList;
	while(current_zombie != NULL)
	{
	kprintf("\n %x \t ZOMBIE ",current_zombie->pid );
	current_zombie = current_zombie->next;
	}
	
	kprintf("\n %x \t RUNNING ", runningTask->pid );
	/*
	tasklist* current_running = (tasklist*)runningProcessList;
	
	while(current_running !=NULL)
	{
        	current_running = current_running->next;
       	}
	*/
	
	/*
	tasklist* current_waiting = (tasklist*)waitProcessList;
	while(current_running !=NULL)
	{
		kprintf("\n %x \t WAITING ",current_waiting->pid );
        	current_waiting = current_waiting->next;
	}
	*/
}



int kill_process(int kpid){

	// when we kill a proecess
	// we check if that process has any children or not 
	// if it does, we access that child's parent and change its parent to MainTask
	// 
	int limit = 43;
	int count = 0;
	task *temp = runningTask;

	while(temp->pid != pid && count < limit){
		temp = temp->next;
		count++;
	}	
	if(count >= limit) {
		return -1;
	}
	else {
		/*
		task *prev_temp = temp;
		while(prev_temp->next != temp){
			prev_temp = prev_temp->next;
		}
		
		prev_temp->next = temp->next;
		*/
		task *prev_task = temp->prev;
		task *next_task = temp->next;
		prev_task->next = next_task;
		next_task->prev = prev_task;
		
		if(temp->child)
			temp->child->parent = lastTask; // changing the child's parent to idleTask
		addtoZombieList(temp);
		// add to zombie queue

	}
	return kpid;
}

void removeTask(){

	// we just add this to zombie queue
	// reclaim all the pages used by this task
	changeCR3((PML4E *)kernel_pml4e, (PDPE *)kernel_pdpe, (PDE *)kernel_pde, (PTE *)kernel_pte, 0);
	//free(runningTask);

	//kprintf("[Kernel PID:%d]: Adding %p to zombie queue\n", runningTask->pid);
	runningTask->prev->next = runningTask->next;
	runningTask->next->prev = runningTask->prev;
	runningTask->status = ZOMBIE_PROCESS_STATUS; 
	addtoZombieList(runningTask);
	
	//kprintf("Changed the task from %d to %d and prev is %d\n", runningTask->pid, next->pid, temp->pid);

	__asm__ __volatile__ (
		"movq %0, %%rdi;"
		"movq %1, %%rsi;"
		:
		:"m"(runningTask), "m"(runningTask->next)
		:
	);

	runningTask = runningTask->next;
	//addtoRunningList(runningTask);
	// switch the rsp's

	__asm__ __volatile__ (
		"movq (%%rsi), %%rsp;"  // change the stack
		:
		:
		:
	);
	getprocessList();
	switch_to_ring_3(runningTask->regs.rip);
	
}

void temp_yield(int exec_next){

	uint64_t *new_rip = *(uint64_t *)parent_rsp;
	runningTask->regs.rip = (uint64_t)new_rip;


	uint64_t user_rsp_start_addr = *((uint64_t *)(parent_rsp + 24));
	//int size_of_user_stack = runningTask->regs.user_rsp - user_rsp_start_addr;

	//childTask->regs.user_rsp -= size_of_user_stack;


	runningTask->regs.user_rsp = user_rsp_start_addr;


	changeCR3((PML4E *)kernel_pml4e, (PDPE *)kernel_pdpe, (PDE *)kernel_pde, (PTE *)kernel_pte, 0);
	__asm__ __volatile__ (
		"movq %0, %%rdi;"
		"movq %1, %%rsi;"
		:
		:"m"(runningTask), "m"(runningTask->next)
		:
	);
	/*
	*/
	runningTask = runningTask->next;
	__asm__ __volatile__ (
		"movq (%%rsi), %%rsp;"  // change the stack
		:
		:
		:
	);
	switch_to_ring_3(runningTask->regs.rip);

}
/*
void remove_zombies_from_queue(task *zombie_process){

	// look for that zombie in the zombie queue and remove it
	tasklist *zom = zombieProcessList;
	tasklist *back_zombie = zombieProcessList;

	if(zom->entry == zombie_process){
		zombieProcessList = zom->next;
		remove_page((uint64_t)zombie_process);
		remove_page((uint64_t)zom);
		pages_to_free+=2;
	}
	else {
		while(zom->entry != zombie_process){
			back_zombie = zom;
			zom = zom->next;
		}

		back_zombie->next = zom->next;
		remove_page((uint64_t)zombie_process);
		remove_page((uint64_t)zom);
		pages_to_free+=2;
	}
}
*/
void clean_zombies(){
	//kprintf("[Kernel]: Removing all the zombie processes\n");
	// traverse zombie list
	// usually called by the main task
	
	tasklist *current_zombie = zombieProcessList;
	tasklist *ahead_zombie = current_zombie;

	while(current_zombie != NULL){
		if(current_zombie->entry->parent == NULL){
			// clean it up
			ahead_zombie = current_zombie->next;
			free(current_zombie->entry);
			current_zombie = ahead_zombie;
		}
		else if(current_zombie->entry->parent == runningTask){
			// say parent has died
			// before dying, parent assigns responsibility to 1
			ahead_zombie = current_zombie->next;
			free(current_zombie->entry);
			current_zombie = ahead_zombie;

		} else if(current_zombie->entry->parent != runningTask && current_zombie->entry->parent->status == ZOMBIE_PROCESS_STATUS){
			//clean it up
			ahead_zombie = current_zombie->next;
			free(current_zombie->entry);
			current_zombie = ahead_zombie;
		} else {
			current_zombie = current_zombie->next;
		}
	}

}
