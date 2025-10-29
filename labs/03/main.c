#include <stdint.h>
#include <stdarg.h>
#include "reg.h"
#include "util.h"
#include "uart.h"
#include "mailbox.h"
#include "fb.h"
#include "reset.h"
#include "shell.h"

int main(void)
{
    uart1_init();
	//uart0_init();
	
    fb_init();
    fb_loadSplashImage();
	
    print_s("\033[2J\033[1;1H");
    print_s("Bootloader running in QEMU GUI!\n");
	
	while(1) {
		shell();
	}
}
