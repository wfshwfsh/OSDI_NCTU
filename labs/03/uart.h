void uart0_init();
void uart0_send(char c);
void uart0_puts(const char* str);

void uart1_init();
void uart1_send(char c);
void uart1_puts(const char* str);
char uart1_getc();
char read_c();
void print_s(char *ch);
void print_c(char ch);
void print_i(int value);
void print_x(unsigned int value);
void my_printf(const char *fmt, ...);

