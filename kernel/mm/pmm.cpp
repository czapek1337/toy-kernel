#include "pmm.h"
#include "../ds/bitmap.h"
#include "../lib/addr.h"

static bitmap_t pmm_bitmap;

void pmm::init(stivale2_struct_mmap_tag_t *mmap) {
    auto highest_addr = 0ull;

    for (auto i = 0; i < mmap->count; i++) {
        auto entry = &mmap->mmap[i];
        auto end_addr = entry->base + entry->size;

        if (entry->type == STIVALE2_MMAP_USABLE && end_addr > highest_addr)
            highest_addr = end_addr;
    }

    auto bitmap_size = highest_addr / 4096 / 8;
    auto bitmap_addr = 0ull;

    for (auto i = 0; bitmap_addr == 0 && i < mmap->count; i++) {
        auto entry = &mmap->mmap[i];

        if (entry->type == STIVALE2_MMAP_USABLE && entry->size >= bitmap_size)
            bitmap_addr = entry->base;
    }

    bitmap_addr = phys_to_io(bitmap_addr);

    pmm_bitmap = bitmap_t((uint8_t *) bitmap_addr, bitmap_size);
    pmm_bitmap.set_range(0, bitmap_size * 8, false);

    for (auto i = 0; i < mmap->count; i++) {
        auto entry = &mmap->mmap[i];

        if (entry->type == STIVALE2_MMAP_USABLE)
            pmm_bitmap.set_range(entry->base / 4096, entry->size / 4096, true);
    }

    pmm_bitmap.set_range(0, 4, false);
    pmm_bitmap.set_range(io_to_phys(bitmap_addr) / 4096, bitmap_size / 4096, false);
}

uint64_t pmm::alloc(uint64_t count) {
    auto addr = pmm_bitmap.find_first_range(0, count, true);

    if (addr == -1)
        return 0;

    pmm_bitmap.set_range(addr, count, false);

    return addr * 4096;
}

void pmm::free(uint64_t addr, uint64_t count) {
    pmm_bitmap.set_range(addr / 4096, count, true);
}
