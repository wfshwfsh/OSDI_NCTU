#include <stdint.h>
#include "util.h"
#include "uart.h"
#include "mailbox.h"
#include "reset.h"

void read_cmd(char *cmd) {
    char now;
    cmd[0] = 0;
    int now_cur = 0;
    while ((now = read_c()) != '\n') {
        if ((int)now == 240) {
            continue;
        } else if (now == 127) {  // delete
            now_cur -= 1;
            if (now_cur >= 0) {
                print_s("\b \b");
                now_cur -= 1;
            }
        } else {
            cmd[now_cur] = now;
            print_c(now);
        }
        now_cur++;
    }
    print_s("\n");
    cmd[now_cur] = 0;
}

void shell()
{
	char cmd[256]={};
	my_printf("$ ");
	read_cmd(cmd);
	
	if(0 == strcmp(cmd, "help")){
		my_printf(
            "help      	: print this help menu\n"
            "hello     	: print Hello World!\n"
			"hw    		: print HW info!\n"
            "timestamp 	: print system timestamp\n"
            "reboot    	: reboot the device\r\n");
	}else if(0 == strcmp(cmd, "hello")){
		my_printf("Hello World!\n");
	}else if(0 == strcmp(cmd, "hw")){
		get_board_revision();
		get_vc_memory_addr();
	}else if(0 == strcmp(cmd, "timestamp")){
        uint64_t tm_cnt, tm_frq, tm_sec, tm_co;
        asm volatile("mrs %0, cntpct_el0" : "=r"(tm_cnt));
        asm volatile("mrs %0, cntfrq_el0" : "=r"(tm_frq));
        tm_sec = tm_cnt/tm_frq;
        my_printf("[%d]\n", tm_sec);
        
	}else if(0 == strcmp(cmd, "reboot")){
		reset(10);
	}else if(0 == strcmp(cmd, "clear")){
		my_printf("\033[2J\033[1;1H");
	}else{
		my_printf("command not found\n");
	}
}
