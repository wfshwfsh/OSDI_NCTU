#include <stddef.h>
#include <stdbool.h>
#include "uart.h"
#include "task.h"

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
	
    task_t* curTask = get_current();
    core_timer_cnt++;
    my_printf("core timer isr - %d\n", core_timer_cnt);
    bool flag=false;
	
    // Refresh timer
    asm volatile("mov x0, 0x1");
    asm volatile("mrs x1, CNTFRQ_EL0");
    asm volatile("mul x0, x0, x1");
    asm volatile("msr cntp_tval_el0, x0");
    
    if(NULL == curTask)
	return;
	
    // Check current task tick
    if(--curTask->ticks <= 0){
	// Reduce Current Task Priority, when task time up
	if(curTask->dynamic_priority > MIN_TASK_PRIORITY){
	    curTask->dynamic_priority--;
	}
	flag=true;
    }

    // Increase Others task's wait_ticks and Schedule Priority
    queueElement_t *elem = runq.head;
    while(NULL != elem){
	task_t *t = elem->task;
   	if((NULL != t) && (eTASK_ST_RUNNING != t->state)){
		t->wait_ticks++;
		
		if(t->wait_ticks > TASK_WAIT_THRESHOLD){
			if(t->dynamic_priority < MAX_TASK_PRIORITY)
				t->dynamic_priority++;
		}
	}
		
	//my_printf("Task: id=%d, state=%d, pri=%d\n", t->id, t->state, t->dynamic_priority);
	elem = elem->next;
    }
    
    if(flag){
        flag = false;
        uint64_t elr, sp_el0, spsr_el1;
        asm volatile("mrs %0, elr_el1" : "=r"(elr));
        curTask->elr = elr;
        asm volatile("mrs %0, sp_el0" : "=r"(sp_el0));
        curTask->utask.sp = sp_el0;
        asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
        curTask->spsr = spsr_el1;
        curTask->ticks = 0;
        asm volatile("ldr x0, =schedule");
        asm volatile("msr elr_el1, x0");
    }
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




