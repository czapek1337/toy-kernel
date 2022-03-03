#pragma once

#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <mem/phys.h>
#include <utils/spin.h>
#include <utils/vector.h>

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

  struct mapping_t {
    vaddr_t start;
    vaddr_t end;

    uint64_t protection;
    uint64_t flags;
  };

  class address_space_t {
  public:
    address_space_t();

    void map(vaddr_t virt, paddr_t phys, size_t size, uint64_t flags);
    void unmap(vaddr_t virt, size_t size);
    void switch_to();

    page_table_t *page_table();

  private:
    page_table_t *m_pt;

    utils::vector_t<mapping_t> m_mappings;
    utils::spin_lock_t m_lock;
  };

  address_space_t *new_vm();

  void destroy_vm(address_space_t *vm);

  void init_paging(stivale2_struct_tag_pmrs *pmrs_tag,                       //
                   stivale2_struct_tag_kernel_base_address *kernel_base_tag, //
                   stivale2_struct_tag_hhdm *hhdm_tag);

  vaddr_t phys_to_virt(paddr_t phys);

} // namespace mem
