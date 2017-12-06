#include <sys/task.h>
#include <sys/kprintf.h>
#include <sys/page.h>
#include <sys/memory.h>
#include <sys/page_table.h>
#include <sys/gdt.h>
#include <sys/syscall.h>
#include <sys/tarfs.h>
#include <sys/elf64.h>

void yield(){

	/*
		1. Populate rdi, rsi with me and next respectively.
		2. get the rip and store it in runningTask ->regs.rip
r
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


void createTask(
		void (*main)(),  // function pointer
		uint64_t rflags, 
		uint64_t page_dir
		){
	task *me = (task *)get_free_page();
	me->status = RUNNING_PROCESS_STATUS;
	me->regs.rip = (uint64_t)main;
	me->regs.cr3 = page_dir;
	me->regs.rsp = (uint64_t)get_free_page() + 4096;  // create stack at the top of the page, so that it can grow downwards and not go to the previous page
	me->regs.user_rsp = (uint64_t)get_free_user_page() + 4096;	
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

	if(runningTask == NULL){
		runningTask = me;
		lastTask = me;
	}
	me->next = runningTask;
	runningTask = me;
	lastTask->next = runningTask;

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
	
	__asm__ __volatile__(

	"syscall;"
	);
	while(1);

}


void switch_to_ring_3()
{
	changeCR3(runningTask->pml4e, runningTask->pdpe, runningTask->pde, runningTask->pte, 0);
	__asm__ __volatile__ (
        	"movq %0, %%r11;"
        	"movq %%r11, %%rsp;"
            :
            : "m"(runningTask->regs.rsp)
            :
        );
        kprintf("new cr3 %p and pointing to %p\n", runningTask->pml4e, runningTask->regs.rip);
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
	uint32_t current_rsp_lo = current_rsp & 0x00000000FFFFFFFF;
	uint32_t current_rsp_hi =(current_rsp & 0xFFFFFFFF00000000)>>32;
	cpuSetMSR(0xC0000102,current_rsp_lo, current_rsp_hi);
	kprintf("we are all set 3, 2, 1 ... Launch !\n");
	__asm__ __volatile__ (
	"movq %0, %%r13;"
	"movq $0x23 , %%rax;"
	"movq %%rax , %%ds;"
	"movq %%rax , %%es;"
	"movq %%rax , %%fs;"
	"movq %%rax , %%gs;"
	"movq %1, %%rax;"

        "push $0x23;"  // data segment is at offset 32.. last two bits should be 2. 32 or 3
	"push %%rax;"
	"pushf;"
	"push $0x2B;" // data segment is at offset 40... last two bits should be 2. 40 or 3
	"push %%r13;"
	
	"iretq;"
	:
	:"m"(fn_to_execute), "m"(user_rsp)
	:
	);
}

void removeTask(){
	// reclaim all the pages used by this task

	// removing the task from the runningTask list
	task* temp = runningTask;
	task* next = runningTask->next;

	while(temp->next != runningTask){
		temp = temp->next;
	}
	temp->next = next;
	runningTask = next;
	kprintf("Changed the task");
	yield();
}
