#pragma once

#include <stddef.h>
#include <stdint.h>

namespace apic {

  void init();

  void send_eoi();
  void send_ipi(size_t lapic_id, uint64_t id);

  size_t get_lapic();

} // namespace apic
