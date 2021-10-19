#pragma once

#include <stdint.h>

enum msr_t {
    MSR_APIC = 0x1b,
    MSR_EFER = 0xc0000080,
    MSR_STAR = 0xc0000081,
    MSR_LSTAR = 0xc0000082,
    MSR_SYSCALL_FLAG_MASK = 0xc0000084,
    MSR_FS_BASE = 0xc0000100,
    MSR_GS_BASE = 0xc0000101,
    MSR_KERN_GS_BASE = 0xc0000102,
};

namespace arch {

void pause();
void halt();
void halt_forever();

void disable_interrupts();
void enable_interrupts();

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

uint64_t read_msr(uint32_t msr);

void write_msr(uint32_t msr, uint64_t value);

} // namespace arch
