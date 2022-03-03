#pragma once

#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <utils/utils.h>

namespace acpi {

  struct packed address_structure_t {
    uint8_t address_space_id; // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
  };

  struct packed sdt_header_t {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
  };

  struct packed rsdp_header_t {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
  };

  struct packed rsdt_t {
    sdt_header_t header;
    uint32_t entries[1];
  };

  struct packed xsdt_t {
    sdt_header_t header;
    uint64_t entries[];
  };

  void init(stivale2_struct_tag_rsdp *rsdp_tag);

} // namespace acpi
