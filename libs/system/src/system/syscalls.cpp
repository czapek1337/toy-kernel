#include "syscalls.h"

inline uint64_t make_syscall(uint64_t id, uint64_t arg1 = 0, uint64_t arg2 = 0, uint64_t arg3 = 0, uint64_t arg4 = 0, uint64_t arg5 = 0) {
    uint64_t result;

    register uint64_t arg4_reg asm("r10") = arg4;
    register uint64_t arg5_reg asm("r8") = arg5;

    asm volatile("syscall"
                 : "=a"(result)
                 : "a"(id), "b"(arg1), "d"(arg2), "S"(arg3), "r"(arg4_reg), "r"(arg5_reg)
                 : "rcx", "r11", "memory");

    return result;
}

void system::trace(const char *message) {
    make_syscall(SYSCALL_TRACE, (uint64_t) message);
}

void system::exit(uint64_t code) {
    make_syscall(SYSCALL_EXIT, code);
}

void system::yield() {
    make_syscall(SYSCALL_YIELD);
}

uint64_t system::open(const char *path) {
    return make_syscall(SYSCALL_OPEN, (uint64_t) path);
}

uint64_t system::close(uint64_t fd) {
    return make_syscall(SYSCALL_CLOSE, fd);
}

uint64_t system::read(uint64_t fd, uint8_t *buffer, uint64_t size) {
    return make_syscall(SYSCALL_READ, fd, (uint64_t) buffer, size);
}

uint64_t system::write(uint64_t fd, const uint8_t *buffer, uint64_t size) {
    return make_syscall(SYSCALL_WRITE, fd, (uint64_t) buffer, size);
}
