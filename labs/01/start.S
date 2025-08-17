    .section .text._start
    .global _start
_start:
    // 關中斷 (CPSR I/F bit)
    mrs r0, cpsr
    orr r0, r0, #0xC0     // set I & F disable bits
    msr cpsr_c, r0

    // 設定堆疊指標
    ldr sp, =stack_top

    // 跳到 C 的 main
    bl main

1:  b 1b                 // 無窮迴圈

    .section .bss
    .align 4
    .space 4096
stack_top: