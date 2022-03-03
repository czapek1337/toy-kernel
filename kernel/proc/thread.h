#pragma once

#include <stddef.h>
#include <stdint.h>

#include <interrupts/interrupts.h>
#include <mem/virt.h>
#include <utils/elf.h>

namespace proc {

  struct process_t;
  struct thread_t;

  struct thread_t {
    size_t tid;
    size_t exit_code;

    mem::address_space_t *vm;
    interrupts::isr_frame_t regs;
  };

  thread_t *create_thread(vaddr_t entry, bool is_user);
  thread_t *create_thread(elf64_header_t *elf, bool is_user);

} // namespace proc
