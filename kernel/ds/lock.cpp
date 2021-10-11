#include "lock.h"

bool lock_t::try_lock() {
    asm("cli");

    if (__sync_bool_compare_and_swap(&m_value, 0, 1))
        return true;

    asm("sti");

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

    asm("cli");
}
