#include <stdint.h>
#include <stdarg.h>
#include "reg.h"
#include "util.h"
#include "uart.h"
#include "mailbox.h"
#include "fb.h"
#include "reset.h"
#include "shell.h"
#include "task.h"
#include "test.h"

static void run_shell()
{
    print_s("Bootloader running in QEMU GUI!\n");
    
    while(1) {
		shell();
	}
}

void req1()
{
    int tid_0, tid_1, tid_2;
    tid_0 = privilege_task_create(&Idle_task, eTASK_PRI_DEFAULT);
    tid_1 = privilege_task_create(&echo1, eTASK_PRI_DEFAULT);
    tid_2 = privilege_task_create(&echo2, eTASK_PRI_DEFAULT);
}

void req2()
{
    privilege_task_create(&priviledge_task1, eTASK_PRI_4);
    privilege_task_create(&priviledge_task2, eTASK_PRI_DEFAULT);
}

void req3()
{
	//do_exec();
	privilege_task_create(&loop_task, eTASK_PRI_DEFAULT);
	privilege_task_create(&loop_task, 3);
}

void req4()
{
	//4-1
	//privilege_task_create(&echo_user, eTASK_PRI_DEFAULT);
	
	//4-2
	//privilege_task_create(exec_user, eTASK_PRI_DEFAULT);
	//privilege_task_create(exec_user, 2);
    
    //4-3 & 4-4
    privilege_task_create(fork_exit_user, eTASK_PRI_DEFAULT);
}

int main(void)
{
    /* init --- beg --- */
    uart1_init();
	//uart0_init();
	
    fb_init();
    fb_loadSplashImage();
	
    task_init();
    
    // enable core timer
    core_timer_enable();
    /* init --- end --- */
    print_s("\033[2J\033[1;1H");
    //run_shell();
    
    /* ============ Lab4 beg ============ */
    /* 111111111 REQ-1 111111111 */
    //req1();
    
    /* 222222222 REQ-2 222222222 */
    //req2();
    
    /* 333333333 REQ-3 333333333 */
    //req3();
	
	/* 444444444 REQ-4 444444444 */
    req4();
	
    //context_switch(&task_pool[tid_1]);
    schedule();
	
    /* ============ Lab4 end ============ */
    return 0;
}
