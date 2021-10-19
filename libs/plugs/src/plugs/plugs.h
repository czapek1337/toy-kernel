#pragma once

namespace plugs {

void enter_critical_section();
void leave_critical_section();

void spinlock_wait();

} // namespace plugs
