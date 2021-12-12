#include <stdint.h>

extern "C" void *memcpy(uint8_t *dest, const uint8_t *src, uint64_t size) {
    asm("rep movsb" : : "S"(src), "D"(dest), "c"(size) : "memory");

    return dest;
}

extern "C" void *memset(uint8_t *dest, uint8_t value, uint64_t size) {
    asm("rep stosb" : "=D"(dest), "=c"(size) : "0"(dest), "1"(size), "a"(value) : "memory");

    return dest;
}
