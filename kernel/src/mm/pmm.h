#pragma once

#include <stdint.h>

#include "../boot/stivale2.h"

namespace pmm {

void init(Stivale2StructMemoryMapTag *mmap);

uint64_t alloc(uint64_t count);

void free(uint64_t addr, uint64_t count);

} // namespace pmm
