SECTIONS {
    . = 0x80000;

    .text : {
        *(.text._start)
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
	__bss_start = .;
        *(.bss*)
        *(COMMON)
	__bss_end = .;
    }
}
