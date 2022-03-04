#pragma once

#include <stddef.h>
#include <stdint.h>

#include <smarter.hpp>

#include <interrupts/interrupts.h>
#include <proc/thread.h>

namespace scheduler {

  void init();
  void schedule(smarter::shared_ptr<proc::thread_t> thread);
  void preempt(interrupts::isr_frame_t *frame);

} // namespace scheduler
