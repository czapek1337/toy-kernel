#include <stdbool.h>
#include <string.h>

#include "../utils/print.h"
#include "../utils/spin.h"
#include "../utils/utils.h"
#include "phys.h"

static uint8_t *bitmap_data;
static size_t bitmap_length;
static size_t bitmap_allocation_hint;
static spin_lock_t pmm_lock;

static void bitmap_set(size_t bit, bool state) {
  size_t index = bit / 8;
  size_t mask = BIT_MASK(bit % 8);

  assert_msg(index < bitmap_length, "Tried to index outside bitmap bounds (%d >= %d)", index, bitmap_length);

  if (state)
    bitmap_data[index] |= mask;
  else
    bitmap_data[index] &= ~mask;
}

static void bitmap_set_range(size_t start, size_t count, bool state) {
  for (size_t i = 0; i < count; i++) {
    bitmap_set(start + i, state);
  }
}

static uint64_t bitmap_find_unused(size_t start, size_t bits) {
  for (size_t i = start; i < bitmap_length * 8; i++) {
    bool found = true;

    for (size_t j = 0; found && j < bits; j++) {
      size_t bit = i + j;
      size_t index = bit / 8;
      size_t mask = BIT_MASK(bit % 8);

      if (IS_SET(bitmap_data[index], mask))
        found = false;
    }

    if (found)
      return i;
  }

  return (size_t) -1;
}

void phys_init(struct stivale2_struct_tag_memmap *memmap_tag) {
  paddr_t max_address = 0;

  for (size_t i = 0; i < memmap_tag->entries; i++) {
    struct stivale2_mmap_entry *entry = &memmap_tag->memmap[i];

    klog_info("Memory map: address=%p, length=%p, type=%d", entry->base, entry->length, entry->type);

    if (entry->type != STIVALE2_MMAP_USABLE)
      continue;

    if (entry->base + entry->length > max_address)
      max_address = entry->base + entry->length;
  }

  size_t bitmap_size = ALIGN_UP(max_address, 4096) / 4096 / 8 + 1;

  struct stivale2_mmap_entry *bitmap_entry = NULL;

  for (size_t i = 0; !bitmap_entry && i < memmap_tag->entries; i++) {
    struct stivale2_mmap_entry *entry = &memmap_tag->memmap[i];

    if (entry->type == STIVALE2_MMAP_USABLE && entry->length > bitmap_size)
      bitmap_entry = entry;
  }

  assert_msg(bitmap_entry, "Could not find a suitable place in memory to host the bitmap");

  bitmap_data = (uint8_t *) bitmap_entry->base;
  bitmap_length = bitmap_size;
  bitmap_allocation_hint = 0;

  memset(bitmap_data, 0xff, bitmap_length);

  for (size_t i = 0; i < memmap_tag->entries; i++) {
    struct stivale2_mmap_entry *entry = &memmap_tag->memmap[i];

    if (entry->type != STIVALE2_MMAP_USABLE)
      continue;

    bitmap_set_range(entry->base / 4096, ALIGN_UP(entry->length, 4096) / 4096, false);
  }

  bitmap_set_range(0, 4, true);
  bitmap_set_range(bitmap_entry->base / 4096, ALIGN_UP(bitmap_size / 4096, 4096), true);
}

void phys_free(paddr_t addr, size_t pages) {
  assert_msg((addr & 0xfff) == 0, "Attempt to free an unaligned physical address");

  spin_lock(&pmm_lock);

  bitmap_set_range(addr / 4096, pages, false);

  spin_unlock(&pmm_lock);
}

paddr_t phys_alloc(size_t pages) {
  spin_lock(&pmm_lock);

  size_t allocation = bitmap_find_unused(bitmap_allocation_hint, pages);

  try:
    if (allocation != (size_t) -1) {
      bitmap_allocation_hint = allocation + pages;

      bitmap_set_range(allocation, pages, true);
      spin_unlock(&pmm_lock);

      memset((void *) (allocation * 4096), 0, pages * 4096);

      return allocation * 4096;
    }

  if (bitmap_allocation_hint != 0) {
    bitmap_allocation_hint = 0;
    allocation = bitmap_find_unused(0, pages);

    goto try;
  }

  klog_panic("Failed to allocate %d pages of physical memory", pages);
}
