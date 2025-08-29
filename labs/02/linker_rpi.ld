ENTRY(_start)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K
  RAM   (rwx): ORIGIN = 0x00000000, LENGTH = 64K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS {
    . = 0x40080000;

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