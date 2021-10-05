#pragma once

#include <stdint.h>

#include "../boot/stivale2.h"

namespace pmm {

void init(stivale2_struct_mmap_tag_t *mmap);

uint64_t alloc(uint64_t count);

void free(uint64_t addr, uint64_t count);

} // namespace pmm
