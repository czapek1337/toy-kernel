#include <stdint.h>

static constexpr uint64_t explode_byte(uint8_t byte) {
    auto value = (uint64_t) byte;

    return value << 56 | value << 48 | value << 40 | value << 32 | value << 24 | value << 16 | value << 8 | value;
}

uint64_t quick_memcpy(void *dest, const void *src, uint64_t size) {
    auto block_count = size / 8;

    asm volatile("rep movsq" : "=S"(src), "=D"(dest) : "S"(src), "D"(dest), "c"(block_count) : "memory");

    return size - block_count * 8;
}

uint64_t quick_memset(void *dest, uint8_t value, uint64_t size) {
    auto block_count = size / 8;

    asm volatile("rep stosq" : "=D"(dest) : "D"(dest), "c"(block_count), "a"(explode_byte(value)) : "memory");

    return size - block_count * 8;
}

extern "C" void *memcpy(void *dest, const void *src, uint64_t size) {
    if (size >= 8)
        size = quick_memcpy(dest, src, size);

    if (size > 0)
        asm volatile("rep movsb" ::"S"(src), "D"(dest), "c"(size) : "memory");

    return dest;
}

extern "C" void *memset(void *dest, uint8_t value, uint64_t size) {
    if (size >= 8)
        size = quick_memset(dest, value, size);

    if (size > 0) {
        asm volatile("rep stosb" : "=D"(dest), "=c"(size) : "0"(dest), "1"(size), "a"(value) : "memory");
    }

    return dest;
}
