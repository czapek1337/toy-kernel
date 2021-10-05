#include "heap.h"
#include "../lib/addr.h"
#include "pmm.h"
#include "vmm.h"

struct minor_block_t;

struct major_block_t {
    uint64_t size;
    uint64_t used;

    major_block_t *prev;
    major_block_t *next;
    minor_block_t *child;
};

struct minor_block_t {
    uint64_t size;

    major_block_t *parent;
    minor_block_t *prev;
    minor_block_t *next;
};

uint64_t heap::alloc(uint64_t size) {
    return 0;
}

void heap::free(uint64_t addr) {}
