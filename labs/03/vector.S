.macro kernel_entry
    stp x0,  x1,  [sp, #-16]!
    stp x2,  x3,  [sp, #-16]!
    stp x4,  x5,  [sp, #-16]!
    stp x6,  x7,  [sp, #-16]!
    stp x8,  x9,  [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    str x30,      [sp, #-16]!    /* push x30 last (single) */
.endm

.macro kernel_exit
    ldr x30,      [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8,  x9,  [sp], #16
    ldp x6,  x7,  [sp], #16
    ldp x4,  x5,  [sp], #16
    ldp x2,  x3,  [sp], #16
    ldp x0,  x1,  [sp], #16
.endm

_dummy_exc_handler:
  kernel_entry
  bl dummy_exception_handler
  kernel_exit
  eret

_SYNC_handler:
  kernel_entry
  mov x0, #0
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl sync_handler
  kernel_exit
  eret
  
_IRQ_handler:
  kernel_entry
  bl irq_handler
  kernel_exit
  eret

_FIQ_handler:
  kernel_entry
  kernel_exit
  eret
  
_SErr_handler:
  kernel_entry
  kernel_exit
  eret


// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:

  /* Exception from current EL while using EL0 */
  .align  7 // entry size is 0x80, .align will pad 0
  b _dummy_exc_handler // branch to a handler function.
  .align  7
  b _dummy_exc_handler
  .align  7
  b _dummy_exc_handler
  .align  7
  b _dummy_exc_handler
  
  /* Exception from current EL while using ELx */
  .align  7
  b _SYNC_handler
  .align  7
  b _IRQ_handler
  .align  7
  b _FIQ_handler
  .align  7
  b _SErr_handler
  
  /* Exception from lower EL and at least one lower EL is AArch64 */
  .align  7
  b _SYNC_handler
  .align  7
  b _IRQ_handler
  .align  7
  b _dummy_exc_handler
  .align  7
  b _dummy_exc_handler
  
  /* Exception from lower EL and all lower EL are AArch32 */
  .align  7
  b _dummy_exc_handler
  .align  7
  b _dummy_exc_handler
  .align  7
  b _dummy_exc_handler
  .align  7
  b _dummy_exc_handler
