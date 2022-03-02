#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../boot/stivale2.h"
#include "phys.h"

#define PTE_P 1ul << 0
#define PTE_W 1ul << 1
#define PTE_U 1ul << 2
#define PTE_WT 1ul << 3
#define PTE_NC 1ul << 4
#define PTE_G 1ul << 8
#define PTE_NX 1ul << 63
#define PTE_ADDR_MASK ~0xffful

typedef uint64_t vaddr_t;

typedef struct {
  uint64_t entries[512];
} page_table_t;

typedef struct {
  page_table_t *pt;
} address_space_t;

void vm_map(address_space_t *vm, vaddr_t virt, paddr_t phys, size_t size, uint64_t flags);
void vm_unmap(address_space_t *vm, vaddr_t virt, size_t size);
void vm_switch(address_space_t *vm);

address_space_t *virt_new_vm();

void virt_init(struct stivale2_struct_tag_pmrs *pmrs_tag,                       //
               struct stivale2_struct_tag_kernel_base_address *kernel_base_tag, //
               struct stivale2_struct_tag_hhdm *hhdm_tag);
