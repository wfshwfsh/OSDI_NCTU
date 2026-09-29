#include <stddef.h>
#include <stdbool.h>
#include "task.h"
#include "test.h"

#define STACK_SIZE 4096

int pid=0;
bool flag_reschedule=false;
task_t task_pool[XNOF_PROCESS];
queueElement_t taskElementPool[XNOF_PROCESS];
uint64_t kstack_pool[XNOF_PROCESS][STACK_SIZE];
uint64_t ustack_pool[XNOF_PROCESS][STACK_SIZE];
runQueue_t runq;

extern void switch_to(task_t* prev, task_t* next,
                      uint64_t nextfunc, uint64_t spsr);
extern void user_context(uint64_t sp, uint64_t func);

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
	uint64_t spsr_el1;
    // allocate task struct and kernel stack
    int new_id = get_new_pid();
    
    pTask=&task_pool[new_id];
    pTask->id = new_id;
    
    pTask->sp = (uint64_t)&kstack_pool[new_id][4096];
    pTask->elr = (uint64_t)func;
    asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
    pTask->spsr = spsr_el1;
    
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
	uint64_t next_func = next->elr;
    switch_to(prev, next, next_func, next->spsr);
	
    // 當前任務被喚醒回到這裡時，重新開中斷
    //asm volatile("msr daifclr, #2");
	
    next->wait_ticks = 0;
}

void schedule(){
    //my_printf("schedule Enter---\n");
    task_t* cur = get_current();
	
    // 1. Pick Next Task
    queueElement_t* next = taskQueue_pick_highest_ready(&runq);
    
    // 2. handle orig task
    cur->state = eTASK_ST_READY;
	
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

utask_t* get_current_utask(){
    uint64_t addr_utask;
    asm volatile("mrs %0, tpidr_el0" : "=r"(addr_utask));
    return (utask_t *)(addr_utask);
}

void switch_to_user_mode(){
    utask_t* utask = get_current_utask();
    
    uint64_t sp   = utask->sp;
    uint64_t func = utask->elr;
	
    user_context(sp, func);
}

void do_exec(void(*func)())
{
	task_t* task = get_current();
    my_printf("current task id = %d\n", task->id);
	
    //tpidr_el0
    uint64_t utask_addr = (uint64_t)&task->utask;
    asm volatile("mov     x6, %0" : "=r"(utask_addr));
    asm volatile("msr     tpidr_el0, x6");
	
	// ELR_EL1：使用者模式過程的程式計數器
	task->utask.elr = (uint64_t) func;
	
	// SP_EL0：使用者模式堆疊指標的位址
	task->utask.sp  = (uint64_t)ustack_pool[task->id+1];
	
	// SPSR_EL1：CPU 使用者模式狀態
    //asm volatile("ldr x6, 0");
    //asm volatile("msr spsr_el1, x6");
	
	// set elr_el1 to PC address after eret (jump to EL0)
	asm volatile("ldr x2, =switch_to_user_mode");
    asm volatile("msr     elr_el1, x2");
	asm volatile("bl      set_aux");
	
	//
	asm volatile("eret");
}

void do_fork(uint64_t elr)
{
    task_t *cur = get_current();
    task_t *new = NULL;
	uint64_t sp_el0;
    asm volatile("mrs %0, sp_el0" : "=r"(sp_el0));
    
    // allocate task struct and kernel stack
    int new_id = get_new_pid();
    
    new=&task_pool[new_id];
    new->id = new_id;
    
    //new->sp = (uint64_t)&kstack_pool[new_id][4096]; //???
    memcpy(&kstack_pool[new_id - 1] + 1, &kstack_pool[task->id - 1] + 1,
                   STACK_SIZE * sizeof(char));
    
    memcpy(&ustack_pool[new_id - 1] + 1, &ustack_pool[task->id - 1] + 1,
                   STACK_SIZE * sizeof(char));
    
    new->elr = elr;
    new->spsr = new->spsr_el1;
    
    new->utask.elr = cur->utask.elr;
    new->utask.sp  = sp_el0; //???
    
    new->base_priority = cur->priority;
    new->dynamic_priority = cur->priority;
    
    new->state = cur->state;
    new->ticks = cur->ticks;
    
    taskElementPool[new_id].task = new;
    
    if(IDLE_TASK_ID != new_id)
        taskQueue_push(&runq, &taskElementPool[new_id]);
}

void do_exit(uint64_t status)
{
    task_t *cur = get_current();
    cur->state = eTASK_ST_ZOMBIE;
    
    my_printf("Exited with status code: %d\n", status);
}