#include "arch/gdt.h"
#include "boot/stivale2.h"
#include "lib/log.h"
#include "mm/heap.h"
#include "mm/pmm.h"
#include "mm/vmm.h"

void kernel_main(stivale2_struct_t *boot_info) {
    auto mmap = (stivale2_struct_mmap_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_MMAP_TAG);
    auto pmrs = (stivale2_struct_pmrs_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_PMRS_TAG);

    arch::init_gdt();
    arch::init_tss();

    pmm::init(mmap);
    vmm::init(pmrs);

    arch::halt();
}
