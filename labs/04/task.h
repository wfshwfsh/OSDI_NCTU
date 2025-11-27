#include <stdint.h>
#include <stdbool.h>

#define XNOF_PROCESS 64
#define XNOF_TASK_RUNQUEUE 32
#define IDLE_TASK_ID 0

typedef enum{
    eTASK_ST_READY=0,
    eTASK_ST_RUNNING=eTASK_ST_READY,
    eTASK_ST_SLEEP, //interruptible, or uninterruptible
    eTASK_ST_ZOMBIE,
    eTASK_ST_DEAD,
    
}eTask_state;

typedef enum{
    eTASK_PRI_1=1,
    eTASK_PRI_IDLE=eTASK_PRI_1,
    eTASK_PRI_2,
    eTASK_PRI_3,
    eTASK_PRI_4,
    eTASK_PRI_5,
    eTASK_PRI_6,
    eTASK_PRI_DEFAULT=eTASK_PRI_6,
    eTASK_PRI_7,
    eTASK_PRI_8,
    eTASK_PRI_9,
};

typedef struct task{
    // save caller register x19~x28, fp, lr, sp
    uint64_t context[10];
    uint64_t fp;
    uint64_t lr;
    uint64_t sp;
    
    int id;
    eTask_state state;
    int priority;
    int tick;
    
    //int type; //??? user or kernel task
    //int sp; //kernel stack pointer
    
    void (*func)();
    
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
extern char kstack_pool[XNOF_PROCESS][4096];
extern runQueue_t runq;


void task_init();
int privilege_task_create(void(*func)(), int priority);
task_t* get_current();
void context_switch(struct task* next);
void schedule();

