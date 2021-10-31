#include "asm.h"
#include "../lib/addr.h"

void arch::pause() {
    asm("pause");
}

void arch::halt() {
    asm("hlt");
}

void arch::halt_forever() {
    asm("cli");
    asm("hlt");
}

void arch::disable_interrupts() {
    asm("cli");
}

void arch::enable_interrupts() {
    asm("sti");
}

void arch::io_outb(uint16_t port, uint8_t value) {
    asm("out %0, %1" : : "a"(value), "d"(port));
}

void arch::io_outw(uint16_t port, uint16_t value) {
    asm("out %0, %1" : : "a"(value), "d"(port));
}

void arch::io_outd(uint16_t port, uint32_t value) {
    asm("out %0, %1" : : "a"(value), "d"(port));
}

uint8_t arch::io_inb(uint16_t port) {
    uint8_t value;
    asm("in %1, %0" : "=a"(value) : "d"(port));
    return value;
}

uint16_t arch::io_inw(uint16_t port) {
    uint16_t value;
    asm("in %1, %0" : "=a"(value) : "d"(port));
    return value;
}

uint32_t arch::io_ind(uint16_t port) {
    uint32_t value;
    asm("in %1, %0" : "=a"(value) : "d"(port));
    return value;
}

void arch::mmio_outb(uint64_t addr, uint8_t value) {
    *(volatile uint8_t *) phys_to_io(addr) = value;
}

void arch::mmio_outw(uint64_t addr, uint16_t value) {
    *(volatile uint16_t *) phys_to_io(addr) = value;
}

void arch::mmio_outd(uint64_t addr, uint32_t value) {
    *(volatile uint32_t *) phys_to_io(addr) = value;
}

void arch::mmio_outq(uint64_t addr, uint64_t value) {
    *(volatile uint64_t *) phys_to_io(addr) = value;
}

uint8_t arch::mmio_inb(uint64_t addr) {
    return *(volatile uint8_t *) phys_to_io(addr);
}

uint16_t arch::mmio_inw(uint64_t addr) {
    return *(volatile uint16_t *) phys_to_io(addr);
}

uint32_t arch::mmio_ind(uint64_t addr) {
    return *(volatile uint32_t *) phys_to_io(addr);
}

uint64_t arch::mmio_inq(uint64_t addr) {
    return *(volatile uint64_t *) phys_to_io(addr);
}
