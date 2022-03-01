#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../boot/stivale2.h"

void phys_init(struct stivale2_struct_tag_memmap *memmap_tag);
void phys_free(uint64_t addr, size_t pages);

uint64_t phys_alloc(size_t pages);
