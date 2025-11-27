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
    
    
    /* 111111111 REQ-1 111111111 */
    //int tid_0, tid_1, tid_2;
    //tid_0 = privilege_task_create(&Idle_task, eTASK_PRI_DEFAULT);
    //tid_1 = privilege_task_create(&echo1);
    //tid_2 = privilege_task_create(&echo2);
    /* 111111111 REQ-1 111111111 */
    
    /* 222222222 REQ-2 222222222 */
    privilege_task_create(&priviledge_task1, eTASK_PRI_4);
    privilege_task_create(&priviledge_task2, eTASK_PRI_DEFAULT);
    /* 222222222 REQ-2 222222222 */
    
    
    //context_switch(&task_pool[tid_1]);
    schedule();
}
