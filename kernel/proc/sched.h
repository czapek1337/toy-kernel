#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../interrupts/interrupts.h"
#include "thread.h"

void sched_init();
void sched_push(thread_t *thread);
void sched_preempt(isr_frame_t *frame);
