#pragma once

#include <core/hash_map.h>
#include <core/string.h>
#include <stdint.h>

#include "../fs/vfs.h"
#include "../intr/intr.h"
#include "../mm/vmm.h"

struct Task {
    uint64_t exit_code;
    uint64_t ticks_since_schedule;

    uint64_t syscall_kernel_stack;
    uint64_t syscall_user_stack;
    uint64_t fd_counter;

    bool is_running;
    bool is_user;

    PageTable *pml4;
    Registers regs;

    core::String name;
    core::HashMap<uint64_t, VfsOpenedFile *> open_fds;

    Task();
    ~Task();

    void save(Registers *regs);
    void load(Registers *regs);
    void kill(uint64_t exit_code);
};
