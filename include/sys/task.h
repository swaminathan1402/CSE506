#ifndef _TASK_H
#define _TASK_H
#include<sys/defs.h>
#include<sys/elf64.h>
#include<sys/page_table.h>
#include<sys/mm_struct.h>
#define RUNNING_PROCESS_STATUS 1
#define SLEEPING_PROCESS_STATUS 2
#define ZOMBIE_PROCESS_STATUS 3
#define READY_PROCESS_STATUS 4
#define WAITING_PROCESS_STATUS 5
#define KILLED_PROCESS_STATUS 6

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
        mm_struct *mm ;
	int isChild;
	struct task *parent;  //for pid inheritance
	struct task *child;
	char **arguments;
	char *command;	
} task;

typedef struct tasklist{
int pid ;
task* entry;
struct tasklist* next; 
char* command;
int status;
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
tasklist *readyProcessList;


void createTask(void(*)(), uint64_t, uint64_t, Elf64_Ehdr *);
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
void temp_yield(int);
void waiting_on_pid(int);
void addtozombieList(task*);
void addtowaitList( task*);
void addtorunningList( task*);
void removefromOtherList(task*);
void removeFromRunningList(task *);
void addtoReadyList(task *);
void clean_zombies();
void free(task *);
void reap_zombie_process(task *);
#endif
