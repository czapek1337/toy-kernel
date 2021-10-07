#pragma once

#include <stdint.h>

namespace arch {

void halt();

void io_outb(uint16_t port, uint8_t value);
void io_outw(uint16_t port, uint16_t value);
void io_outd(uint16_t port, uint32_t value);

uint8_t io_inb(uint16_t port);
uint16_t io_inw(uint16_t port);
uint32_t io_ind(uint16_t port);

void mmio_outb(uint64_t addr, uint8_t value);
void mmio_outw(uint64_t addr, uint16_t value);
void mmio_outd(uint64_t addr, uint32_t value);
void mmio_outq(uint64_t addr, uint64_t value);

uint8_t mmio_inb(uint64_t addr);
uint16_t mmio_inw(uint64_t addr);
uint32_t mmio_ind(uint64_t addr);
uint64_t mmio_inq(uint64_t addr);

} // namespace arch
