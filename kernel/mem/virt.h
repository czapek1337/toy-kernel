#pragma once

#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <mem/phys.h>

#define PTE_P 1ul << 0
#define PTE_W 1ul << 1
#define PTE_U 1ul << 2
#define PTE_WT 1ul << 3
#define PTE_NC 1ul << 4
#define PTE_G 1ul << 8
#define PTE_NX 1ul << 63
#define PTE_ADDR_MASK ~0xffful

using vaddr_t = uint64_t;

namespace mem {

  struct page_table_t {
    uint64_t entries[512];

    void map(vaddr_t virt, paddr_t phys, size_t size, uint64_t flags);
    void unmap(vaddr_t virt, size_t size);
  };

  struct address_space_t {
    page_table_t *pt;

    void map(vaddr_t virt, paddr_t phys, size_t size, uint64_t flags);
    void unmap(vaddr_t virt, size_t size);
    void switch_to();
  };

  address_space_t *new_vm();

  void destroy_vm(address_space_t *vm);

  void init_paging(stivale2_struct_tag_pmrs *pmrs_tag,                       //
                   stivale2_struct_tag_kernel_base_address *kernel_base_tag, //
                   stivale2_struct_tag_hhdm *hhdm_tag);

  vaddr_t phys_to_virt(paddr_t phys);

} // namespace mem
