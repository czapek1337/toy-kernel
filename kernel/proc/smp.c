#include <stddef.h>
#include <stdint.h>

#include "../mem/phys.h"
#include "../mem/virt.h"
#include "smp.h"

void kernel_ap_main(struct stivale2_smp_info *ap_info);

void smp_init(struct stivale2_struct_tag_smp *smp_tag) {
  for (size_t i = 0; i < smp_tag->cpu_count; i++) {
    struct stivale2_smp_info *info = &smp_tag->smp_info[i];

    if (info->lapic_id == smp_tag->bsp_lapic_id)
      continue;

    info->target_stack = phys_to_virt(phys_alloc(2) + 0x2000);
    info->goto_address = (uint64_t) kernel_ap_main;
  }
}
