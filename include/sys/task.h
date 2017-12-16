#ifndef _TASK_H
#define _TASK_H
#include<sys/defs.h>
#include<sys/elf64.h>
#include<sys/page_table.h>
#include<sys/mm_struct.h>
#define RUNNING_PROCESS_STATUS 1
#define SLEEPING_PROCESS_STATUS 2
#define ZOMBIE_PROCESS_STATUS 3
#define ORPHAN_PROCESS_STATUS 4
void initTasking();
typedef struct {
	uint64_t rsp, rbp, rip, cr3, user_rsp;
} registers;


typedef struct task{	
	registers regs;
	struct task* next;
	struct task* prev;
	PML4E* pml4e;
	PDPE* pdpe;
	PDE* pde;
	PTE* pte;
	int status;
	int pid;
        struct mm_struct *mm ;
	int isChild;
	struct task *parent;  //for pid inheritance
	struct task *child;	
	char arguments[256];
} task;

typedef struct tasklist{
int pid ;
task* entry;
struct tasklist* next; 
char* command;
}tasklist;


task* runningTask;
task* lastTask;
uint64_t parent_rsp;
int pid;
//task* mainTask;
//task* otherTask;
//task* idleTask;
//task* userTask;

tasklist* runningProcessList;
tasklist*  zombieProcessList;
tasklist* waitProcessList;


void createTask(void(*)(), uint64_t, uint64_t);
void yield();
void switchTo(registers *old, registers *current);
void createIdleTask();
void switch_to_ring_3();
void mainOne();
void mainTwo();
void beIdle();
void removeTask();
int fork();
task* createChildTask();
int exec(char * ,char** );

void getprocessList();

int kill_process(int);
void temp_yield();
void waiting_on_pid(int);
void addtozombieList(task*);
void addtowaitList( task*);
void addtorunningList( task*);
void removefromOtherList(task*);
void addVMAtoTask(task* me ,struct vm_area_struct* new_vma );
void clean_zombies();
void free(task *);
#endif
