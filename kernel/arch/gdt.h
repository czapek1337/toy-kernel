#pragma once

#define GDT_NULL 0x0
#define GDT_CODE16 0x08
#define GDT_DATA16 0x10
#define GDT_CODE32 0x18
#define GDT_DATA32 0x20
#define GDT_CODE64 0x28
#define GDT_DATA64 0x30
#define GDT_USER_CODE64 0x38
#define GDT_USER_DATA64 0x40

void gdt_init();
