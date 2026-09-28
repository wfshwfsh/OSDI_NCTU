#include <stddef.h>
#include <stdint.h>

typedef enum{
    eSYSCALL_UARTWRITE=0,
	eSYSCALL_UARTREAD,
	eSYSCALL_EXEC,
	eSYSCALL_FORK,
	eSYSCALL_EXIT,
	eSYSCALL_END,
    
}eSYSCALL;

size_t uart_write(const char buf[], size_t size);
size_t uart_read(const char buf[], size_t size);
void exec(void (*func)());
void fork();
void exit();
