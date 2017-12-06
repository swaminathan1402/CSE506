#ifndef _TASK_H
#define _TASK_H
#include<sys/defs.h>
#include<sys/elf64.h>
#include<sys/page_table.h>
#define RUNNING_PROCESS_STATUS 1
#define SLEEPING_PROCESS_STATUS 2
#define EXIT_PROCESS_STATUS 3
#define ZOMBIE_PROCESS_STATUS 4
void initTasking();
typedef struct {
	uint64_t rsp, rbp, rip, cr3, user_rsp;
} registers;

typedef struct task{
	registers regs;
	struct task* next;
	PML4E* pml4e;
	PDPE* pdpe;
	PDE* pde;
	PTE* pte;
	int status;
	
} task;

task* runningTask;
task* lastTask;
//task* mainTask;
//task* otherTask;
//task* idleTask;
//task* userTask;

void createTask(void(*)(), uint64_t, uint64_t);
void yield();
void switchTo(registers *old, registers *current);
void createIdleTask();
void switch_to_ring_3();
void mainOne();
void mainTwo();
void beIdle();
void removeTask();
#endif
