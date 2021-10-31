#pragma once

#include <stdint.h>

#include "../boot/stivale2.h"

enum PageTableEntryFlags : uint64_t {
    PAGE_TABLE_ENTRY_PRESENT = 1 << 0,
    PAGE_TABLE_ENTRY_WRITE = 1 << 1,
    PAGE_TABLE_ENTRY_USER = 1 << 2,
    PAGE_TABLE_ENTRY_WRITE_THROUGH = 1 << 3,
    PAGE_TABLE_ENTRY_NO_CACHE = 1 << 4,
    PAGE_TABLE_ENTRY_GLOBAL = 1 << 8,
    PAGE_TABLE_ENTRY_NO_EXECUTE = 1ull << 63,
};

class PageTableEntry {
private:
    uint64_t m_value;

public:
    PageTableEntry(const PageTableEntry &other) : m_value(other.m_value) {}
    PageTableEntry(uint64_t addr, uint64_t flags) : m_value(addr | flags) {}

    uint64_t get_address() const { return m_value & 0x000ffffffffff000; }
    uint64_t get_flags() const { return m_value & ~0x000ffffffffff000; }

    void set_address(uint64_t addr) { m_value = get_flags() | addr; }
    void set_flags(uint64_t flags) { m_value = get_address() | flags; }
};

class PageTable {
private:
    PageTableEntry m_entries[512];

public:
    void map(uint64_t virt_addr, uint64_t phys_addr, uint64_t size, uint64_t flags);
    void unmap(uint64_t virt_addr, uint64_t size);

    PageTableEntry &get(uint64_t index) { return m_entries[index]; }
    const PageTableEntry &get(uint64_t index) const { return m_entries[index]; }
};

class TemporaryMappingGuard {
private:
    PageTable *m_pml4;

    uint64_t m_virt_addr;
    uint64_t m_phys_addr;
    uint64_t m_size;

public:
    inline TemporaryMappingGuard(PageTable *page_table, uint64_t virt_addr, uint64_t phys_addr, uint64_t size, uint64_t flags)
        : m_pml4(page_table), m_virt_addr(virt_addr), m_phys_addr(phys_addr), m_size(size) //
    {
        page_table->map(virt_addr, phys_addr, size, flags);
    }

    inline TemporaryMappingGuard(PageTable *page_table, uint64_t addr, uint64_t size, uint64_t flags)
        : TemporaryMappingGuard(page_table, addr, addr, size, flags) {}

    inline ~TemporaryMappingGuard() { m_pml4->unmap(m_virt_addr, m_size); }
};

namespace vmm {

void init(Stivale2StructPmrsTag *pmrs);
void destroy_pml4(PageTable *pml4);
void switch_to(PageTable *pml4);

PageTable *create_pml4();

inline PageTable *kernel_pml4;

} // namespace vmm
