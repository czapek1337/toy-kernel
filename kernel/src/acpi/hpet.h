#pragma once

#include <stdint.h>

#include "acpi.h"

enum HpetRegister {
    HPET_REG_GENERAL_CAPS_AND_ID = 0x00,
    HPET_REG_GENERAL_CONFIG = 0x10,
    HPET_REG_MAIN_COUNTER = 0xf0,
};

struct [[gnu::packed]] HpetHeader : SdtHeader {
    uint8_t hardware_rev_id;
    uint8_t info;
    uint16_t pci_vendor;
    uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
};

namespace hpet {

void init();
void sleep_for(uint64_t nanoseconds);

} // namespace hpet
