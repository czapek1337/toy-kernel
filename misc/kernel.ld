ENTRY(kernel_start)

PHDRS
{
    null PT_NULL FLAGS(0);
    text PT_LOAD FLAGS(1 << 0 | 1 << 2);
    rodata PT_LOAD FLAGS(1 << 2);
    data PT_LOAD FLAGS(1 << 1 | 1 << 2);
}

SECTIONS
{
    . = 0xffffffff80200000;

    .text :
    {
        *(.text*)
    } : text

    . += 0x1000;

    .stivale2hdr :
    {
        KEEP(*(.stivale2hdr))
    } : rodata

    .rodata :
    {
        *(.rodata*)
    } : rodata

    . += 0x1000;

    .data :
    {
        *(.data*)
    } : data

    .bss :
    {
        *(.bss*)
    } : data
}
