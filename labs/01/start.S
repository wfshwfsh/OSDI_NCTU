    .section .text._start
    .global _start
    .align 4
_start:
    // 關閉 IRQ 和 FIQ (修改 DAIF)
    mrs     x0, daif
    orr     x0, x0, #0xC0    // I=0x80, F=0x40
    msr     daif, x0

    // 設定堆疊指標
    ldr     x0, =stack_top
    mov     sp, x0

    // 跳到 C 的 main
    bl      main

1:  b       1b               // 無窮迴圈

    .section .bss
    .align 12                 // 4096 對齊
    .space 4096
stack_top: