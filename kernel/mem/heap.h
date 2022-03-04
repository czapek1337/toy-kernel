#pragma once

#include <stddef.h>
#include <stdint.h>

#include <frg/slab.hpp>
#include <frg/spinlock.hpp>

#include <mem/phys.h>

namespace mem {

  using kernel_pool_t = frg::slab_pool<physical_policy_t, frg::ticket_spinlock>;
  using kernel_allocator_t = frg::slab_allocator<physical_policy_t, frg::ticket_spinlock>;

  inline kernel_pool_t allocator_pool(phys_policy);
  inline kernel_allocator_t kernel_allocator(&allocator_pool);

} // namespace mem

void *operator new(size_t size);
void *operator new[](size_t size);

void operator delete(void *ptr);
void operator delete(void *ptr, size_t size);

void operator delete[](void *ptr);
void operator delete[](void *ptr, size_t size);
