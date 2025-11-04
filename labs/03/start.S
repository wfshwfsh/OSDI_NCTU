.section .text.boot
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

2:  // cpu id == 0

el2_entry:
	// set stack before our code
    ldr     x2, =stack_top
    mov     sp, x2
	
	// Set EL1 stack poiont
	msr 	sp_el1, x2
    isb
	
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    
    // Route interrupt to EL2
    //mov     x0, #(1 << 4)      // IMO
    //msr     hcr_el2, x0
    //mrs     x0, hcr_el2
	
	// Set EL1/EL0 allow ctrl
	mov     x0, #(1 << 31)      // bit[31]=1: AArch64
	orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    
    // Enable FP/NEON for EL1/EL0
    mov    x0, #(0b11 << 20)   // FPEN = 0b11 (EL0/EL1 可使用 FP/NEON)
    msr    cpacr_el1, x0
    isb

    // Setup SCTLR access
    mov     x0, #0x0800
    movk    x0, #0x30d0, lsl #16
    msr     sctlr_el1, x0

	// Set Exception Table
	ldr 	x0, =exception_table
	msr 	VBAR_EL1, x0
    isb

	// Save pstate to spsr_el2 before jump to EL1
	mov 	x1, #5				// M = 0b0101 -> EL1h
	orr     x1, x1, #(1 << 6)	// bit[6]=1 Disable SError
	orr     x1, x1, #(1 << 7)	// bit[7]=1 Disable FIQ
	orr     x1, x1, #(1 << 8)	// bit[8]=1 Disable IRQ
	msr     spsr_el2, x1
	
	// address EL1 and jump to
	adr x0, el1_entry
	msr elr_el2, x0
	
	eret

el1_entry:
    //mrs     x3, CurrentEL  //For checking jump to EL1 or not (x3>>2) = 1
    
    // Disable IRQ & FIQ
    mrs     x0, daif
    orr     x0, x0, #0xC0    // I=0x80, F=0x40
    msr     daif, x0
    
	// Set EL0 stack poiont
	ldr     x2, =stack_top
	bic     x2, x2, #0xF     // 確保 16-byte 對齊
	msr 	sp_el0, x2
	
    // set up exception handlers
    ldr     x0, =exception_table
    msr     VBAR_EL1, x0
    isb
	
	// Save pstate to spsr_el1 before jump to EL0
	mov 	x1, #5				// M = 0b0101 -> EL1h
	orr     x1, x1, #(1 << 6)	// bit[6]=1 Disable SError
	orr     x1, x1, #(1 << 7)	// bit[7]=1 Disable FIQ
	orr     x1, x1, #(1 << 8)	// bit[8]=1 Disable IRQ
	msr     spsr_el1, x1
	
	// address EL1 and jump to
	adr 	x0, el0_entry
	msr 	elr_el1, x0
	
	// enable uart1 interrupt
	bl      set_aux
	eret

el0_entry:
    //mrs     x3, CurrentEL  //For checking jump to EL0 or not (x3>>2) = 0
    
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
