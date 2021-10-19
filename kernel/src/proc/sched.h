#pragma once

#include <core/string.h>
#include <stdint.h>

#include "elf.h"
#include "task.h"

namespace task {

void init_sched();
void create_task(const core::string_t &name, uint64_t entry, uint64_t stack_size, bool is_user);
void create_task_from_elf(const core::string_t &name, elf64_t *elf, uint64_t stack_size, bool is_user);

task_t *reschedule(registers_t *regs);
task_t *get_current_task();

} // namespace task
