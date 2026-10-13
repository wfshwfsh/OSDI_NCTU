.macro kernel_entry
    str   x0, [sp, #-16]!
    str   x1, [sp, #-16]!
    str   x2, [sp, #-16]!
    str   x3, [sp, #-16]!
    str   x4, [sp, #-16]!
    str   x5, [sp, #-16]!
    str   x6, [sp, #-16]!
    str   x7, [sp, #-16]!
    str   x8, [sp, #-16]!
    str   x9, [sp, #-16]!
    str   x10, [sp, #-16]!
    str   x11, [sp, #-16]!
    str   x12, [sp, #-16]!
    str   x13, [sp, #-16]!
    str   x14, [sp, #-16]!
    str   x15, [sp, #-16]!
    str   x16, [sp, #-16]!
    str   x17, [sp, #-16]!
    str   x18, [sp, #-16]!
    str   x19, [sp, #-16]!
    str   x20, [sp, #-16]!
    str   x21, [sp, #-16]!
    str   x22, [sp, #-16]!
    str   x23, [sp, #-16]!
    str   x24, [sp, #-16]!
    str   x25, [sp, #-16]!
    str   x26, [sp, #-16]!
    str   x27, [sp, #-16]!
    str   x28, [sp, #-16]!
    str   x29, [sp, #-16]!
    str   x30, [sp, #-16]!
  .endm

.macro kernel_exit
    ldr   x30, [sp], #16  
    ldr   x29, [sp], #16  
    ldr   x28, [sp], #16  
    ldr   x27, [sp], #16  
    ldr   x26, [sp], #16  
    ldr   x25, [sp], #16  
    ldr   x24, [sp], #16  
    ldr   x23, [sp], #16  
    ldr   x22, [sp], #16  
    ldr   x21, [sp], #16  
    ldr   x20, [sp], #16  
    ldr   x19, [sp], #16  
    ldr   x18, [sp], #16  
    ldr   x17, [sp], #16  
    ldr   x16, [sp], #16  
    ldr   x15, [sp], #16  
    ldr   x14, [sp], #16  
    ldr   x13, [sp], #16  
    ldr   x12, [sp], #16  
    ldr   x11, [sp], #16  
    ldr   x10, [sp], #16  
    ldr   x9, [sp], #16  
    ldr   x8, [sp], #16  
    ldr   x7, [sp], #16  
    ldr   x6, [sp], #16  
    ldr   x5, [sp], #16  
    ldr   x4, [sp], #16  
    ldr   x3, [sp], #16  
    ldr   x2, [sp], #16  
    ldr   x1, [sp], #16  
	ldr   x0, [sp], #16
.endm


// vector.S
.global child_return_from_fork
child_return_from_fork:
    kernel_exit          // 將 trap_frame 上的 x1~x30 全部 pop 出來
    ldr   x0, [sp], #16  // 彈出 x0 (在 do_fork 中已被填為 0)
    eret                 // 透過硬體機制安全返回 EL0，恢復 PC 與 PSTATE

_dummy_exc_handler:
  kernel_entry
  bl dummy_exception_handler
  kernel_exit
  #ldr   x0, [sp], #16
  eret

_SYNC_handler:
  kernel_entry
  mov x6, sp
  bl sync_handler
  kernel_exit
  eret

_IRQ_handler:
  kernel_entry
  bl irq_handler
  kernel_exit
  #ldr   x0, [sp], #16
  msr daifclr, #2
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
