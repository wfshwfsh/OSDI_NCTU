#include "reg.h"
#include "uart.h"
#include "timer.h"

#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define AUX_IRQ (1 << 29)

void set_aux() {
    *(ENABLE_IRQS_1) = AUX_IRQ;
}

void dummy_exception_handler()
{
	my_printf("dummy_exception_handler\n");
}

void sync_handler(unsigned long type, unsigned long esr, unsigned long elr)
{
	int iss, ec;
	iss = esr & ((1 << 24) -1);
	ec = esr >> 26;
	
	if(1 == iss){
		my_printf("type: %x\n", type);
		my_printf("Exception return address 0x%x\n", elr);
		my_printf("(EC)Exception class 0x%x\n", ec);
		my_printf("(ISS)Instruction specific syndrome 0x%x\n", iss);
	}else if(2 == iss){
        //asm volatile("ti:");
        core_timer_enable();
		local_timer_init();
    }else{
        my_printf("??? ISS = %d\n", iss);
        ;
    }
}

void irq_handler()
{
	my_printf("irq_handler\n");
	
	//TBD - check irq source
	core_timer_handler();
	local_timer_handler();
}

void fiq_handler()
{
	my_printf("fiq_handler\n");
}

void serr_handler()
{
	my_printf("serr_handler\n");
}
