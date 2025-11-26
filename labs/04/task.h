#include <stdint.h>

#define XNOF_PROCESS 64
#define XNOF_TASK_RUNQUEUE 32

typedef enum{
    eTASK_ST_READY=0,
    eTASK_ST_RUNNING=eTASK_ST_READY,
    eTASK_ST_SLEEP, //interruptible, or uninterruptible
    eTASK_ST_ZOMBIE,
    eTASK_ST_DEAD,
    
}eTask_state;

typedef struct task{
    // save caller register x19~x28, fp, lr, sp
    uint64_t context[10];
    uint64_t fp;
    uint64_t lr;
    uint64_t sp;
    
    int id;
    eTask_state state;
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
	
}runQueue_t;

extern task_t task_pool[XNOF_PROCESS];
extern char kstack_pool[XNOF_PROCESS][4096];
    

void task_init();
int privilege_task_create(void(*func)());
void context_switch(struct task* next);
void schedule();