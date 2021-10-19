#include "syscalls.h"

uint64_t detail::make_syscall(uint64_t id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    uint64_t result;

    register uint64_t r8 asm("r8") = arg4;
    register uint64_t r9 asm("r9") = arg5;

    asm volatile("syscall" : "=a"(result) : "a"(id), "b"(arg1), "d"(arg2), "S"(arg3), "r"(r8), "r"(r9));

    return result;
}

void system::trace(const char *message) {
    detail::make_syscall(SYSCALL_TRACE, (uint64_t) message);
}

void system::exit(uint64_t code) {
    detail::make_syscall(SYSCALL_EXIT, code);
}

void system::yield() {
    detail::make_syscall(SYSCALL_YIELD);
}
