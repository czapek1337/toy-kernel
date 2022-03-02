#pragma once

#include <stddef.h>
#include <stdint.h>

void apic_init();
void apic_eoi();
void apic_ipi(size_t lapic_id, uint64_t id);

size_t apic_get_lapic();
