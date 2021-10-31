ENTRY(user_start)

SECTIONS
{
    . = 0x200000;

    .text :
    {
        *(.text*)
    }

    . += 0x1000;

    .rodata :
    {
        *(.rodata*)
    }

    . += 0x1000;

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
    }
}
