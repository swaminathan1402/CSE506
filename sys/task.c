#include <sys/task.h>
#include <sys/kprintf.h>
#include <sys/page.h>
void yield(){
	// we have running task and next task
	//switchTo(runningTask, next);

	task *next = runningTask->next;
	runningTask = next;
	__asm__ __volatile__(
		"push %%rax;"
		"push %%rbx;"
		"push %%rcx;"
		"push %%rdx;"
		"push %%rbp;"
		"push %%rsi;"
		"push %%rdi;"
		"movq %1, %%rsp;"
		"pop %%rdi;"
		"pop %%rsi;"
		"pop %%rbp;"
		"pop %%rdx;"
		"pop %%rcx;"
		"pop %%rbx;"
		"pop %%rax;"
		"call %0;"
		:
		: "m"(next->regs.rip) ,"m"(next->regs.rsp)
		:
	);

}

void mainTwo() {
	kprintf("@MainTwo\n");
	yield();
}

void mainOne() {
	kprintf("@MainOne\n");
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
void createTask(task *task, void (*main)(), uint64_t rflags, uint64_t page_dir){
	task->regs.rax = 0;
	task->regs.rbx = 0;
	task->regs.rcx = 0;
	task->regs.rdx = 0;
	task->regs.rsi = 0;
	task->regs.rdi = 0;
	task->regs.rflags = rflags;
	task->regs.rip = (uint64_t)main;
	task->regs.cr3 = page_dir;
	task->regs.rsp = (uint64_t)get_free_page();  // create new stack 
	__asm__volatile(
	"mov %%rsp ,%0  ;"
	"mov %1 , %%rsp;"
	"mov %2 ,%%rax;"	
	"push %%rax;"
	"mov %3 ,%%rbx;"	
	"push %%rbx;"
	"mov %4 ,%%rcx;"	
	"push %%rcx;"
	"mov %5 , %%rdx;" 	
	"push %%rdx;"
	"mov"
        "push %%rbp;"
	"push %%rsi;"
	"push %%rdi;"
	
	"mov %0 ,%%rsp"
        :
	:
	:

	);	

}
