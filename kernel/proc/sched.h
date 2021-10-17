#pragma once

#include <stdint.h>

#include "task.h"

namespace task {

void init_sched();
void register_task(task_t *task);

task_t *reschedule();
task_t *get_current_task();

} // namespace task
