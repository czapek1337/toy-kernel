#pragma once

#include <stdint.h>

#include "acpi.h"

struct [[gnu::packed]] madt_record_t {
    uint8_t type;
    uint8_t length;
};

struct [[gnu::packed]] madt_t {
    sdt_header_t header;
    uint32_t lapic_address;
    uint32_t flags;
};
