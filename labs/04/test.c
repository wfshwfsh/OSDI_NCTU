#include "uart.h"
#include "task.h"

void echo1()
{
    int cnt = 100000000;
    my_printf("1..\n");
    
    while(cnt!=0){
        cnt--;
    }
    
    //req-1.3: call context_switch in task
    context_switch(&task_pool[1]);
}

void echo2()
{
    int cnt = 100000000;
    my_printf("2..\n");
    while(cnt!=0){
        cnt--;
    }
    
    //req-1.3: call context_switch in task
    context_switch(&task_pool[0]);
}
