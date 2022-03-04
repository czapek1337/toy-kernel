#pragma once

#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>

namespace mem {

  void init_pmm(stivale2_struct_tag_memmap *mmap_tag);

  uintptr_t phys_alloc(size_t pages);

  void phys_free(uintptr_t addr, size_t pages);

  struct physical_policy_t {
    uintptr_t map(size_t length);

    void unmap(uintptr_t pointer, size_t length);
  };

  inline physical_policy_t phys_policy;

} // namespace mem
