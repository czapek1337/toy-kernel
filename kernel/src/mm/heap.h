#pragma once

#include <stdint.h>

namespace heap {

void init();

uint64_t alloc(uint64_t size);

void free(uint64_t addr);

} // namespace heap

void *operator new(uint64_t size);
void *operator new[](uint64_t size);

void operator delete(void *ptr);
void operator delete[](void *ptr);
