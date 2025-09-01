#include "mailbox.h"
#include "util.h"

volatile unsigned int __attribute__((aligned(16))) mailbox[64];

void mailbox_w(int ch)
{

}

void mailbox_r(int ch)
{

}

void mailbox_call(int ch)
{
	//mailbox_w(ch);
	//mailbox_r(ch);
	
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
  
  mailbox[0] = 6 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_BOARD_REVISION; // tag identifier
  mailbox[3] = 4; // maximum of request and response value buffer's length.
  //mailbox[4] = TAG_REQUEST_CODE;
  mailbox[4] = 0; // value buffer
  // tags end
  mailbox[5] = END_TAG;

  //mailbox_call(mailbox); // message passing procedure call, you should implement it following the 6 steps provided above.
  mailbox_call(MAILBOX_CH__ARM2VC);

  //printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
  print_i(mailbox[5]);
  print_s("\n");
}

void get_vc_memory_addr(){
  
  memset(&mailbox, 0, sizeof(mailbox));
  
  mailbox[0] = 7 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_VC_MEMADDR; // tag identifier
  mailbox[3] = 8; // maximum of request and response value buffer's length.
  //mailbox[4] = TAG_REQUEST_CODE;
  mailbox[4] = 0; // value buffer
  mailbox[5] = 0; // value buffer
  // tags end
  mailbox[6] = END_TAG;

  //mailbox_call(mailbox); // message passing procedure call, you should implement it following the 6 steps provided above.
  mailbox_call(MAILBOX_CH__ARM2VC);

  //printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
  print_s("base_addr: ");
  print_i(mailbox[5]);
  print_s("\n");
  print_s("size: ");
  print_i(mailbox[6]);
  print_s("\n");
}







