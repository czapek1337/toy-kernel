#pragma once

#include <stdint.h>

enum SyscallNumber {
    SYSCALL_TRACE,
    SYSCALL_EXIT,
    SYSCALL_YIELD,
    SYSCALL_OPEN,
    SYSCALL_CLOSE,
    SYSCALL_READ,
    SYSCALL_WRITE,
};

namespace system {

void trace(const char *message);
void exit(uint64_t code);
void yield();

uint64_t open(const char *path);
uint64_t close(uint64_t fd);
uint64_t read(uint64_t fd, uint8_t *buffer, uint64_t size);
uint64_t write(uint64_t fd, const uint8_t *buffer, uint64_t size);

} // namespace system
