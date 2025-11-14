#include "uart.h"

#define LOCAL_TIMER_CONTROL ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int*)0x40000038)

#define EXPIRE_PERIOD 0xfffffff

int local_timer_cnt=0;
int core_timer_cnt=0;


void core_timer_enable() {
    // 1. Generic Timer is ARM base timer, ctrl by arm system reg: cntp_ctl, cntp_tval
    // 2. 
    asm volatile(
        "mov x0, 2;"
        "ldr x1, =0x40000040;"
		"str x0, [x1];"
		        
        "mov x0, 0xfffff;"
        "msr cntp_tval_el0, x0;" // set expired time
        
        "mov x0, 1;"
        "msr cntp_ctl_el0, x0;"  // enable timer
        

        "msr daifclr, #2;"
    );
}

void core_timer_handler(){
	
    core_timer_cnt++;
    my_printf("core timer isr - %d\n", core_timer_cnt);
	
    asm volatile("mov x0, 0x1");
    asm volatile("mrs x1, CNTFRQ_EL0");
    asm volatile("mul x0, x0, x1");
    asm volatile("msr cntp_tval_el0, x0");
	return;
}

void local_timer_init(){
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 25000000;
  *LOCAL_TIMER_CONTROL = flag | reload;
  
  asm volatile("msr daifclr, #2");
}

void local_timer_handler(){
  int k = 214143141;
  local_timer_cnt++;

  *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
  my_printf("local timer isr - %d\n", local_timer_cnt);

  //asm volatile("msr daifclr, #2");
  //while(k--) {
  //    asm volatile("nop");
  //}
}




