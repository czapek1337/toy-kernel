#pragma once

#include <core/string.h>
#include <stdint.h>

#include "elf.h"
#include "process.h"

namespace sched {

void init();
void queue(Thread *thread);
void dequeue(Thread *thread);
void reschedule(Registers *regs);

Thread *get_current_thread();

} // namespace sched
