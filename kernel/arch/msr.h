#pragma once

#include <stdint.h>

#define MSR_APIC 0x1b

void msr_write(uint32_t msr, uint64_t value);

uint64_t msr_read(uint32_t msr);
