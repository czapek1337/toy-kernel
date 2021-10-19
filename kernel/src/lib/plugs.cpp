#include <plugs/plugs.h>

#include "../arch/asm.h"
#include "../intr/intr.h"

void plugs::enter_critical_section() {
    intr::retain();
}

void plugs::leave_critical_section() {
    intr::release();
}

void plugs::spinlock_wait() {
    arch::pause();
}
