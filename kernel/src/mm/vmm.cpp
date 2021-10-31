#include <core/lock.h>

#include "../lib/addr.h"
#include "../lib/log.h"
#include "pmm.h"
#include "vmm.h"

static core::SpinLock vmm_lock;

static PageTable *get_page_table(PageTable *root, uint64_t index, bool create) {
    auto entry = &root->get(index);

    if (entry->get_flags() & PAGE_TABLE_ENTRY_PRESENT)
        return (PageTable *) phys_to_io(entry->get_address());

    if (!create)
        return nullptr;

    auto pt_phys = pmm::alloc(1);
    auto pt = (PageTable *) phys_to_io(pt_phys);

    __builtin_memset(pt, 0, sizeof(PageTable));

    entry->set_address(pt_phys);
    entry->set_flags(PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE | PAGE_TABLE_ENTRY_USER);

    return pt;
}

static void map_page(PageTable *root, uint64_t virt_addr, uint64_t phys_addr, uint64_t flags) {
    auto pml3 = get_page_table(root, (virt_addr >> 39) & 0x1ff, true);
    auto pml2 = get_page_table(pml3, (virt_addr >> 30) & 0x1ff, true);
    auto pml1 = get_page_table(pml2, (virt_addr >> 21) & 0x1ff, true);
    auto entry = &pml1->get((virt_addr >> 12) & 0x1ff);

    if (entry->get_flags() & PAGE_TABLE_ENTRY_PRESENT)
        log_fatal("Tried to map an address {#016x} that is already mapped to {#016x}", virt_addr, entry->get_address());

    entry->set_address(phys_addr);
    entry->set_flags(flags);
}

static void unmap_page(PageTable *root, uint64_t virt_addr) {
    auto pml3 = get_page_table(root, (virt_addr >> 39) & 0x1ff, false);
    auto pml2 = pml3 ? get_page_table(pml3, (virt_addr >> 30) & 0x1ff, false) : nullptr;
    auto pml1 = pml2 ? get_page_table(pml2, (virt_addr >> 21) & 0x1ff, false) : nullptr;
    auto entry = pml1 ? &pml1->get((virt_addr >> 12) & 0x1ff) : nullptr;

    if (!entry || !(entry->get_flags() & PAGE_TABLE_ENTRY_PRESENT))
        log_fatal("Tried to unmap an address {#016x} that is not mapped", virt_addr);

    entry->set_address(0);
    entry->set_flags(0);

    asm("invlpg (%0)" : : "r"(virt_addr));
}

static void destroy_pml(int level, PageTable *root, uint64_t start, uint64_t end) {
    if (level < 1)
        return;

    for (auto i = start; i < end; i++) {
        auto next_pml = get_page_table(root, i, false);

        if (!next_pml)
            continue;

        destroy_pml(level - 1, next_pml, 0, 512);
    }

    pmm::free(io_to_phys((uint64_t) root), 1);
}

void PageTable::map(uint64_t virt_addr, uint64_t phys_addr, uint64_t size, uint64_t flags) {
    assert_msg((virt_addr % 0x1000) == 0, "Virtual address must be page aligned");
    assert_msg((phys_addr % 0x1000) == 0, "Physical address must be page aligned");
    assert_msg((size % 0x1000) == 0, "Size must be expressed in pages");

    core::LockGuard lock(vmm_lock);

    for (auto i = 0ul; i < size / 4096; i++) {
        map_page(this, virt_addr + i * 4096, phys_addr + i * 4096, flags);
    }
}

void PageTable::unmap(uint64_t virt_addr, uint64_t size) {
    assert_msg((virt_addr % 0x1000) == 0, "Virtual address must be page aligned");
    assert_msg((size % 0x1000) == 0, "Size must be expressed in pages");

    core::LockGuard lock(vmm_lock);

    for (auto i = 0ul; i < size / 4096; i++) {
        unmap_page(this, virt_addr + i * 4096);
    }
}

void vmm::init(Stivale2StructPmrsTag *pmrs) {
    auto pt_phys = pmm::alloc(1);
    auto pt = (PageTable *) phys_to_io(pt_phys);

    __builtin_memset(pt, 0, sizeof(PageTable));

    log_info("The new kernel page table is allocated at {#016x}", pt_phys);

    pt->map(phys_to_io(0), 0, gib(4), PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITE);

    for (auto i = 0; i < pmrs->count; i++) {
        auto pmr = &pmrs->pmrs[i];
        auto flags = (uint64_t) PAGE_TABLE_ENTRY_PRESENT;

        if (pmr->permissions & STIVALE2_PMR_WRITABLE)
            flags |= PAGE_TABLE_ENTRY_WRITE;

        if (!(pmr->permissions & STIVALE2_PMR_EXECUTABLE))
            flags |= PAGE_TABLE_ENTRY_NO_EXECUTE;

        pt->map(pmr->base, kernel_to_phys(pmr->base), align_up(pmr->size, 4096), flags);
    }

    kernel_pml4 = pt;

    asm("mov %0, %%cr3" : : "r"(pt_phys));

    log_info("Successfully switched to the new kernel page table");
}

void vmm::destroy_pml4(PageTable *pml4) {
    destroy_pml(4, pml4, 0, 255);
}

void vmm::switch_to(PageTable *pml4) {
    core::LockGuard lock(vmm_lock);

    uint64_t old_plm4;

    asm volatile("mov %%cr3, %0" : "=r"(old_plm4));
    asm volatile("mov %0, %%cr3" : : "r"(io_to_phys((uint64_t) pml4)));
}

PageTable *vmm::create_pml4() {
    core::LockGuard lock(vmm_lock);

    auto pt_phys = pmm::alloc(1);
    auto pt = (PageTable *) phys_to_io(pt_phys);

    __builtin_memset(pt, 0, sizeof(PageTable));

    for (auto i = 256; i < 512; i++) {
        pt->get(i) = vmm::kernel_pml4->get(i);
    }

    return pt;
}
