#include <stdint.h>
#include <stdbool.h>

#define XNOF_PROCESS 64
#define XNOF_TASK_RUNQUEUE 32
#define IDLE_TASK_ID 0


#define TASK_WAIT_THRESHOLD 4

typedef enum{
    eTASK_ST_READY=0,
    eTASK_ST_RUNNING,
    eTASK_ST_SLEEP, //interruptible, or uninterruptible
    eTASK_ST_ZOMBIE,
    eTASK_ST_DEAD,
    
}eTask_state;


typedef enum{
	eTASK_PRI_0=0,
	eTASK_PRI_IDLE=eTASK_PRI_0,
    eTASK_PRI_1,
    eTASK_PRI_2,
    eTASK_PRI_3,
    eTASK_PRI_4,
    eTASK_PRI_5,
    eTASK_PRI_6,
    eTASK_PRI_DEFAULT=eTASK_PRI_6,
    eTASK_PRI_7,
    eTASK_PRI_8,
    eTASK_PRI_9
}eTASK_PRI;
#define MIN_TASK_PRIORITY   eTASK_PRI_1
#define MAX_TASK_PRIORITY   eTASK_PRI_8

typedef struct uContext{
    
    uint64_t sp;	//usr mode's stack ptr
    uint64_t elr;	//elr_el1: exception return addr(PC)
    uint64_t spsr;  //spsr_el1: 
    
}uContext_t;

typedef struct kContext{
    
    // save caller register x19~x28, fp, lr, sp
    uint64_t context[10];
    
    uint64_t fp; //x29:
    uint64_t lr; //x30: br/blr return addr
    uint64_t sp;
    
}kContext_t;

typedef struct task{
    
    kContext_t kctx;
    uContext_t uctx;
	
	int id;
	eTask_state state;
    int base_priority;
	int dynamic_priority;
    int ticks;
	int wait_ticks;
	
}task_t;

typedef struct queueElement{
	
	task_t *task;
	struct queueElement *next;
	
}queueElement_t;


typedef struct runQueue{
	
	queueElement_t *head;
	queueElement_t *tail;
    int count;
    
    queueElement_t *idle;
	
}runQueue_t;

extern int pid;
extern bool flag_reschedule;
extern task_t task_pool[XNOF_PROCESS];
extern queueElement_t taskElementPool[XNOF_PROCESS];
extern uint64_t kstack_pool[XNOF_PROCESS][4096];
extern runQueue_t runq;


void task_init();
int privilege_task_create(void(*func)(), int priority);
task_t* get_current();
void context_switch(struct task* next);
void schedule();

void do_exec(void(*func)());
void do_fork(uint64_t elr);
void do_exit(uint64_t status);