#include "lock.h"
#include "../arch/asm.h"
#include "../intr/intr.h"

bool lock_t::try_lock() {
    intr::retain();

    if (__sync_bool_compare_and_swap(&m_value, 0, 1))
        return true;

    intr::release();

    return false;
}

void lock_t::lock() {
    while (true) {
        if (try_lock())
            break;

        arch::pause();
    }
}

void lock_t::unlock() {
    __sync_bool_compare_and_swap(&m_value, 1, 0);

    intr::release();
}
