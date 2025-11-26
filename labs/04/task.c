#include <stddef.h>
#include <stdbool.h>
#include "task.h"
#include "test.h"

static int pid=0;
task_t task_pool[XNOF_PROCESS];
queueElement_t taskElementPool[XNOF_PROCESS];
char kstack_pool[XNOF_PROCESS][4096];
runQueue_t runq;

extern void switch_to(task_t *prev, task_t *next);

int get_new_pid()
{
    return pid++;
}

void taskQueue_init()
{
	runq.head = NULL;
	runq.tail = NULL;
    runq.count = 0;
}

bool isQueueEmpty(runQueue_t *pRunq)
{
    if(0 == pRunq->count){
        return true;
    }else{
        return false;
    }
}

bool isQueueFull(runQueue_t *pRunq)
{
    if(XNOF_TASK_RUNQUEUE == pRunq->count){
        return true;
    }else{
        return false;
    }
}

bool taskQueue_push(runQueue_t *pRunq, queueElement_t *pTaskElement)
{
	if(isQueueFull(pRunq)){
        return false;
	}else{
        //set queue element ptr to task pool entry
        if(NULL != pRunq->head)
            pRunq->head->next = pTaskElement;
        pRunq->head = pTaskElement;
		pRunq->head->task = pTaskElement->task;
        pRunq->count++;
        
        if(NULL == pRunq->tail)
            pRunq->tail = pTaskElement;
        return true;
	}
}

queueElement_t* taskQueue_pop(runQueue_t *pRunq)
{
    task_t *task = NULL;
	queueElement_t *pTaskElement = NULL;
    
    if(isQueueEmpty(pRunq)){
        //return NULL;
        
        /* Temprary for Re-create Idle task */
        privilege_task_create(&Idle_task);
        pTaskElement = pRunq->tail;
        pRunq->tail = pRunq->tail->next;
        pRunq->count--;
        return pTaskElement;
	}else{
        //task = pRunq->tail->task;
        pTaskElement = pRunq->tail;
        pRunq->tail = pRunq->tail->next;
        pRunq->count--;
        
        /* re-enqueue to runq tail */
        //int new_id = get_new_pid();
        //taskElementPool[new_id].task = pTaskElement->task;
        //taskQueue_push(&runq, &taskElementPool[new_id]);
        //new_id++;
        
        return pTaskElement;
    }
}

void task_init()
{
    taskQueue_init();
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
    
    taskElementPool[new_id].task = pTask;
    taskQueue_push(&runq, &taskElementPool[new_id]);
    
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

void schedule(){
    
    // 1. Pick Next Task
    queueElement_t* next = taskQueue_pop(&runq);
    
    // 2. Switch 
    context_switch(next->task);
    
}
