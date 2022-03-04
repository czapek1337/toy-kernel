#include <mem/virt.h>

void *operator new(size_t size) {
  return mem::kernel_allocator.allocate(size);
}

void *operator new[](size_t size) {
  return mem::kernel_allocator.allocate(size);
}

void operator delete(void *pointer) {
  mem::kernel_allocator.free(pointer);
}

void operator delete(void *pointer, [[maybe_unused]] size_t size) {
  mem::kernel_allocator.free(pointer);
}

void operator delete[](void *pointer) {
  mem::kernel_allocator.free(pointer);
}

void operator delete[](void *pointer, [[maybe_unused]] size_t size) {
  mem::kernel_allocator.free(pointer);
}
