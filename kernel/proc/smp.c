#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#include "../mem/phys.h"
#include "../mem/virt.h"
#include "cpu.h"
#include "smp.h"

static size_t ap_count;
static size_t ap_booted_count;

void kernel_ap_main(struct stivale2_smp_info *ap_info);

void smp_init(struct stivale2_struct_tag_smp *smp_tag) {
  for (size_t i = 0; i < smp_tag->cpu_count; i++) {
    struct stivale2_smp_info *info = &smp_tag->smp_info[i];

    if (info->lapic_id == smp_tag->bsp_lapic_id)
      continue;

    info->target_stack = phys_to_virt(phys_alloc(2) + 0x2000);
    info->goto_address = (uint64_t) kernel_ap_main;

    ap_count++;
  }

  while (ap_booted_count != ap_count) {
    asm("pause");
  }
}

void smp_init_cpu(struct stivale2_smp_info *ap_info) {
  cpu_init(ap_info->processor_id, ap_info->lapic_id);

  __atomic_add_fetch(&ap_booted_count, 1, __ATOMIC_ACQ_REL);
}
