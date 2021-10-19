SECTIONS {
    . = 0x00800000;

    .text : {
        *(.text*)
    }

    . += 0x1000;

    .rodata : {
        *(.rodata*)
    }

    . += 0x1000;

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
    }
}
