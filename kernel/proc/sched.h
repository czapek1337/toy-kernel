#pragma once

#include <stdint.h>

#include "task.h"

namespace task {

void init_sched();
void create_task(uint64_t entry, uint64_t stack_size, bool is_user);

task_t *reschedule();
task_t *get_current_task();

} // namespace task
