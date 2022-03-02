#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../boot/stivale2.h"

typedef struct {
  uint8_t address_space_id; // 0 - system memory, 1 - system I/O
  uint8_t register_bit_width;
  uint8_t register_bit_offset;
  uint8_t reserved;
  uint64_t address;
} __attribute__((packed)) address_structure_t;

typedef struct {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oem_id[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((__packed__)) sdt_header_t;

typedef struct {
  char signature[8];
  uint8_t checksum;
  char oem_id[6];
  uint8_t revision;
  uint32_t rsdt_address;
  uint32_t length;
  uint64_t xsdt_address;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((__packed__)) rsdp_header_t;

typedef struct {
  sdt_header_t header;
  uint32_t entries[1];
} __attribute__((__packed__)) rsdt_t;

typedef struct {
  sdt_header_t header;
  uint64_t entries[];
} __attribute__((__packed__)) xsdt_t;

void acpi_init(struct stivale2_struct_tag_rsdp *rsdp_tag);
