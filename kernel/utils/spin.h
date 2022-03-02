#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SPIN_UNLOCKED 0
#define SPIN_LOCKED 1

typedef uint64_t spin_lock_t;

void spin_lock(spin_lock_t *lock);
void spin_unlock(spin_lock_t *lock);

bool spin_try_lock(spin_lock_t *lock);
