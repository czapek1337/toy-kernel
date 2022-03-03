#pragma once

#define noreturn __attribute__((noreturn))
#define packed __attribute__((packed))
#define aligned(x) __attribute__((aligned(x)))

#define ALIGN_DOWN(value, align) ((value) & ~((align) - (1)))
#define ALIGN_UP(value, align) ALIGN_DOWN((value) + (align) - (1), align)

#define BIT_MASK(x) (1 << (x))
#define IS_SET(x, mask) (((x) & (mask)) == (mask))
#define IS_CLEAR(x, mask) (((x) & (mask)) == 0)

#define KIB(x) (x * 1024ul)
#define MIB(x) (KIB(x) * 1024ul)
#define GIB(x) (MIB(x) * 1024ul)
#define TIB(x) (GIB(x) * 1024ul)
