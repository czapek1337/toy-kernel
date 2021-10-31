#pragma once

#include <stdint.h>

#include "acpi.h"

struct [[gnu::packed]] MadtRecordHeader {
    uint8_t type;
    uint8_t length;
};

struct [[gnu::packed]] MadtHeader : SdtHeader {
    uint32_t lapic_address;
    uint32_t flags;
};
