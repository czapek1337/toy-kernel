#pragma once

#include <stdint.h>

#include "../boot/stivale2.h"

struct [[gnu::packed]] RsdpHeader {
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

struct [[gnu::packed]] SdtHeader {
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

struct [[gnu::packed]] RsdtHeader : SdtHeader {
    uint32_t entries[];
};

struct [[gnu::packed]] XsdtHeader : SdtHeader {
    uint64_t entries[];
};

namespace acpi {

void init(Stivale2StructRsdpTag *rsdp);

SdtHeader *find_table(const char *signature);

} // namespace acpi
