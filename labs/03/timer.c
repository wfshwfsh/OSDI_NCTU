#include "uart.h"

#define LOCAL_TIMER_CONTROL ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int*)0x40000038)

#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)0x40000040)
#define EXPIRE_PERIOD 0xfffffff



void core_timer_enable() {
    
    asm volatile(
        ".global CORE0_TIMER_IRQ_CTRL;"
        "CORE0_TIMER_IRQ_CTRL:;"
        ".quad 0x40000040;"
        ".equ EXPIRE_PERIOD, 0xffffffff;"
        "mov x0, 1;"
        "msr cntp_ctl_el0, x0;" // enable timer
        "mov x0, #EXPIRE_PERIOD;"
        "msr cntp_tval_el0, x0;" // set expired time
        "mov x0, 2;"
        "ldr x1, =CORE0_TIMER_IRQ_CTRL;"
		"str x0, [x1];"
		"msr daifclr, #2;"
    );
}

void core_timer_handler(){
    unsigned long cval;
    asm volatile("mrs %0, cntp_cval_el0" : "=r"(cval));
    cval += EXPIRE_PERIOD;
    
    my_printf("core timer\n");
    //asm volatile(
        //".equ EXPIRE_PERIOD, 0xffffffff;"
        //"mov x0, #EXPIRE_PERIOD;"
        asm volatile("mov x0, %0;" : "=r"(cval));
        asm volatile("msr cntp_tval_el0, x0;");
    //);
}

void local_timer_init(){
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 25000000;
  *LOCAL_TIMER_CONTROL = flag | reload;
  
  //asm volatile("msr daifclr, #0xf");
  asm volatile("msr daifclr, #2");
}

void local_timer_handler(){
  int k = 214143141;

  *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
  my_printf("local timer\n");

  asm volatile("msr daifclr, #2");
  while(k--) {
      asm volatile("nop");
  }
}




