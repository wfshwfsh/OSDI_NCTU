#include <stdint.h>
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
    my_printf("\033[2J\033[1;1H");
    my_printf("Bootloader running in QEMU GUI!\n");
	
    uart0_init();
    
    fb_init();
    fb_loadSplashImage();
    
	while(1) {
		shell();
	}
}
