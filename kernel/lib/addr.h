#pragma once

#include <stdint.h>

constexpr uint64_t phys_to_io(uint64_t addr) {
    return addr + 0xffff800000000000;
}

constexpr uint64_t phys_to_kernel(uint64_t addr) {
    return addr + 0xffffffff80000000;
}

constexpr uint64_t io_to_phys(uint64_t addr) {
    return addr - 0xffff800000000000;
}

constexpr uint64_t kernel_to_phys(uint64_t addr) {
    return addr - 0xffffffff80000000;
}

constexpr uint64_t align_down(uint64_t addr, uint64_t align) {
    return addr & ~(align - 1);
}

constexpr uint64_t align_up(uint64_t addr, uint64_t align) {
    return align_down(addr + align - 1, align);
}

constexpr uint64_t kib(uint64_t count) {
    return count * 1024;
}

constexpr uint64_t mib(uint64_t count) {
    return kib(count) * 1024;
}

constexpr uint64_t gib(uint64_t count) {
    return mib(count) * 1024;
}

constexpr uint64_t tib(uint64_t count) {
    return gib(count) * 1024;
}
