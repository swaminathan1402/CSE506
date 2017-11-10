#include <sys/task.h>
#include <sys/kprintf.h>
#include <sys/page.h>

void yield(){
	// we have running task and next task
	//switchTo(runningTask, next);

	// updating running task to next
	__asm__ __volatile__(
		"movq %1, %%r8;"
		"movq %%r8 , %0;"
		:"=m"(runningTask)
		:"m"(runningTask->regs.rsp)
		:
	);
	runningTask->regs.rsp -= 56;  // growing the stack

	kprintf("%p new rip: %p\n", runningTask->regs.rip);
	__asm__ __volatile__(
                "push %%rax;"
                "push %%rbx;"
                "push %%rcx;"
                "push %%rdx;"
                "push %%rdi;"
                "pushf;"
                "push %%rsi;"
		"movq %0, %%rsp;"
		"pop %%rsi;"
		"popf;"
		"pop %%rdi;"
		"pop %%rdx;"
		"pop %%rcx;"
		"pop %%rbx;"
		"pop %%rax;"
		:
		: "m"(runningTask->regs.rsp)
		:"r11"
	);
	runningTask->regs.rsp += 56;
	__asm__ __volatile__ (
		"call %0;"
		:
		: "m"(runningTask->regs.rip)
		:
	);

}

void mainTwo() {
	kprintf("@MainTwo ---- 1\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}
	__asm__ __volatile__ (
		"lea 14(%%rip), %%r11;"
		"movq %%r11, %0;"
		: "=m"(otherTask->regs.rip)
		:
		: 
	);
	yield();
	kprintf("@MainTwo ---- 2\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}
	__asm__ __volatile__ (
		"lea 14(%%rip), %%r11;"
		"movq %%r11, %0;"
		: "=m"(otherTask->regs.rip)
		:
		:
	);
	yield();
}

void mainOne() {
	kprintf("@MainOne ---- 1\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}
	__asm__ __volatile__ (
		"lea 14(%%rip), %%r11;"
		"movq %%r11, %0;"
		: "=m"(mainTask->regs.rip)
		:
		:
	);
	yield();
	kprintf("@MainOne ---- 2\n");
	for(int i=0; i<1000; i++) {
	    for(int j=0; j<1000; j++) {
	    }
	}
	__asm__ __volatile__ (
		"lea 14(%%rip), %%r11;"
		"movq %%r11, %0;"
		: "=m"(mainTask->regs.rip)
		:
		:
	);
	yield();
}
/*
void initTasking(){
	
	__asm__ __volatile__ (
		"movq %%cr3, %%rax;"
		"movq %%rax, %0;"
		: "=m"(mainTask->regs.cr3)
		:
	);
	kprintf("The value of cr3: %p\n", mainTask->regs.cr3);
	__asm__ __volatile__ (
		"pushf;"
		"pop %0;"
		: "=m"(mainTask->regs.rflags)
		:
	);
	kprintf("The value of rflags: %p\n", mainTask->regs.rflags);
	createTask(otherTask, otherMain, mainTask->regs.rflags, mainTask->regs.cr3);
	mainTask->next = otherTask;
	otherTask->next = mainTask;
	runningTask = mainTask;
}
*/
void createTask(task *me, 
		void (*main)(), 
		uint64_t rflags, 
		uint64_t page_dir, 
		task *next ){
	/*
	task->regs.rax = 0;
	task->regs.rbx = 0;
	task->regs.rcx = 0;
	task->regs.rdx = 0;
	task->regs.rsi = 0;
	task->regs.rdi = 0;
	task->regs.rflags = rflags;
	*/
	me->regs.rip = (uint64_t)main;
	me->regs.cr3 = page_dir;
	me->regs.rsp = (uint64_t)get_free_page() + 4096;  // create stack at the top of the page, so that it can grow downwards and not go to the previous page

	__asm__ __volatile__(
	"movq %%rsp, %%r11;"
	"movq %1, %%rsi;"
	"movq %2, %%rdi;"
	"movq %0 ,%%rsp;"
	"movq $0 ,%%rax;"	
	"push %%rax;"
	"movq $0 ,%%rbx;"	
	"push %%rbx;"
	"movq $0 ,%%rcx;"	
	"push %%rcx;"
	"movq $0 , %%rdx;" 	
	"push %%rdx;"
	"push %%rdi;"
	"pushf;"
	"push %%rsi;"
	"movq %%r11, %%rsp;"
        :
	:"m"(me->regs.rsp), "m"(next), "m"(me)
	:"memory"
	);
	me->regs.rsp -= 56;
}
