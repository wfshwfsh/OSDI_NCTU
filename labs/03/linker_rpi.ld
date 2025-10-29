SECTIONS
{
    . = 0x80000;
    _boot_start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
		. = ALIGN(16);
	}
	
	.stack (NOLOAD) :{
		. = ALIGN(16);
		stack_bottom = .;
		. += 0x4000;         /* 比原本 4096 再多一點 stack 空間 */
		. = ALIGN(16);
		stack_top = .;
	}
    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;
