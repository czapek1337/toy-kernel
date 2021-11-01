#include <stdint.h>

extern "C" void *memcpy(uint8_t *dest, const uint8_t *src, uint64_t size) {
    if (size > 8) {
        auto qwords = size / 8;

        asm volatile("rep movsq" : : "S"(src), "D"(dest), "c"(qwords) : "memory");

        size -= qwords * 8;
        dest += qwords * 8;
        src += qwords * 8;
    }

    if (size > 0)
        asm volatile("rep movsb" : : "S"(src), "D"(dest), "c"(size) : "memory");

    return dest;
}

extern "C" void *memset(uint8_t *dest, uint8_t value, uint64_t size) {
    if (size > 0)
        asm volatile("rep stosb" : "=D"(dest), "=c"(size) : "0"(dest), "1"(size), "a"(value) : "memory");

    return dest;
}
