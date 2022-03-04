#include <frg/mutex.hpp>

#include <mem/heap.h>
#include <mem/virt.h>
#include <utils/print.h>
#include <utils/utils.h>

static mem::page_table_t *kernel_pt;
static uintptr_t hhdm_offset;

static mem::page_table_t *pt_get_next(mem::page_table_t *pt, size_t index, bool create_if_missing) {
  if (pt && IS_SET(pt->entries[index], PTE_P))
    return (mem::page_table_t *) mem::phys_to_virt(pt->entries[index] & PTE_ADDR_MASK);

  if (!pt || !create_if_missing)
    return nullptr;

  pt->entries[index] = PTE_P | PTE_W | PTE_U | mem::phys_alloc(1);

  return (mem::page_table_t *) mem::phys_to_virt(pt->entries[index] & PTE_ADDR_MASK);
}

static void pt_map_page(mem::page_table_t *pml4, uintptr_t virt, uintptr_t phys, uint64_t flags) {
  auto pml3 = pt_get_next(pml4, (virt >> 39) & 0x1ff, true);
  auto pml2 = pt_get_next(pml3, (virt >> 30) & 0x1ff, true);
  auto pml1 = pt_get_next(pml2, (virt >> 21) & 0x1ff, true);

  auto pml1_index = (virt >> 12) & 0x1ff;

  if (IS_SET(pml1->entries[pml1_index], PTE_P))
    kpanic("Tried to map a virtual address 0x{:x} that's already mapped to 0x{:x}", virt, pml1->entries[pml1_index] & PTE_ADDR_MASK);

  pml1->entries[pml1_index] = flags | phys;
}

static void pt_unmap_page(mem::page_table_t *pml4, uintptr_t virt) {
  auto pml3 = pt_get_next(pml4, (virt >> 39) & 0x1ff, false);
  auto pml2 = pt_get_next(pml3, (virt >> 30) & 0x1ff, false);
  auto pml1 = pt_get_next(pml2, (virt >> 21) & 0x1ff, false);

  auto pml1_index = (virt >> 12) & 0x1ff;

  if (!pml1 || IS_CLEAR(pml1->entries[pml1_index], PTE_P))
    kpanic("Tried to unmap a virtual address 0x{:x} that isn't mapped", virt);

  pml1->entries[pml1_index] = 0;

  asm("invlpg (%0)" : : "r"(virt));
}

static void pt_destroy_level(mem::page_table_t *pt, size_t level, size_t start, size_t end) {
  if (level < 1)
    return;

  for (auto i = start; i < end; i++) {
    auto next_pml = pt_get_next(pt, i, false);

    if (next_pml)
      pt_destroy_level(next_pml, level - 1, 0, 512);
  }

  mem::phys_free(mem::virt_to_phys((uintptr_t) pt), 1);
}

void mem::page_table_t::map(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags) {
  kassert_msg((virt & 0xfff) == 0, "Virtual address must be page aligned");
  kassert_msg((phys & 0xfff) == 0, "Physical address must be page aligned");
  kassert_msg((size & 0xfff) == 0, "Size must be expressed in pages");

  for (auto i = 0u; i < size / 4096; i++) {
    pt_map_page(this, virt + i * 4096, phys + i * 4096, flags);
  }
}

void mem::page_table_t::unmap(uintptr_t virt, size_t size) {
  kassert_msg((virt & 0xfff) == 0, "Virtual address must be page aligned");
  kassert_msg((size & 0xfff) == 0, "Size must be expressed in pages");

  for (auto i = 0u; i < size / 4096; i++) {
    pt_unmap_page(this, virt + i * 4096);
  }
}

mem::address_space_t::address_space_t() : m_mappings(mem::kernel_allocator) {
  m_pt = (page_table_t *) phys_to_virt(phys_alloc(1));
}

void mem::address_space_t::map(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags) {
  frg::unique_lock lock(m_lock);

  m_pt->map(virt, phys, size, flags);
}

void mem::address_space_t::unmap(uintptr_t virt, size_t size) {
  frg::unique_lock lock(m_lock);

  m_pt->unmap(virt, size);
}

void mem::address_space_t::switch_to() {
  asm("mov %0, %%cr3" : : "r"(virt_to_phys((uintptr_t) m_pt)));
}

mem::page_table_t *mem::address_space_t::page_table() {
  return m_pt;
}

void mem::destroy_vm(smarter::shared_ptr<address_space_t> vm) {
  pt_destroy_level(vm->page_table(), 4, 0, 255);
}

smarter::shared_ptr<mem::address_space_t> mem::new_vm() {
  auto vm = smarter::make_shared<address_space_t>();

  for (auto i = 256; i < 512; i++) {
    vm->page_table()->entries[i] = kernel_pt->entries[i];
  }

  return vm;
}

void mem::init_paging(stivale2_struct_tag_pmrs *pmrs_tag,                       //
                      stivale2_struct_tag_kernel_base_address *kernel_base_tag, //
                      stivale2_struct_tag_hhdm *hhdm_tag)                       //
{
  hhdm_offset = hhdm_tag->addr;
  kernel_pt = (page_table_t *) phys_to_virt(phys_alloc(1));

  kernel_pt->map(0, 0, GIB(4), PTE_P | PTE_W);
  kernel_pt->map(hhdm_offset, 0, GIB(4), PTE_P | PTE_W);

  // Unmap the null page so we can catch null accesses :^)
  kernel_pt->unmap(0, KIB(4));

  for (auto i = 0u; i < pmrs_tag->entries; i++) {
    auto pmr = &pmrs_tag->pmrs[i];
    auto map_flags = PTE_P;

    if (IS_SET(pmr->permissions, STIVALE2_PMR_WRITABLE))
      map_flags |= PTE_W;

    if (IS_CLEAR(pmr->permissions, STIVALE2_PMR_EXECUTABLE))
      map_flags |= PTE_NX;

    auto phys_addr = kernel_base_tag->physical_base_address + (pmr->base - kernel_base_tag->virtual_base_address);

    kernel_pt->map(pmr->base, phys_addr, ALIGN_UP(pmr->length, 4096), map_flags);
  }

  asm("mov %0, %%cr3" : : "r"(virt_to_phys((uintptr_t) kernel_pt)));
}

uintptr_t mem::phys_to_virt(uintptr_t phys) {
  return phys + hhdm_offset;
}

uintptr_t mem::virt_to_phys(uintptr_t virt) {
  kassert(virt >= hhdm_offset);

  return virt - hhdm_offset;
}
