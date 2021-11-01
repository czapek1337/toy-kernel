#include "pci.h"
#include "../arch/asm.h"
#include "../lib/log.h"

constexpr static uint64_t PCI_CONFIG_ADDRESS_PORT = 0xcf8;
constexpr static uint64_t PCI_CONFIG_DATA_PORT = 0xcfc;

static void set_pci_address(uint32_t address, uint32_t offset) {
    arch::io_outd(PCI_CONFIG_ADDRESS_PORT, address | (offset & 0xfc) | 0x80000000);
}

uint16_t PciHeader::get_vendor_id() {
    return read_word(0x00);
}

uint16_t PciHeader::get_device_id() {
    return read_word(0x02);
}

uint8_t PciHeader::get_class_code() {
    return read_byte(0x0b);
}

uint8_t PciHeader::get_subclass() {
    return read_byte(0x0a);
}

uint8_t PciHeader::get_prog_if() {
    return read_byte(0x09);
}

uint8_t PciHeader::get_revision() {
    return read_byte(0x08);
}

uint8_t PciHeader::get_header_type() {
    return read_byte(0x0e);
}

uint8_t PciHeader::read_byte(uint32_t offset) {
    set_pci_address(address, offset);

    return arch::io_inb(PCI_CONFIG_DATA_PORT + (offset & 3));
}

uint16_t PciHeader::read_word(uint32_t offset) {
    assert_msg((offset & 1) == 0, "PCI word read from unaligned offset");

    set_pci_address(address, offset);

    return arch::io_inw(PCI_CONFIG_DATA_PORT + (offset & 3));
}

uint32_t PciHeader::read_dword(uint32_t offset) {
    assert_msg((offset & 3) == 0, "PCI dword read from unaligned offset");

    set_pci_address(address, offset);

    return arch::io_ind(PCI_CONFIG_DATA_PORT + (offset & 3));
}

void PciHeader::write_byte(uint32_t offset, uint8_t value) {
    set_pci_address(address, offset);

    arch::io_outb(PCI_CONFIG_DATA_PORT + (offset & 3), value);
}

void PciHeader::write_word(uint32_t offset, uint16_t value) {
    assert_msg((offset & 1) == 0, "PCI word write to unaligned offset");

    set_pci_address(address, offset);

    arch::io_outw(PCI_CONFIG_DATA_PORT + (offset & 3), value);
}

void PciHeader::write_dword(uint32_t offset, uint32_t value) {
    assert_msg((offset & 3) == 0, "PCI dword write to unaligned offset");

    set_pci_address(address, offset);

    arch::io_outd(PCI_CONFIG_DATA_PORT + (offset & 3), value);
}

PciBar PciHeader::get_bar(uint32_t index) {
    // TODO: Implement

    return {};
}

void PciHeader::enable_mmio() {
    // TODO: Implement
}

void PciHeader::become_master() {
    // TODO: Implement
}

static void handle_pci_device(uint64_t bus, uint64_t device, uint64_t function) {
    auto header = PciHeader(bus, device, function);

    if (header.get_vendor_id() == 0xffff)
        return;

    log_debug("Found device {#04x}:{#04x} in slot {}.{} on bus {}, class={}, subclass={}", header.get_vendor_id(), header.get_device_id(),
              device, function, bus, header.get_class_code(), header.get_subclass());

    if (function == 0 && header.get_header_type() & 0x80) {
        for (auto function = 1; function < 8; function++) {
            handle_pci_device(bus, device, function);
        }
    }
}

void pci::scan() {
    for (auto bus = 0; bus < 256; bus++) {
        for (auto device = 0; device < 32; device++) {
            handle_pci_device(bus, device, 0);
        }
    }
}
