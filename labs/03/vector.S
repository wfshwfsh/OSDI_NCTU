_exc_handler:
  bl exception_handler
  eret

_SYNC_handler:
  mov x0, #0
  mrs x1, esr_el2
  mrs x2, elr_el2
  bl sync_handler
  eret
  
_IRQ_handler:
  bl irq_handler
  eret

_FIQ_handler:
  eret
  
_SErr_handler:
  eret


// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
  
  b _exc_handler // branch to a handler function.
  .balign 0x80 // entry size is 0x80, .align will pad 0
  b _exc_handler
  .balign 0x80
  b _exc_handler
  .balign 0x80
  b _exc_handler
  .balign 0x80

  b _SYNC_handler
  .balign 0x80
  b _IRQ_handler
  .balign 0x80
  b _FIQ_handler
  .balign 0x80
  b _SErr_handler
  .balign 0x80
  
  b _SYNC_handler
  .balign 0x80
  b _IRQ_handler
  .balign 0x80
  b _FIQ_handler
  .balign 0x80
  b _SErr_handler
  .balign 0x80
  
  b _exc_handler
  .balign 0x80
  b _exc_handler
  .balign 0x80
  b _exc_handler
  .balign 0x80
  b _exc_handler
  .balign 0x80