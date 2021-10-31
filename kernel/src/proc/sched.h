#pragma once

#include <core/string.h>
#include <stdint.h>

#include "elf.h"
#include "task.h"

namespace task {

void init_sched();
void create_task(const core::string_t &name, uint64_t entry, uint64_t stack_size, bool is_user);
void create_task_from_elf(const core::string_t &name, Elf64 *elf, uint64_t stack_size, bool is_user);

Task *reschedule(Registers *regs);
Task *get_current_task();

} // namespace task
