#pragma once

#include <stddef.h>
#include <stdint.h>

namespace mem {

  void init_heap();

  void *heap_alloc(size_t size);
  void *heap_alloc_zero(size_t size);

  void heap_free(void *pointer);

} // namespace mem

void *operator new(size_t size);
void *operator new[](size_t size);

void operator delete(void *ptr);
void operator delete(void *ptr, size_t size);

void operator delete[](void *ptr);
void operator delete[](void *ptr, size_t size);
