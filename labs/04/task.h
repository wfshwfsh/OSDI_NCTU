#include <stdint.h>

#define XNOF_PROCESS 64

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
    
    eTask_state state;
    int id;
    //int type; //??? user or kernel task
    //int sp; //kernel stack pointer
    
    void (*func)();
    
}task_t;

extern task_t task_pool[XNOF_PROCESS];
extern char kstack_pool[XNOF_PROCESS][4096];
    

