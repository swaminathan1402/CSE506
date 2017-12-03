#ifndef _TASK_H
#define _TASK_H
#include<sys/defs.h>
#include<sys/elf64.h>
#include<sys/page_table.h>

void initTasking();
typedef struct {
	uint64_t rsp, rbp, rip, cr3;
} registers;

typedef struct task{
	registers regs;
	struct task* next;
	PML4E* pml4e;
	PDPE* pdpe;
	PDE* pde;
	PTE* pte;
	
} task;

task* runningTask;
task* lastTask;
//task* mainTask;
//task* otherTask;
//task* idleTask;
//task* userTask;

void createTask(void(*)(), uint64_t, uint64_t, Elf64_Ehdr *);
void yield();
void switchTo(registers *old, registers *current);

void switch_to_ring_3(uint64_t);
void mainOne();
void mainTwo();
void beIdle();

#endif
