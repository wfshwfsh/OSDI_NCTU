#include "uart.h"
#include "task.h"

void Idle_task()
{
    int cnt = 10000000;
    my_printf(".\n");
    while(1){
        my_printf("A\n");
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

void priviledge_task1()
{
    int cnt = 1000000;
    my_printf("TASK1 Entry --- \n");
    while(1){
        if(flag_reschedule){
            my_printf("Task1 Reschedule\n");
            flag_reschedule = false;
            schedule();
        }
        
        while(cnt!=0){
            cnt--;
        }
    }
}

void priviledge_task2()
{
    int cnt = 1000000;
    my_printf("TASK2 Entry --- \n");
    while(1){
        if(flag_reschedule){
            my_printf("Task2 Reschedule\n");
            flag_reschedule = false;
            schedule();
        }
        
        while(cnt!=0){
            cnt--;
        }
    }
}