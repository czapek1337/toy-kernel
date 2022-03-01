ENTRY(kernel_main)

PHDRS {
  null    PT_NULL FLAGS(0) ;              /* Null segment */
  text    PT_LOAD FLAGS(1 << 0 | 1 << 2); /* Read + execute */
  rodata  PT_LOAD FLAGS(1 << 2);          /* Read only */
  data    PT_LOAD FLAGS(1 << 1 | 1 << 2); /* Read + write */
}

SECTIONS {
  . = 0xffffffff80000000;

  .text : {
    *(.text .text.*)
  } : text

  . += CONSTANT(MAXPAGESIZE);

  .stivale2hdr : {
    KEEP(*(.stivale2hdr))
  } : rodata

  .rodata : {
    *(.rodata .rodata.*)
  } : rodata

  . += CONSTANT(MAXPAGESIZE);

  .data : {
    *(.data .data.*)
  } : data

  .bss : {
    *(.bss .bss.*)
  } : data
}
