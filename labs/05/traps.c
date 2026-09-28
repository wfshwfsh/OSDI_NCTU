#include "reg.h"
#include "uart.h"
#include "timer.h"
#include "syscall.h"
#include "task.h"

#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define AUX_IRQ (1 << 29)

void set_aux() {
    *(ENABLE_IRQS_1) = AUX_IRQ;
}

void dummy_exception_handler()
{
	my_printf("dummy_exception_handler\n");
}

void sync_handler(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7)
{
	uint64_t iss, ec;
	uint64_t syscall_no, esr, elr;
    asm volatile("mov %0, x8" : "=r"(syscall_no));
	asm volatile("mrs %0, esr_el1" : "=r"(esr));
	asm volatile("mrs %0, elr_el1" : "=r"(elr));
	
	iss = esr & ((1 << 24) -1);
	ec = esr >> 26;
	
	if (0 == iss){
		switch (syscall_no){
			case eSYSCALL_UARTWRITE:
				for(uint64_t i=0;i<x1;i++){
					uart1_send(*((char *)x0 + i));
				}
				break;
			case eSYSCALL_UARTREAD:
				for(uint64_t i=0;i<x1;i++){
					*((char *)x0 + i) = uart1_getc();
				}
				break;
			case eSYSCALL_EXEC:
				do_exec((void (*)())x0);
				break;
			case eSYSCALL_FORK:
				break;
			case eSYSCALL_EXIT:
				break;
			default:
				break;
		}
	}else if(1 == iss && ec == 0x15){
		my_printf("Exception return address 0x%x\n", elr);
		my_printf("(EC)Exception class 0x%x\n", ec);
		my_printf("(ISS)Instruction specific syndrome 0x%x\n", iss);
	}else if(2 == iss && ec == 0x15){
		my_printf("enable timer\n");
        core_timer_enable();
		local_timer_init();
    }else{
        my_printf("??? ISS = %d\n", iss);
    }
}

void irq_handler()
{
    unsigned int core0_irq_src = *CORE0_IRQ_SRC;
	//my_printf("irq_handler = 0x%x\n", core0_irq_src);
    
    if(core0_irq_src & (1 << 1)){
        //my_printf("Generic timer interrupt pending\n");
        core_timer_handler();
    }
    
    if(core0_irq_src & (1 << 11)){
        //my_printf("Local timer interrupt pending\n");
        local_timer_handler();
    }
    
    // Still in ISR, Do NOT schedule here! //
}

void fiq_handler()
{
	my_printf("fiq_handler\n");
}

void serr_handler()
{
	my_printf("serr_handler\n");
}
