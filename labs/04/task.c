#include <stddef.h>
#include <stdbool.h>
#include "task.h"
#include "test.h"

int pid=0;
bool flag_reschedule=false;
task_t task_pool[XNOF_PROCESS];
queueElement_t taskElementPool[XNOF_PROCESS];
uint64_t kstack_pool[XNOF_PROCESS][4096];
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
    
    privilege_task_create(&Idle_task, eTASK_PRI_IDLE);
    runq.idle = &taskElementPool[IDLE_TASK_ID];
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
	if(isQueueFull(pRunq) || NULL == pTaskElement){
        return false;
	}
	
	if(NULL == pRunq->head){
		pRunq->head = pTaskElement;
		pRunq->tail = pTaskElement;
	}else{
		pRunq->tail->next = pTaskElement;
		pRunq->tail = pTaskElement;
	}
	
	pRunq->count++;
	return true;
}

queueElement_t* taskQueue_pop(runQueue_t *pRunq)
{
	queueElement_t *pTaskElement = NULL;
    
    if(isQueueEmpty(pRunq)){
        return pRunq->idle;
	}else{
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

queueElement_t* taskQueue_pick_highest_ready(runQueue_t *pRunq)
{
	queueElement_t *curr = pRunq->head;
	queueElement_t *bestTask = NULL;
	int max_priority = -1;

    while (curr != NULL) {
        if (curr->task != NULL && curr->task->state == eTASK_ST_READY) {
            if (curr->task->dynamic_priority > max_priority) {
                max_priority = curr->task->dynamic_priority;
                bestTask = curr;
            }
        }
        curr = curr->next;
    }

    return bestTask ? bestTask : pRunq->idle;
}

void task_init()
{
    taskQueue_init();
}

int privilege_task_create(void(*func)(), int priority)
{
    task_t *pTask=NULL;
    // allocate task struct and kernel stack
    int new_id = get_new_pid();
    
    pTask=&task_pool[new_id];
    pTask->id = new_id;
    
    //sp 
    pTask->sp = &kstack_pool[new_id][4096];
    pTask->lr = func;
    pTask->func = func;
    
    pTask->state = eTASK_ST_READY;
    pTask->base_priority = priority;
	pTask->dynamic_priority = priority;
    pTask->ticks = 0;
    
    taskElementPool[new_id].task = pTask;
    
    if(IDLE_TASK_ID != new_id)
        taskQueue_push(&runq, &taskElementPool[new_id]);
    
    return new_id;
}

task_t* get_current(){
    uint64_t addr_task;
    asm volatile("mrs %0, tpidr_el1" : "=r"(addr_task));
    return (task_t *)(addr_task);
}

void context_switch(struct task* next){
    task_t* prev = get_current();
    switch_to(prev, next);
	
	// 當前任務被喚醒回到這裡時，重新開中斷
    asm volatile("msr daifclr, #2");
	
    //next->func();
	next->wait_ticks = 0;
}

void schedule(){
    //my_printf("schedule Enter---\n");
	task_t* cur = get_current();
	
    // 1. Pick Next Task
    queueElement_t* next = taskQueue_pick_highest_ready(&runq);
    
	// 2. handle orig task
	cur->state = eTASK_ST_READY;
	
	//my_printf("Task_ID: curr=%d, next=%d\n", cur->id, next->task->id);
	my_printf("Task(id,pri): curr=(%d,%d), next=(%d,%d)\n"
		, cur->id, cur->dynamic_priority
		, next->task->id, next->task->dynamic_priority);
	
    // 3. Switch 
	//next->task->ticks = next->task->base_priority;
	next->task->ticks = next->task->dynamic_priority;
	next->task->wait_ticks = 0;
	next->task->state = eTASK_ST_RUNNING;
    context_switch(next->task);
}

void do_exec(void(*func)())
{
	// SP_EL0：使用者模式堆疊指標的位址
	// ELR_EL1：使用者模式過程的程式計數器
	// SPSR_EL1：CPU 使用者模式狀態
	
	
	
}


