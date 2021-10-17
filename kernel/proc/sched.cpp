#include "sched.h"
#include "../ds/vector.h"
#include "../lib/log.h"

static vector_t<task_t *> tasks;
static uint64_t current_task_idx;

void task::init_sched() {
    current_task_idx = -1;

    // TODO: Init scheduler
}

void task::register_task(task_t *task) {
    log_info("Registered a task located at {#016x}", task);

    tasks.push(task);
}

void task::reschedule() {
    current_task_idx = (current_task_idx + 1) % tasks.size();

    // TODO: A more sophisticated rescheduling logic lol
}

task_t *task::get_current_task() {
    if (tasks.size() == 0 || current_task_idx == -1)
        return nullptr;

    return tasks[current_task_idx];
}
