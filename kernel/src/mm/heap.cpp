#include <core/lock.h>

#include "../ds/linked_list.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "heap.h"
#include "pmm.h"
#include "vmm.h"

// This is an adapted version of Embedded Artistry's malloc implementation that's contained in
// their libmemory repository: https://github.com/embeddedartistry/libmemory/

struct HeapBlock : LinkedListNode<HeapBlock> {
    uint64_t size;
};

constexpr auto heap_base_offset = gib(8);
constexpr auto heap_initial_size = kib(64);

static core::SpinLock heap_lock;
static HeapBlock heap_root;

static HeapBlock *create_block(uint64_t size) {
    auto pages = align_up(size, 4096) / 4096;

    if (pages < 8)
        pages = 8;

    auto block_phys = pmm::alloc(pages);
    auto block = (HeapBlock *) phys_to_io(heap_base_offset + block_phys);

    vmm::kernel_pml4->map((uint64_t) block, block_phys, pages * 4096, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE);

    block->size = pages * 4096 - sizeof(HeapBlock);

    return block;
}

void heap::init() {
    heap_root.insert(&heap_root, &heap_root);

    auto new_block = create_block(heap_initial_size);

    new_block->insert_front(&heap_root);

    log_info("Successfully initialized the heap with {}KiB of memory", heap_initial_size / kib(1));
}

uint64_t heap::alloc(uint64_t size) {
    assert_msg(size > 0, "Tried make an allocation of size 0");

    // Acquire the heap lock, we do it manually here, instead of using the LockGuard
    // construct to avoid recursive locking in case when we need to recursively call
    // into alloc after we allocate some more blocks
    heap_lock.lock();

    // Align the size to 8 bytes
    size = align_up(size, 8);

    // Try to make an allocation from existing blocks
    for (auto block = heap_root.next(); block != &heap_root; block = block->next()) {
        // Check if the block is big enough to hold our allocation
        if (block->size < size)
            continue;

        auto space_left = block->size - size;

        if (space_left >= sizeof(HeapBlock) + 32) {
            auto new_block = (HeapBlock *) ((uint64_t) block + sizeof(HeapBlock) + size);

            new_block->size = block->size - size - sizeof(HeapBlock);

            block->size = size;

            new_block->insert(block, block->next());
        }

        block->remove();

        heap_lock.unlock();

        return (uint64_t) block + sizeof(HeapBlock);
    }

    // Allocate a fresh block of heap memory for the next run
    auto new_block = create_block(size + sizeof(HeapBlock));

    new_block->insert_front(&heap_root);

    heap_lock.unlock();

    // Try again, hopefully successfully this time
    return alloc(size);
}

void heap::free(uint64_t addr) {
    assert_msg(addr != 0, "Tried freeing a null pointer");

    // Acquire the heap lock
    core::LockGuard lock(heap_lock);

    // Lets figure out which block this allocation belongs to
    auto block = (HeapBlock *) (addr - sizeof(HeapBlock));
    auto add_to_the_tail = true;

    // Lets put that block back in its correct spot in the free list
    for (auto it_block = heap_root.next(); add_to_the_tail && it_block != &heap_root; it_block = it_block->next()) {
        // Check if the block we're attempting to free comes before the current block
        if (it_block > block) {
            block->insert(it_block->prev(), it_block);

            add_to_the_tail = false;
        }
    }

    // If the block could not be put back into its proper place, put it
    // back at the heap's tail
    if (add_to_the_tail) {
        block->insert_back(&heap_root);
    }
    // Else, merge any adjacent blocks to reduce fragmentation
    else {
        HeapBlock *last_block = nullptr;

        for (auto it_block = heap_root.next(), next = it_block->next(); it_block != &heap_root; it_block = next, next = it_block->next()) {
            auto previous_data = (uint64_t) last_block + sizeof(HeapBlock);

            if (last_block && previous_data + last_block->size == (uint64_t) it_block) {
                last_block->size += sizeof(HeapBlock) + it_block->size;

                it_block->remove();

                continue;
            }

            last_block = it_block;
        }
    }
}

void *operator new(uint64_t size) {
    return (void *) heap::alloc(size);
}

void *operator new[](uint64_t size) {
    return (void *) heap::alloc(size);
}

void operator delete(void *ptr) {
    heap::free((uint64_t) ptr);
}

void operator delete[](void *ptr) {
    heap::free((uint64_t) ptr);
}
