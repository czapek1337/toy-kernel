#pragma once

#include <stddef.h>
#include <stdint.h>

#include <arch/gdt.h>

namespace proc {

  // Forward declaration
  struct thread_t;

} // namespace proc

namespace cpu {

  struct cpu_info_t {
    cpu_info_t *self;

    size_t id;
    size_t lapic_id;

    arch::gdt_t gdt;
    arch::tss_t tss;

    proc::thread_t *thread;
  };

  void init_bsp();
  void init_ap(size_t id, size_t lapic_id);

  cpu_info_t *get();

} // namespace cpu
