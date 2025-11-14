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
    /* init --- end --- */
    print_s("\033[2J\033[1;1H");
    
    //run_shell();
    
    int tid_1, tid_2;
    tid_1 = privilege_task_create(echo1);
    tid_2 = privilege_task_create(echo2);
    
    //run_task1 => context_switch(cur, task2) => context_switch(cur, task1)
    context_switch(&task_pool[tid_1]);
}
