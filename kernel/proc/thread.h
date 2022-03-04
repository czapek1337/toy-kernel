#pragma once

#include <stddef.h>
#include <stdint.h>

#include <frg/list.hpp>
#include <smarter.hpp>

#include <interrupts/interrupts.h>
#include <mem/virt.h>
#include <utils/elf.h>

namespace scheduler {

  enum {
    TS_EXITING,
    TS_RUNNING,
    TS_WAITING,
  };

  struct thread_t {
    using hook_t = frg::intrusive_list_hook<smarter::shared_ptr<thread_t>, smarter::shared_ptr<thread_t>>;

    size_t tid;
    size_t exit_code = 0;
    size_t state = TS_RUNNING;

    smarter::shared_ptr<mem::address_space_t> vm;
    interrupts::isr_frame_t regs;

    hook_t list_hook;
  };

  smarter::shared_ptr<thread_t> create_thread(uintptr_t entry, bool is_user);
  smarter::shared_ptr<thread_t> create_thread(elf64_header_t *elf, bool is_user);

} // namespace scheduler
