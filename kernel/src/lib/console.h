#pragma once

#include "../boot/stivale2.h"

namespace console {

void init(Stivale2StructFramebufferTag *framebuffer);
void write(char ch);

} // namespace console
