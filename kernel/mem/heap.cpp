#include <string.h>

#include <mem/heap.h>
#include <mem/phys.h>
#include <mem/virt.h>
#include <utils/linked_list.h>
#include <utils/print.h>
#include <utils/spin.h>
#include <utils/utils.h>

constexpr static auto heap_blocks = 8;
constexpr static auto heap_block_size = KIB(32) / KIB(4);

struct heap_node_t : utils::linked_list_node_t<heap_node_t> {
  size_t size;
  uint8_t data[];
};

static utils::spin_lock_t heap_lock;
static heap_node_t heap_root;

static void add_block(heap_node_t *block, size_t size) {
  block->size = size - sizeof(heap_node_t);
  block->insert_front(&heap_root);
}

void mem::init_heap() {
  heap_root.insert(&heap_root, &heap_root);

  for (auto i = 0u; i < heap_blocks; i++) {
    auto block_phys = phys_alloc(heap_block_size);

    add_block((heap_node_t *) phys_to_virt(block_phys), heap_block_size * KIB(4));
  }

  klog_info("Initialized heap with %dKiB of memory", (heap_block_size * heap_blocks * KIB(4)) / KIB(1));
}

void *mem::heap_alloc([[maybe_unused]] size_t size) {
  utils::spin_lock_guard_t lock(heap_lock);

  size = ALIGN_UP(size, sizeof(void *));

allocate:
  for (auto block = heap_root.next(); block != &heap_root; block = block->next()) {
    if (block->size < size)
      continue;

    if (auto space_left = block->size - size; space_left >= sizeof(heap_node_t) + 32) {
      auto new_block = (heap_node_t *) ((vaddr_t) block + sizeof(heap_node_t) + size);

      new_block->size = block->size - size - sizeof(heap_node_t);
      new_block->insert(block, block->next());

      block->size = size;
    }

    block->remove();

    return (void *) &block->data;
  }

  auto block_phys = phys_alloc(8);

  add_block((heap_node_t *) phys_to_virt(block_phys), heap_block_size * KIB(4));

  goto allocate;
}

void *mem::heap_alloc_zero(size_t size) {
  auto alloc = heap_alloc(size);

  if (alloc)
    memset(alloc, 0, size);

  return alloc;
}

void mem::heap_free([[maybe_unused]] void *pointer) {
  assert_msg(pointer, "Tried freeing a null pointer");

  utils::spin_lock_guard_t lock(heap_lock);

  auto block = (heap_node_t *) ((vaddr_t) pointer - sizeof(heap_node_t));
  auto add_to_the_tail = true;

  for (auto it_block = heap_root.next(); add_to_the_tail && it_block != &heap_root; it_block = it_block->next()) {
    if (it_block > block) {
      block->insert(it_block->prev(), it_block);
      add_to_the_tail = false;
    }
  }

  if (add_to_the_tail) {
    block->insert_back(&heap_root);
    return;
  }

  for (auto it = heap_root.next(), last = (heap_node_t *) nullptr, next = it->next(); it != &heap_root; it = next, next = it->next()) {
    auto previous_data = (vaddr_t) &it->data;

    if (last && previous_data + last->size == (vaddr_t) it) {
      it->size += sizeof(heap_node_t) + it->size;
      it->remove();

      continue;
    }

    last = it;
  }
}

void *operator new(size_t size) {
  return mem::heap_alloc(size);
}

void *operator new[](size_t size) {
  return mem::heap_alloc(size);
}

void operator delete(void *pointer) {
  mem::heap_free(pointer);
}

void operator delete(void *pointer, [[maybe_unused]] size_t size) {
  mem::heap_free(pointer);
}

void operator delete[](void *pointer) {
  mem::heap_free(pointer);
}

void operator delete[](void *pointer, [[maybe_unused]] size_t size) {
  mem::heap_free(pointer);
}
