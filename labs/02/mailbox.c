#include "reg.h"
#include "mailbox.h"
#include "util.h"

volatile unsigned int __attribute__((aligned(16))) mailbox[64];

void mailbox_call(int ch)
{
	unsigned int req = (((unsigned int)((unsigned long)&mailbox) & ~0xF) | (ch & 0xF));
	unsigned int resp;
	
	do {
		asm volatile("nop");
	}while(*MAILBOX_STATUS & MAILBOX_FULL);
	*MAILBOX_WRITE = req;
	
	//do {
		while (! *MAILBOX_STATUS & MAILBOX_EMPTY) ;
		resp = *MAILBOX_READ;
		
	//}while( ((resp & 0xf) != 8 || (resp & ~0xf)) !=&mailbox );
	
}



void get_board_revision(){
  
  memset(&mailbox, 0, sizeof(mailbox));
  
  mailbox[0] = 7 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_BOARD_REVISION; // tag identifier
  mailbox[3] = 4; // maximum of request and response value buffer's length.
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0; // value buffer
  // tags end
  mailbox[6] = END_TAG;
  
  mailbox_call(MAILBOX_CH__ARM2VC);
  print_i(mailbox[5]); // it should be 0xa020d3 for rpi3 b+
  print_s("\n");
}

void get_vc_memory_addr(){
  
  memset(&mailbox, 0, sizeof(mailbox));
  
  mailbox[0] = 8 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_VC_MEMADDR; // tag identifier
  mailbox[3] = 8; // maximum of request and response value buffer's length.
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0; // value buffer
  mailbox[6] = 0; // value buffer
  // tags end
  mailbox[7] = END_TAG;
  
  mailbox_call(MAILBOX_CH__ARM2VC);
  print_s("base_addr: ");
  print_i(mailbox[5]);
  print_s("\n");
  
  print_s("size: ");
  print_i(mailbox[6]);
  print_s("\n");
}

void get_uart0_clk_state(){
  
  memset(&mailbox, 0, sizeof(mailbox));
  
  mailbox[0] = 8 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = 0x00030001; // get clk state
  mailbox[3] = 4;
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0x000000002; // uart clk id
  mailbox[6] = 0; // value buffer - state

  // tags end
  mailbox[7] = END_TAG;
  
  mailbox_call(MAILBOX_CH__ARM2VC);
  print_s("clk_id: ");
  print_i(mailbox[5]);
  print_s("\n");
  
  print_s("clk_state: ");
  print_i(mailbox[6]);
  print_s("\n");
}

int get_uart0_clk_rate(){
  
  memset(&mailbox, 0, sizeof(mailbox));
  
  mailbox[0] = 8 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = 0x00030002; // get clk state
  mailbox[3] = 4;
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0x000000002; // uart clk id
  mailbox[6] = 0; // value buffer - state
  
  // tags end
  mailbox[7] = END_TAG;
  
  mailbox_call(MAILBOX_CH__ARM2VC);
  print_s("clk_id: ");
  print_i(mailbox[5]);
  print_s("\n");
  
  print_s("clk_rate: ");
  print_i(mailbox[6]);
  print_s("\n");
  
  return mailbox[6];
}





