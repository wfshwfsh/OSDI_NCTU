
#define CORE0_TIMER_IRQ_CTRL    0x40000040
#define CORE0_IRQ_SRC           ((volatile unsigned int*)(0x40000060))


void core_timer_enable();
void core_timer_handler();
void local_timer_init();
void local_timer_handler();

