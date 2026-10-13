#define IER_REG_EN_REC_INT (1 << 0)
#define IER_REG_INT (3 << 2)  // Must be set to receive interrupts
#define IER_REG_VALUE (IER_REG_EN_REC_INT | IER_REG_INT)

void uart0_init();
void uart0_send(char c);
void uart0_puts(const char* str);

void uart1_init();
void uart1_send(char c);
void uart1_puts(const char* str);
char uart1_getc();
char read_c();
void print_s(const char *ch);
void print_c(char ch);
void print_i(int value);
void print_x(unsigned int value);
void my_printf(const char *fmt, ...);
void print_OK();

