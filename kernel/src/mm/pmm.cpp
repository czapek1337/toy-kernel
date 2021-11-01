#include <core/lock.h>

#include "../ds/bitmap.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "pmm.h"

static core::SpinLock pmm_lock;
static Bitmap pmm_bitmap;
static uint64_t best_bet;

static uint64_t try_allocate_pages(uint64_t count) {
    auto addr = pmm_bitmap.find_first_range(best_bet, count, true);

    if (addr != -1) {
        pmm_bitmap.set_range(addr, count, false);
        best_bet = addr + count;

        return addr * 4096;
    }

    if (best_bet != 0) {
        best_bet = 0;

        return try_allocate_pages(count);
    }

    return ~0;
}

const char *mmap_type_to_string(uint32_t type) {
    switch (type) {
    case STIVALE2_MMAP_USABLE: return "USABLE";
    case STIVALE2_MMAP_RESERVED: return "RESERVED";
    case STIVALE2_MMAP_ACPI_RECLAIMABLE: return "ACPI_RECLAIMABLE";
    case STIVALE2_MMAP_ACPI_NVS: return "ACPI_NVS";
    case STIVALE2_MMAP_BAD_MEMORY: return "BAD_MEMORY";
    case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE: return "BOOTLOADER_RECLAIMABLE";
    case STIVALE2_MMAP_KERNEL_AND_MODULES: return "KERNEL_AND_MODULES";
    case STIVALE2_MMAP_FRAMEBUFFER: return "FRAMEBUFFER";
    }

    return "UNKNOWN";
}

void pmm::init(Stivale2StructMemoryMapTag *mmap) {
    log_info("Current system memory map:");

    for (auto i = 0; i < mmap->count; i++) {
        auto entry = &mmap->mmap[i];

        log_info("  base={#016x}, size={#08x}, type={}", entry->base, entry->size, mmap_type_to_string(entry->type));
    }

    auto highest_addr = 0ul;

    for (auto i = 0; i < mmap->count; i++) {
        auto entry = &mmap->mmap[i];
        auto end_addr = entry->base + entry->size;

        if (entry->type == STIVALE2_MMAP_USABLE && end_addr > highest_addr)
            highest_addr = end_addr;
    }

    auto bitmap_size = highest_addr / 4096 / 8;
    auto bitmap_addr = 0ul;

    for (auto i = 0; bitmap_addr == 0 && i < mmap->count; i++) {
        auto entry = &mmap->mmap[i];

        if (entry->type == STIVALE2_MMAP_USABLE && entry->size >= bitmap_size)
            bitmap_addr = entry->base;
    }

    if (bitmap_addr == 0)
        log_fatal("Could not find a suitable memory region for the bitmap of size of {} bytes", bitmap_size);

    pmm_bitmap = Bitmap((uint8_t *) phys_to_io(bitmap_addr), bitmap_size);
    pmm_bitmap.set_range(0, bitmap_size * 8, false);

    for (auto i = 0; i < mmap->count; i++) {
        auto entry = &mmap->mmap[i];

        if (entry->type == STIVALE2_MMAP_USABLE)
            pmm_bitmap.set_range(entry->base / 4096, entry->size / 4096, true);
    }

    pmm_bitmap.set_range(0, 4, false);
    pmm_bitmap.set_range(bitmap_addr / 4096, align_up(bitmap_size, 4096) / 4096, false);
}

uint64_t pmm::alloc(uint64_t count) {
    core::LockGuard lock(pmm_lock);

    auto result = try_allocate_pages(count);

    if (result != ~1)
        return result;

    if (count == 1)
        log_fatal("Could not allocate a single page");

    log_fatal("Could not allocate {} pages, if you don't need contigous memory, try allocating them one by one", count);
}

void pmm::free(uint64_t addr, uint64_t count) {
    core::LockGuard lock(pmm_lock);

    pmm_bitmap.set_range(addr / 4096, count, true);
}
