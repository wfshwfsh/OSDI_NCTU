#include <stdint.h>

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

int main(void)
{
    uart_puts("Bootloader running in QEMU GUI!\n");

    // 模擬跳到 app
    for(volatile int i=0;i<10000;i++);  // 簡單延遲

    uart_puts("Bootloader finished.\n");

    while(1) { __asm__ volatile("wfi"); }
}
