#include <string.h>

#include <acpi/acpi.h>
#include <acpi/hpet.h>
#include <mem/virt.h>
#include <utils/print.h>

static void acpi_discover_table(vaddr_t addr) {
  auto table = (acpi::sdt_header_t *) addr;

  if (memcmp(table->signature, "HPET", 4) == 0)
    acpi::init_hpet((acpi::hpet_header_t *) table);
}

void acpi::init(stivale2_struct_tag_rsdp *rsdp_tag) {
  auto rsdp = (rsdp_header_t *) rsdp_tag->rsdp;

  if (rsdp->revision == 0) {
    auto rsdt = (rsdt_t *) mem::phys_to_virt(rsdp->rsdt_address);
    auto length = rsdt->header.length - sizeof(sdt_header_t);

    for (auto i = 0u; i < length / sizeof(uint32_t); i++) {
      acpi_discover_table(mem::phys_to_virt(rsdt->entries[i]));
    }
  } else if (rsdp->revision == 2) {
    auto xsdt = (xsdt_t *) mem::phys_to_virt(rsdp->xsdt_address);
    auto length = xsdt->header.length - sizeof(sdt_header_t);

    for (auto i = 0u; i < length / sizeof(uint64_t); i++) {
      acpi_discover_table(mem::phys_to_virt(xsdt->entries[i]));
    }
  } else {
    klog_panic("Unknown ACPI revision, expected 0 or 2, got %d", rsdp->revision);
  }
}
