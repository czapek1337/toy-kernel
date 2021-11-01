#pragma once

#include <core/hash_map.h>
#include <stdint.h>

#include "../fs/vfs.h"
#include "../intr/intr.h"
#include "../mm/vmm.h"
#include "elf.h"

struct Process;

struct Thread {
    uint64_t id;

    uint64_t exit_code;
    uint64_t syscall_kernel_stack;
    uint64_t syscall_user_stack;

    bool is_running;

    Process *process;
    Registers *registers;

    static Thread *spawn(Process *process, uint64_t entry, bool is_user);
};

struct Process {
    uint64_t id;
    uint64_t exit_code;

    uint64_t tid_counter;
    uint64_t fd_counter;

    Process *parent_process;
    Thread *main_thread;

    bool is_running;
    bool is_user;

    PageTable *pml4;

    core::String name;
    core::HashMap<uint64_t, Thread *> threads;
    core::HashMap<uint64_t, VfsOpenedFile *> file_descriptors;

    static Process *spawn(Process *parent_process, const core::String &name, uint64_t entry, bool is_user);
    static Process *spawn(Process *parent_process, const core::String &name, Elf64 *elf, bool is_user);
};
