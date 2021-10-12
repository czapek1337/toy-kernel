#include "heap.h"
#include "../ds/lock.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "pmm.h"
#include "vmm.h"

struct heap_block_t {
    heap_block_t *prev;
    heap_block_t *next;
    uint64_t size;
};

static void block_insert(heap_block_t *entry, heap_block_t *prev, heap_block_t *next) {
    entry->next = next;
    entry->prev = prev;

    prev->next = entry;
    next->prev = entry;
}

static void block_add(heap_block_t *entry, heap_block_t *head) {
    block_insert(entry, head, head->next);
}

static void block_add_tail(heap_block_t *entry, heap_block_t *head) {
    block_insert(entry, head->prev, head);
}

static void block_remove(heap_block_t *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;

    entry->prev = nullptr;
    entry->next = nullptr;
}

constexpr auto heap_base_offset = gib(8);
constexpr auto heap_initial_size = kib(64);

static lock_t heap_lock;
static heap_block_t heap_root;

static heap_block_t *create_block(uint64_t size) {
    auto pages = align_up(size, 4096) / 4096;

    if (pages < 8)
        pages = 8;

    auto block_phys = pmm::alloc(pages);
    auto block = (heap_block_t *) phys_to_io(heap_base_offset + block_phys);

    vmm::kernel_pml4->map((uint64_t) block, block_phys, pages * 4096, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE);

    block->prev = nullptr;
    block->next = nullptr;
    block->size = pages * 4096 - sizeof(heap_block_t);

    return block;
}

void heap::init() {
    heap_root.prev = &heap_root;
    heap_root.next = &heap_root;

    block_add(create_block(heap_initial_size), &heap_root);

    log_info("Successfully initialized the heap with {}KiB of memory", heap_initial_size / kib(1));
}

uint64_t heap::alloc(uint64_t size) {
    assert_msg(size > 0, "Tried make an allocation of size 0");

    // Acquire the heap lock, we do it manually here, instead of using the lock_guard_t
    // construct to avoid recursive locking in case when we need to recursively call
    // into alloc after we allocate some more blocks
    heap_lock.lock();

    // Align the size to 8 bytes
    size = align_up(size, 8);

    // Try to make an allocation from existing blocks
    for (auto block = heap_root.next; block != &heap_root; block = block->next) {
        // Check if the block is big enough to hold our allocation
        if (block->size < size)
            continue;

        auto space_left = block->size - size;

        if (space_left >= sizeof(heap_block_t) + 32) {
            auto new_block = (heap_block_t *) ((uint64_t) block + sizeof(heap_block_t) + size);

            new_block->size = block->size - size - sizeof(heap_block_t);

            block->size = size;

            block_insert(new_block, block, block->next);
        }

        block_remove(block);

        heap_lock.unlock();

        return (uint64_t) block + sizeof(heap_block_t);
    }

    // Allocate a fresh block of heap memory for the next run
    block_add(create_block(size), &heap_root);

    heap_lock.unlock();

    // Try again, hopefully successfully this time
    return alloc(size);
}

void heap::free(uint64_t addr) {
    assert_msg(addr != 0, "Tried freeing a null pointer");

    // Acquire the heap lock
    lock_guard_t lock(heap_lock);

    // Lets figure out which block this allocation belongs to
    auto block = (heap_block_t *) (addr - sizeof(heap_block_t));
    auto add_to_the_tail = true;

    // Lets put that block back in its correct spot in the free list
    for (auto it_block = heap_root.next; add_to_the_tail && it_block != &heap_root; it_block = it_block->next) {
        // Check if the block we're attempting to free comes before the current block
        if (it_block > block) {
            block_insert(block, it_block->prev, it_block);

            add_to_the_tail = false;
        }
    }

    // If the block could not be put back into its proper place, put it
    // back at the heap's tail
    if (add_to_the_tail) {
        block_add_tail(block, &heap_root);
    }
    // Else, merge any adjacent blocks to reduce fragmentation
    else {
        heap_block_t *previous = nullptr;

        for (auto it_block = heap_root.next, next = it_block->next; it_block != &heap_root; it_block = next, next = it_block->next) {
            auto previous_data = (uint64_t) previous + sizeof(heap_block_t);

            if (previous && previous_data + previous->size == (uint64_t) it_block) {
                previous->size += sizeof(heap_block_t) + it_block->size;

                block_remove(it_block);

                continue;
            }

            previous = it_block;
        }
    }
}
