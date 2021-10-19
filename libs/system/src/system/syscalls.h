#pragma once

#include <stdint.h>

enum syscall_number_t : uint64_t {
    SYSCALL_TRACE,
    SYSCALL_EXIT,
    SYSCALL_YIELD,
};

namespace detail {

uint64_t make_syscall(uint64_t id, uint64_t arg1 = 0, uint64_t arg2 = 0, uint64_t arg3 = 0, uint64_t arg4 = 0, uint64_t arg5 = 0);

}

namespace system {

void trace(const char *message);
void exit(uint64_t code);
void yield();

} // namespace system
