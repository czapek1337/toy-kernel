#pragma once

#include <stdint.h>

class Msr {
private:
    uint32_t m_address;

public:
    constexpr Msr(uint32_t address) : m_address(address) {}

    constexpr static Msr apic() { return 0x1b; }
    constexpr static Msr efer() { return 0xc0000080; }
    constexpr static Msr star() { return 0xc0000081; }
    constexpr static Msr lstar() { return 0xc0000082; }
    constexpr static Msr syscall_flag_mask() { return 0xc0000084; }
    constexpr static Msr fs_base() { return 0xc0000100; }
    constexpr static Msr gs_base() { return 0xc0000101; }
    constexpr static Msr kernel_gs_base() { return 0xc0000102; }

    inline uint64_t read() const {
        uint32_t low, high;

        asm("rdmsr" : "=a"(low), "=d"(high) : "c"(m_address));

        return ((uint64_t) high << 32) | low;
    }

    inline void write(uint64_t value) const {
        auto low = (uint32_t) (value & 0xffffffff);
        auto high = (uint32_t) (value >> 32);

        asm("wrmsr" : : "a"(low), "d"(high), "c"(m_address));
    }
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

} // namespace arch
