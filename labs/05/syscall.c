#include <stddef.h>
#include "syscall.h"
#include "task.h"

size_t uart_write(const char buf[], size_t size)
{
	asm volatile("mov x8, #0");
	asm volatile("svc 0");
	return size;
}

size_t uart_read(const char buf[], size_t size)
{
	asm volatile("mov x8, #1");
	asm volatile("svc 0");
	return size;
}

void exec(void (*func)())
{
	asm volatile("mov x8, #2");
	asm volatile("svc 0");
}

void fork()
{
	asm volatile("mov x8, #3");	
	asm volatile("svc 0");
}

void exit()
{
	asm volatile("mov x8, #4");
	asm volatile("svc 0");
}