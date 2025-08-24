#include <stdint.h>
#include "util.h"

#define MMIO_BASE       0x3F000000
/* Auxilary mini UART registers */
#define AUX_ENABLE ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR ((volatile unsigned int*)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int*)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT ((volatile unsigned int*)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))

#define UART0_BASE 0x3F201000
#define UART0_DR   ((volatile unsigned int*)(UART0_BASE + 0x00))
#define UART0_FR   ((volatile unsigned int*)(UART0_BASE + 0x18))

void uart_send(char c) {
    while (*UART0_FR & (1 << 5)); // TXFF bit, transmit FIFO full
    *UART0_DR = c;
}

void uart_puts(const char* str) {
    while (*str) {
        if (*str == '\n') uart_send('\r'); // 加上 CRLF
        uart_send(*str++);
    }
}

char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x01));
    /* read it and return */
    r = (char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

void print_s(char *ch) { uart_puts(ch); }
void print_c(char ch) { uart_send(ch); }
char read_c() { return uart_getc(); }

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
	print_s("$ ");
	read_cmd(cmd);
	
	if(0 == strcmp(cmd, "help")){
		print_s(
            "help      : print this help menu\n"
            "hello     : print Hello World!\n"
            "timestamp : print system timestamp\n"
            "reboot    : reboot the device\r\n");
	}else if(0 == strcmp(cmd, "hello")){
		;
	}else if(0 == strcmp(cmd, "timestamp")){
		;
	}else if(0 == strcmp(cmd, "reboot")){
		;
	}else if(0 == strcmp(cmd, "clear")){
		print_s("\033[2J\033[1;1H");
	}else{
		print_s("command not found");
	}
}

int main(void)
{
    uart_puts("Bootloader running in QEMU GUI!\n");

	while(1) {
		shell();
	}
}
