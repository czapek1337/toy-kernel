#include <string.h>

#include "../utils/ll.h"
#include "../utils/print.h"
#include "../utils/spin.h"
#include "../utils/utils.h"
#include "heap.h"
#include "phys.h"
#include "virt.h"

typedef struct {
  ll_node_t node;
  size_t size;
  uint8_t data[];
} heap_node_t;

static spin_lock_t heap_lock;
static ll_node_t heap_free_list = {
  .prev = &heap_free_list,
  .next = &heap_free_list,
};

#define ALLOC_HEADER_SIZE offset_of(heap_node_t, data)
#define MIN_ALLOC_SIZE ALLOC_HEADER_SIZE + 32

static void defragment_heap() {
  heap_node_t *block = NULL;
  heap_node_t *last_block = NULL;
  heap_node_t *temp = NULL;

  ll_for_each_entry_safe(block, temp, &heap_free_list, node) {
    if (last_block && (uintptr_t) last_block->data + last_block->size == (uintptr_t) block) {
      last_block->size += sizeof(*block) + block->size;
      ll_delete(&block->node);
      continue;
    }

    last_block = block;
  }
}

static void add_block(heap_node_t *block, size_t size) {
  block->size = (uintptr_t) block + size - (uintptr_t) block - ALLOC_HEADER_SIZE;

  spin_lock(&heap_lock);

  ll_add_front(&block->node, &heap_free_list);

  spin_unlock(&heap_lock);
}

void heap_init() {
  size_t block_size = 2;
  size_t block_count = 128;

  for (size_t i = 0; i < block_count; i++) {
    paddr_t page = phys_alloc(block_size);

    add_block((heap_node_t *) phys_to_virt(page), 0x1000 * block_size);
  }

  klog_info("Initialized heap with %dKiB of memory", (block_size * block_count * 0x1000) / 1024);
}

void heap_free(void *pointer) {
  assert_msg(pointer, "Tried freeing a null pointer");

  heap_node_t *block = container_of(pointer, heap_node_t, data);
  heap_node_t *free_block = NULL;

  spin_lock(&heap_lock);

  ll_for_each_entry(free_block, &heap_free_list, node) {
    if (free_block > block) {
      ll_insert(&block->node, free_block->node.prev, &free_block->node);
      goto end;
    }
  }

  ll_add_back(&block->node, &heap_free_list);

end:
  defragment_heap();

  spin_unlock(&heap_lock);
}

void *heap_alloc(size_t size) {
  void *pointer = NULL;

  if (size == 0)
    return pointer;

  size = ALIGN_UP(size, sizeof(void));

  spin_lock(&heap_lock);

  heap_node_t *block = NULL;

  ll_for_each_entry(block, &heap_free_list, node) {
    if (block->size >= size) {
      pointer = block->data;
      break;
    }
  }

  if (!pointer) {
    spin_unlock(&heap_lock);
    return pointer;
  }

  if (block->size - size >= MIN_ALLOC_SIZE) {
    heap_node_t *new_block = (heap_node_t *) ((uintptr_t) block->data + size);

    new_block->size = block->size - size - ALLOC_HEADER_SIZE;
    block->size = size;

    ll_insert(&new_block->node, &block->node, block->node.next);
  }

  ll_delete(&block->node);

  spin_unlock(&heap_lock);

  return pointer;
}

void *heap_alloc_zero(size_t size) {
  void *allocation = heap_alloc(size);

  if (allocation)
    memset(allocation, 0, size);

  return allocation;
}
