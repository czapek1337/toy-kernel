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

task_t *task::reschedule() {
    // TODO: A more sophisticated rescheduling logic lol

    auto current_task = get_current_task();

    if (current_task && ++current_task->ticks_since_schedule < 3)
        return current_task;

    current_task_idx = (current_task_idx + 1) % tasks.size();

    auto new_task = get_current_task();

    new_task->ticks_since_schedule = 0;

    return new_task;
}

task_t *task::get_current_task() {
    if (tasks.size() == 0 || current_task_idx == -1)
        return nullptr;

    return tasks[current_task_idx];
}
