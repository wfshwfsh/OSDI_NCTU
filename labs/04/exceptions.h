void set_aux();
void dummy_exception_handler();
void sync_handler(unsigned long type, unsigned long esr, unsigned long elr);
void irq_handler();
void fiq_handler();
void serr_handler();
