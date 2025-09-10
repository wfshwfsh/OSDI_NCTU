#include <stdarg.h>
#include "reg.h"
#include "mailbox.h"


/* Uart0: PL011-UART */
void uart0_init() {
	int r,clk_uart0;
    
	// 1. Configure the UART clock frequency by mailbox
	get_uart0_clk_state();
    clk_uart0 = get_uart0_clk_rate(); // 3MHz
	
	// 2. Enable GPIO (almost same as mini UART).
    /* map UART0 to GPIO pins */
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15));  // gpio14, gpio15
    r |= (4 << 12) | (4 << 15);     // alt0
    *GPFSEL1 = r;
    *GPPUD = 0;  // enable pins 14 and 15
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;  // flush GPIO setup
	
    // Disable CR before setting all uart reg
    *UART0_CR = 0;
    
	// 3. Set IBRD and FBRD to configure baud rate.
    // BaudDiv = 3MHz/(16 * 115200)
	*UART0_IBRD = 1;
    *UART0_FBRD = 62;
	
	// 4. Set LCRH to configure line control.
	*UART0_LCRH = (3<<5);
	
	// 5. Set CR to enable UART.
    *UART0_CR = ((1<<9) | (1<<8) | (1<<0));
	
}

void uart0_send(char c) {
    while (*UART0_FR & (1 << 5)); // TXFF bit, transmit FIFO full
    *UART0_DR = c;
}

void uart0_puts(const char* str) {
    while (*str) {
        if (*str == '\n') uart0_send('\r'); // 加上 CRLF
        uart0_send(*str++);
    }
}

char uart0_getc() {
    char r;
    /* wait until something is in the buffer */
    do {
        asm volatile("nop");
    } while (!(*UART0_FR & (1<<4))); //RXFE, Receive FIFO empty
    /* read it and return */
    r = (char)(*UART0_DR);
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

/* Uart1: mini-UART */
void uart1_init() {

    //register unsigned int r;
    
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

char read_c() { return uart1_getc(); }
void print_c(char ch) { uart1_send(ch); }
void print_s(const char *ch) { uart1_puts(ch); }
//char read_c() { return uart0_getc(); }
//void print_c(char ch) { uart0_send(ch); }
//void print_s(char *ch) { uart0_puts(ch); }

void print_i(int value) {
    int neg=0, i=0;
    char buf[32]={};
    
    if (value < 0) {
        neg = 1;
        value = -value;
    }
    
    while(value>0){
        buf[i++] = value%10 + '0';
        value = value/10;
    }
    
    if(neg){
        buf[i++] = '-';
    }
    
    while(i>=0){
        print_c(buf[i--]);
    }
}

void print_x(unsigned int value)
{
    char HEX_STRING[]="0123456789abcdef";
    int i=0;
    char buf[32]={};
    
    while(value>0){
        buf[i++] = HEX_STRING[(value & 0xf)];
        //if((value & 0xf) < 10){
        //    buf[i++] = (value & 0xf) + '0';
        //}else{
        //    buf[i++] = (value & 0xf) - 10 + 'a';
        //}
        
        value = (value >> 4);
    }
    
    while(i>=0){
        print_c(buf[i--]);
    }
}

void my_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's':
                    print_s(va_arg(args, const char *));
                break;
                case 'd':
                    print_i(va_arg(args, int));
                break;
                case 'x':
                    print_x(va_arg(args, unsigned int));
                break;
            }
            fmt++;
        } else {
            print_c(*fmt);
            fmt++;
        }
    }
    
    va_end(args);
}
