#include <stddef.h>
#include "task.h"

static int pid=0;
task_t task_pool[XNOF_PROCESS];
char kstack_pool[XNOF_PROCESS][4096];

extern void switch_to(task_t *prev, task_t *next);

int get_new_pid()
{
    return pid++;
}

void task_init()
{
    
}

int privilege_task_create(void(*func)())
{
    task_t *pTask=NULL;
    // allocate task struct and kernel stack
    int new_id = get_new_pid();
    
    pTask=&task_pool[new_id];
    pTask->id = new_id;
    
    //sp 
    pTask->sp = &kstack_pool[new_id];
    pTask->lr = func;
    pTask->func = func;
    
    pTask->state = eTASK_ST_READY;
    return new_id;
}

static task_t* get_current(){
    uint64_t addr_task;
    asm volatile("mrs %0, tpidr_el1" : "=r"(addr_task));
    return (task_t *)(addr_task);
}

void context_switch(struct task* next){
    struct task* prev = get_current();
    switch_to(prev, next);
    //next->func();
}

