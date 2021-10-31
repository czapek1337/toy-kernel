#include <plugs/plugs.h>

#include "lock.h"

bool core::SpinLock::try_lock() {
    plugs::enter_critical_section();

    if (__sync_bool_compare_and_swap(&m_value, 0, 1))
        return true;

    plugs::leave_critical_section();

    return false;
}

void core::SpinLock::lock() {
    while (true) {
        if (try_lock())
            break;

        plugs::spinlock_wait();
    }
}

void core::SpinLock::unlock() {
    __sync_bool_compare_and_swap(&m_value, 1, 0);

    plugs::leave_critical_section();
}
