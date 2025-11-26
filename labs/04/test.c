#include "uart.h"
#include "task.h"

void Idle_task()
{
    my_printf(".\n");
    while(isQueueEmpty()){
        
        int cnt = 100000000;
        for(int i=0;i<cnt;i++) ;
        
        schedule();
    }
}

void echo1()
{
    int cnt = 100000000;
    my_printf("1..\n");
    
    while(cnt!=0){
        cnt--;
    }
    
    //req-1.3: call context_switch in task
    //context_switch(&task_pool[1]);
    schedule();
}

void echo2()
{
    int cnt = 100000000;
    my_printf("2..\n");
    while(cnt!=0){
        cnt--;
    }
    
    //req-1.3: call context_switch in task
    //context_switch(&task_pool[0]);
    schedule();
}
