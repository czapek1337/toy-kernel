#include <utils/print.h>
#include <utils/spin.h>

void spin_lock(spin_lock_t *lock) {
  for (;;) {
    if (spin_try_lock(lock))
      break;

    asm("pause" ::: "memory");
  }
}

void spin_unlock(spin_lock_t *lock) {
  assert(__sync_bool_compare_and_swap(lock, SPIN_LOCKED, SPIN_UNLOCKED));
}

bool spin_try_lock(spin_lock_t *lock) {
  if (__sync_bool_compare_and_swap(lock, SPIN_UNLOCKED, SPIN_LOCKED))
    return true;

  return false;
}
