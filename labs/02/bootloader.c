#include <stdint.h>
#include "reg.h"
#include "util.h"
#include "uart.h"
#include "mailbox.h"
#include "fb.h"

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

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void reset(int tick){ // reboot after watchdog timer expire
  //set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
  //set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
  
  *PM_RSTC = (PM_PASSWORD | 0x20);
  *PM_WDOG = (PM_PASSWORD | tick);
}

void cancel_reset() {
  //set(PM_RSTC, PM_PASSWORD | 0); // full reset
  //set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
  
  *PM_RSTC = (PM_PASSWORD | 0);
  *PM_WDOG = (PM_PASSWORD | 0);
}

void shell()
{
	char cmd[256]={};
	print_s("$ ");
	read_cmd(cmd);
	
	if(0 == strcmp(cmd, "help")){
		print_s(
            "help      	: print this help menu\n"
            "hello     	: print Hello World!\n"
			"hw    		: print HW info!\n"
            "timestamp 	: print system timestamp\n"
            "reboot    	: reboot the device\r\n");
	}else if(0 == strcmp(cmd, "hello")){
		print_s("Hello World!\n");
	}else if(0 == strcmp(cmd, "hw")){
		get_board_revision();
		get_vc_memory_addr();
	}else if(0 == strcmp(cmd, "timestamp")){
        uint64_t tm_cnt, tm_frq, tm_sec, tm_co;
        asm volatile("mrs %0, cntpct_el0" : "=r"(tm_cnt));
        asm volatile("mrs %0, cntfrq_el0" : "=r"(tm_frq));
        tm_sec = tm_cnt/tm_frq;
        print_i(tm_sec);
        //print_s(".");
		//print_i(tm_frq);
        print_s("\n");
	}else if(0 == strcmp(cmd, "reboot")){
		reset(10);
	}else if(0 == strcmp(cmd, "clear")){
		print_s("\033[2J\033[1;1H");
	}else{
		print_s("command not found\n");
	}
}

int main(void)
{
    uart1_init();
    print_s("\033[2J\033[1;1H");
    print_s("Bootloader running in QEMU GUI!\n");
	
    uart0_init();
    
    
    fb_init();
    fb_loadSplashImage();
    
	while(1) {
		shell();
	}
}
