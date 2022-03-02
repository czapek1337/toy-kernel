#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../boot/stivale2.h"

typedef uint64_t paddr_t;

void phys_init(struct stivale2_struct_tag_memmap *mmap_tag);
void phys_free(paddr_t addr, size_t pages);

paddr_t phys_alloc(size_t pages);
