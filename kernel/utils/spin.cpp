#include <utils/print.h>
#include <utils/spin.h>

#define SPIN_UNLOCKED 0
#define SPIN_LOCKED 1

bool utils::spin_lock_t::try_lock() {
  if (__sync_bool_compare_and_swap(&m_lock, SPIN_UNLOCKED, SPIN_LOCKED))
    return true;

  return false;
}

void utils::spin_lock_t::lock() {
  for (;;) {
    if (try_lock())
      break;

    asm("pause" ::: "memory");
  }
}

void utils::spin_lock_t::unlock() {
  assert(__sync_bool_compare_and_swap(&m_lock, SPIN_LOCKED, SPIN_UNLOCKED));
}
