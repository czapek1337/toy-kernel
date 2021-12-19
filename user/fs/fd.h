#pragma once

#include <stdint.h>

#include "vfs.h"

namespace vfs::fd {

uint64_t allocate(VfsOpenedFile *file);
uint64_t duplicate(uint64_t old_fd);

void destroy(uint64_t fd);

} // namespace vfs::fd
