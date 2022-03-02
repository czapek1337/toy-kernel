#include <string.h>

#include "../mem/virt.h"
#include "../utils/print.h"
#include "acpi.h"
#include "hpet.h"

static void acpi_discover_table(vaddr_t addr) {
  sdt_header_t *table = (void *) addr;

  if (memcmp(table->signature, "HPET", 4) == 0)
    hpet_init((hpet_header_t *) table);
}

void acpi_init(struct stivale2_struct_tag_rsdp *rsdp_tag) {
  rsdp_header_t *rsdp = (rsdp_header_t *) rsdp_tag->rsdp;

  if (rsdp->revision == 0) {
    rsdt_t *rsdt = (rsdt_t *) phys_to_virt(rsdp->rsdt_address);

    for (size_t i = 0; i < (rsdt->header.length - sizeof(sdt_header_t)) / sizeof(uint32_t); i++) {
      acpi_discover_table((uintptr_t) phys_to_virt(rsdt->entries[i]));
    }
  } else if (rsdp->revision == 2) {
    xsdt_t *xsdt = (xsdt_t *) phys_to_virt(rsdp->xsdt_address);

    for (size_t i = 0; i < (xsdt->header.length - sizeof(sdt_header_t)) / sizeof(uint64_t); i++) {
      acpi_discover_table((uintptr_t) phys_to_virt(xsdt->entries[i]));
    }
  } else {
    klog_panic("Unknown ACPI revision, expected 0 or 2, got %d", rsdp->revision);
  }
}
