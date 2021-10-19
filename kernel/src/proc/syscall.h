#pragma once

#include <stdint.h>

namespace syscall {

void init();
void set_gs_base(uint64_t user_gs);

} // namespace syscall
