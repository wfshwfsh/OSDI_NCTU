.section .text._start
.global _start
#.align 4

_start:
    // 關閉 IRQ 和 FIQ (修改 DAIF)
    mrs     x0, daif
    orr     x0, x0, #0xC0    // I=0x80, F=0x40
    msr     daif, x0
	
	//check MPIDR_EL1: if cpu id==0, jump to label2
	mrs x1, MPIDR_EL1
	and x1, x1, #3
	cbz x1, 2f

1:
	wfe
	b 1b

2:
	// set stack before our code
    ldr     x0, =stack_top
    mov     sp, x0

	// clear bss
	ldr		x0, =__bss_start
	ldr		x1, =__bss_end
	mov 	x2, #0
	
bss_clear_loop:
	cmp x0, x1
    b.ge bss_done
    str x2, [x0], #8    // 64-bit system, 每次清 8 bytes
    b bss_clear_loop
bss_done:

3:
    // 跳到 C 的 main
    bl      main

4:
	wfe
	b 4b

.section .bss
.align 12                 // 4096 對齊
.space 4096
stack_top:
