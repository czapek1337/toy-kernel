#pragma once

#include <stddef.h>
#include <stdint.h>

#include <frg/spinlock.hpp>
#include <frg/vector.hpp>
#include <smarter.hpp>

#include <boot/stivale2.h>
#include <mem/heap.h>
#include <mem/phys.h>

#define PTE_P 1ul << 0
#define PTE_W 1ul << 1
#define PTE_U 1ul << 2
#define PTE_WT 1ul << 3
#define PTE_NC 1ul << 4
#define PTE_G 1ul << 8
#define PTE_NX 1ul << 63
#define PTE_ADDR_MASK ~0xffful

namespace mem {

  struct page_table_t {
    uint64_t entries[512];

    void map(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags);
    void unmap(uintptr_t virt, size_t size);
  };

  struct mapping_t {
    uintptr_t start;
    uintptr_t end;

    uint64_t protection;
    uint64_t flags;
  };

  class address_space_t {
  public:
    address_space_t();

    void map(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags);
    void unmap(uintptr_t virt, size_t size);
    void switch_to();

    page_table_t *page_table();

  private:
    page_table_t *m_pt;

    frg::vector<mapping_t, mem::kernel_allocator_t> m_mappings;
    frg::ticket_spinlock m_lock;
  };

  smarter::shared_ptr<address_space_t> new_vm();

  void destroy_vm(smarter::shared_ptr<address_space_t> vm);

  void init_paging(stivale2_struct_tag_pmrs *pmrs_tag,                       //
                   stivale2_struct_tag_kernel_base_address *kernel_base_tag, //
                   stivale2_struct_tag_hhdm *hhdm_tag);

  uintptr_t phys_to_virt(uintptr_t phys);
  uintptr_t virt_to_phys(uintptr_t virt);

} // namespace mem
