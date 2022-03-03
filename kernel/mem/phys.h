#pragma once

#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>

using paddr_t = uint64_t;

namespace mem {

  void init_pmm(stivale2_struct_tag_memmap *mmap_tag);

  paddr_t phys_alloc(size_t pages);

  void phys_free(paddr_t addr, size_t pages);

} // namespace mem
