#pragma once

#include <stddef.h>
#include <stdint.h>

#include <acpi/acpi.h>
#include <utils/utils.h>

namespace acpi {

  struct packed hpet_header_t {
    sdt_header_t header;
    uint8_t hardware_rev_id;
    uint8_t info;
    uint16_t pci_vendor;
    address_structure_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
  };

  void init_hpet(hpet_header_t *table);
  void sleep(size_t nanoseconds);

} // namespace acpi
