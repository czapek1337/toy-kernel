#pragma once

#include <stdint.h>

namespace heap {

uint64_t alloc(uint64_t size);

void free(uint64_t addr);

} // namespace heap
