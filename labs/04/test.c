#include "uart.h"
#include "task.h"
#include "syscall.h"

void Idle_task()
{
    int cnt = 10000000;
    my_printf(".\n");
    while(1){
        my_printf("Idle...\n");
       // ç¢ºä? DAIF ??IRQ bit æ¸…é™¤ (?Ÿç”¨ IRQ)
        asm volatile("msr daifclr, #2");
        asm volatile("wfi"); // ç­‰å? Interrupt ?šé?
		for(int i=0;i<cnt;i++) ;
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

void loop_user()
{
    my_printf("loop_user \n");
    while(1)
	;
}

void loop_task()
{
	my_printf("loop_task\n");
    do_exec(loop_user);
}

void echo_user()
{
	char ch[10]={};
	uart_write("echo_user \n", 11);
	uart_read(ch, 1);
	my_printf("your input: ");
	uart_write(ch, 1);
	my_printf("\n");
	while(1)
		;
}

void exec_user() {
    print_s("exec user (call loop user)\n");
    exec(loop_user);
    while (1)
        ;
}

void fork_exit_user()
{
    print_s("fork exit user\n");
    int pid=fork();
    if(0 == pid){
        my_printf("pid=%d\n", pid);
        while(1)
            ;
    }else{
        my_printf("pid=%d\n", pid);
        exit(2);
        my_printf("should not print here\n");
    }
}