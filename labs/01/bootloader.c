#include <stdint.h>
#include "io.h"
#include "util.h"


#if 0
/* Uart0: PL011-UART */
void uart1_init() {

    
}

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
#else
/* Uart1: mini-UART */
void uart1_init() {

    register unsigned int r;
    
    // 1. Set AUXENB register to enable mini UART. Then mini UART register can be accessed.
    *AUX_ENABLE = (1<<0);
    
    // 2. Set AUX_MU_CNTL_REG to 0. Disable transmitter and receiver during configuration.
    *AUX_MU_CNTL = 0;
    
    // 3. Set AUX_MU_IER_REG to 0. Disable interrupt because currently you don’t need interrupt.
    *AUX_MU_IER = 0;
    
    // 4. Set AUX_MU_LCR_REG to 3. Set the data size to 8 bit.
    *AUX_MU_LCR = 3;
    
    // 5. Set AUX_MU_MCR_REG to 0. Don’t need auto flow control.
    *AUX_MU_MCR = 0;
    
    // 6. Set AUX_MU_BAUD to 270. Set baud rate to 115200: 270 = (250MHz/8/115200)-1
    *AUX_MU_BAUD = 270;
    
    // 7. Set AUX_MU_IIR_REG to 6. (bit[1]=1 Clear Rx FIFO, bit[2]=1 Clear Tx FIFO)
    *AUX_MU_IIR = 6;

    /* map UART1 to GPIO pins */
    //r = *GPFSEL1;
    //r &= ~((7 << 12) | (7 << 15));  // gpio14, gpio15
    //r |= (2 << 12) | (2 << 15);     // alt5
    //*GPFSEL1 = r;
    //*GPPUD = 0;  // enable pins 14 and 15
    //r = 150;
    //while (r--) {
    //    asm volatile("nop");
    //}
    //*GPPUDCLK0 = (1 << 14) | (1 << 15);
    //r = 150;
    //while (r--) {
    //    asm volatile("nop");
    //}
    //*GPPUDCLK0 = 0;    // flush GPIO setup


    // 8. Set AUX_MU_CNTL_REG to 3. Enable the transmitter and receiver
    *AUX_MU_CNTL |= 3;
    
}

void uart1_send(char c) {
    
    do{
        asm volatile("nop");
    }while ( !(*AUX_MU_LSR & (1<<5)) );
    *AUX_MU_IO = c;
}

void uart1_puts(const char* str) {
    while (*str) {
        if (*str == '\n') uart1_send('\r'); // 加上 CRLF
        uart1_send(*str++);
    }
}

char uart1_getc() {
    char r;
    /* wait until something is in the buffer */
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & (1<<0)));
    /* read it and return */
    r = (char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}
#endif

char read_c() { return uart1_getc(); }
void print_s(char *ch) { uart1_puts(ch); }
void print_c(char ch) { uart1_send(ch); }
void print_i(int x) {
    if (x < 0) {
        print_c('-');
        x = -x;
    }
    if (x >= 10) print_i(x / 10);
    print_c(x % 10 + '0');
}




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
            "help      : print this help menu\n"
            "hello     : print Hello World!\n"
            "timestamp : print system timestamp\n"
            "reboot    : reboot the device\r\n");
	}else if(0 == strcmp(cmd, "hello")){
		print_s("Hello World!\n");
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
		print_s("command not found");
	}
}

int main(void)
{
    uart1_init();
    print_s("\033[2J\033[1;1H");
    print_s("Bootloader running in QEMU GUI!\n");
    
	while(1) {
		shell();
	}
}
