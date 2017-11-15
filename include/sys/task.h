#ifndef _TASK_H
#define _TASK_H
#include<sys/defs.h>

void initTasking();
typedef struct {
	uint64_t rsp, rbp, rip, cr3;
} registers;

typedef struct task{
	registers regs;
	struct task* next;
} task;

task* runningTask;
task* mainTask;
task* otherTask;
task* idleTask;

void createTask(task*, void(*)(), uint64_t, uint64_t, task*);
void yield();
void switchTo(registers *old, registers *current);
void switch_to_ring_3();
void mainOne();
void mainTwo();
void beIdle();
#endif
