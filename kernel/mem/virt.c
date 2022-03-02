#include <stdbool.h>

#include "../utils/print.h"
#include "../utils/spin.h"
#include "../utils/utils.h"
#include "heap.h"
#include "virt.h"

static page_table_t *kernel_pt;
static vaddr_t hhdm_offset;
static spin_lock_t vmm_lock;

static page_table_t *pt_get_next(page_table_t *pt, size_t index, bool create_if_missing) {
  if (!pt)
    return NULL;

  if (IS_SET(pt->entries[index], PTE_P))
    return (page_table_t *) (hhdm_offset + (pt->entries[index] & PTE_ADDR_MASK));

  if (!create_if_missing)
    return NULL;

  paddr_t phys_addr = phys_alloc(1);

  pt->entries[index] = PTE_P | PTE_W | PTE_U | phys_addr;

  return (page_table_t *) (hhdm_offset + phys_addr);
}

static void pt_map_single(page_table_t *pt, vaddr_t virt, paddr_t phys, uint64_t flags) {
  size_t pml4_index = (virt >> 39) & 0x1ff;
  size_t pml3_index = (virt >> 30) & 0x1ff;
  size_t pml2_index = (virt >> 21) & 0x1ff;
  size_t pml1_index = (virt >> 12) & 0x1ff;

  page_table_t *pml3 = pt_get_next(pt, pml4_index, true);
  page_table_t *pml2 = pt_get_next(pml3, pml3_index, true);
  page_table_t *pml1 = pt_get_next(pml2, pml2_index, true);

  if (IS_SET(pml1->entries[pml1_index], PTE_P))
    klog_panic("Tried to map a virtual address %p that's already mapped to %p", virt, pml1->entries[pml1_index] & PTE_ADDR_MASK);

  pml1->entries[pml1_index] = flags | phys;
}

static void pt_unmap_single(page_table_t *pt, vaddr_t virt) {
  size_t pml4_index = (virt >> 39) & 0x1ff;
  size_t pml3_index = (virt >> 30) & 0x1ff;
  size_t pml2_index = (virt >> 21) & 0x1ff;
  size_t pml1_index = (virt >> 12) & 0x1ff;

  page_table_t *pml3 = pt_get_next(pt, pml4_index, false);
  page_table_t *pml2 = pt_get_next(pml3, pml3_index, false);
  page_table_t *pml1 = pt_get_next(pml2, pml2_index, false);

  if (!pml1 || IS_CLEAR(pml1->entries[pml1_index], PTE_P))
    klog_panic("Tried to unmap a virtual address %p that isn't mapped", virt);

  pml1->entries[pml1_index] = 0;

  asm("invlpg (%0)" : : "r"(virt));
}

static void pt_destroy(page_table_t *pt, size_t level, size_t start, size_t end) {
  if (level < 1)
    return;

  for (size_t i = start; i < end; i++) {
    page_table_t *next_pml = pt_get_next(pt, i, false);

    if (next_pml)
      pt_destroy(next_pml, level - 1, 0, 512);
  }

  phys_free((paddr_t) pt - hhdm_offset, 1);
}

static void pt_map(page_table_t *pt, vaddr_t virt, paddr_t phys, size_t size, uint64_t flags) {
  assert_msg((virt & 0xfff) == 0, "Virtual address must be page aligned");
  assert_msg((phys & 0xfff) == 0, "Physical address must be page aligned");
  assert_msg((size & 0xfff) == 0, "Size must be expressed in pages");

  for (size_t i = 0; i < size / 4096; i++) {
    pt_map_single(pt, virt + i * 4096, phys + i * 4096, flags);
  }
}

static void pt_unmap(page_table_t *pt, vaddr_t virt, size_t size) {
  assert_msg((virt & 0xfff) == 0, "Virtual address must be page aligned");
  assert_msg((size & 0xfff) == 0, "Size must be expressed in pages");

  for (size_t i = 0; i < size / 4096; i++) {
    pt_unmap_single(pt, virt + i * 4096);
  }
}

void vm_destroy(address_space_t *vm) {
  pt_destroy(vm->pt, 4, 0, 255);
}

void vm_map(address_space_t *vm, vaddr_t virt, paddr_t phys, size_t size, uint64_t flags) {
  spin_lock(&vmm_lock);
  pt_map(vm->pt, virt, phys, size, flags);
  spin_unlock(&vmm_lock);
}

void vm_unmap(address_space_t *vm, vaddr_t virt, size_t size) {
  spin_lock(&vmm_lock);
  pt_unmap(vm->pt, virt, size);
  spin_unlock(&vmm_lock);
}

void vm_switch(address_space_t *vm) {
  asm("mov %0, %%cr3" : : "r"((vaddr_t) vm->pt - hhdm_offset));
}

address_space_t *virt_new_vm() {
  address_space_t *vm = ALLOC(address_space_t);

  vm->pt = (page_table_t *) (hhdm_offset + phys_alloc(1));

  for (size_t i = 256; i < 512; i++) {
    vm->pt->entries[i] = kernel_pt->entries[i];
  }

  return vm;
}

void virt_init(struct stivale2_struct_tag_pmrs *pmrs_tag,                       //
               struct stivale2_struct_tag_kernel_base_address *kernel_base_tag, //
               struct stivale2_struct_tag_hhdm *hhdm_tag)                       //
{
  hhdm_offset = hhdm_tag->addr;
  kernel_pt = (page_table_t *) (hhdm_offset + phys_alloc(1));

  pt_map(kernel_pt, 0, 0, GIB(4), PTE_P | PTE_W);
  pt_map(kernel_pt, hhdm_offset, 0, GIB(4), PTE_P | PTE_W);

  for (size_t i = 0; i < pmrs_tag->entries; i++) {
    struct stivale2_pmr *pmr = &pmrs_tag->pmrs[i];

    uint64_t map_flags = PTE_P;

    if (IS_SET(pmr->permissions, STIVALE2_PMR_WRITABLE))
      map_flags |= PTE_W;

    if (IS_CLEAR(pmr->permissions, STIVALE2_PMR_EXECUTABLE))
      map_flags |= PTE_NX;

    paddr_t phys_addr = kernel_base_tag->physical_base_address + (pmr->base - kernel_base_tag->virtual_base_address);

    pt_map(kernel_pt, pmr->base, phys_addr, ALIGN_UP(pmr->length, 4096), map_flags);
  }

  asm("mov %0, %%cr3" : : "r"((vaddr_t) kernel_pt - hhdm_offset));
}

vaddr_t phys_to_virt(paddr_t phys) {
  return phys + hhdm_offset;
}
