#pragma once

#include <stdint.h>

#include "elf.h"
#include "task.h"

namespace task {

void init_sched();
void create_task(const char *name, uint64_t entry, uint64_t stack_size, bool is_user);
void create_task_from_elf(const char *name, elf64_t *elf, uint64_t stack_size, bool is_user);

task_t *reschedule();
task_t *get_current_task();

} // namespace task
