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

    my_printf("11111\n");
    //uart0_init();
    my_printf("22222\n");
    
    //fb_init();
    my_printf("33333\n");
    //fb_loadSplashImage();

    my_printf("11111\n");    
	while(1) {
		shell();
	}
}
