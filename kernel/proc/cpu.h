#pragma once

#include <stddef.h>
#include <stdint.h>

#include <smarter.hpp>

#include <arch/gdt.h>
#include <proc/thread.h>

namespace cpu {

  struct cpu_info_t {
    cpu_info_t *self;

    size_t id;
    size_t lapic_id;

    arch::gdt_t gdt;
    arch::tss_t tss;

    smarter::shared_ptr<proc::thread_t> thread;
  };

  void init_bsp();
  void init_ap(size_t id, size_t lapic_id);

  cpu_info_t *get();

} // namespace cpu
