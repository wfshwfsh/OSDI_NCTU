#include "uart.h"

void echo1()
{
    int cnt = 100000000;
    my_printf("1..\n");
    
    while(cnt!=0){
        cnt--;
    }
    
    //call context_switch(prev, next)
}

void echo2()
{
    int cnt = 100000000;
    my_printf("2..\n");
    while(cnt!=0){
        cnt--;
    }
    
    //call context_switch(prev, next)
}
