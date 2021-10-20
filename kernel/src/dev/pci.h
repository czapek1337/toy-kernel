#pragma once

#include <stdint.h>

enum pci_vendor_id_t {
    PCI_VENDOR_QEMU = 0x1234,
    PCI_VENDOR_INTEL = 0x8086,
};

enum pci_class_code_t {
    PCI_CLASS_UNKNOWN = 0x00,
    PCI_CLASS_MASS_STORAGE_CONTROLLER = 0x01,
    PCI_CLASS_NETWORK_CONTROLLER = 0x02,
    PCI_CLASS_DISPLAY_CONTROLLER = 0x03,
    PCI_CLASS_BRIDGE_DEVICE = 0x06,
};

enum pci_subclass_t {
    // Unknown
    PCI_SUBCLASS_UNKNOWN_NON_VGA_COMPAT = 0x00,
    PCI_SUBCLASS_UNKNOWN_VGA_COMPAT = 0x01,
    // Mass storage controller
    PCI_SUBCLASS_MSC_IDE_CONTROLLER = 0x01,
    PCI_SUBCLASS_MSC_SATA_CONTROLLER = 0x06,
    PCI_SUBCLASS_MSC_NVM_CONTROLLER = 0x08,
    // Network controller
    PCI_SUBCLASS_NET_ETHERNET_CONTROLLER = 0x00,
    PCI_SUBCLASS_NET_OTHER = 0x80,
    // Display controller
    PCI_SUBCLASS_DISPLAY_VGA_COMPAT = 0x00,
    PCI_SUBCLASS_DISPLAY_3D_CONTROLLER = 0x03,
    PCI_SUBCLASS_DISPLAY_OTHER = 0x80,
    // Bridge device
    PCI_SUBCLASS_BRIDGE_HOST = 0x00,
    PCI_SUBCLASS_BRIDGE_ISA = 0x01,
};

struct pci_bar_t {
    uint64_t addr;
    uint64_t size;

    bool mmio;
    bool prefetchable;
};

struct pci_header_t {
    uint32_t bus;
    uint32_t device;
    uint32_t function;
    uint32_t address;

    pci_header_t(uint32_t bus, uint32_t device, uint32_t function) : bus(bus), device(device), function(function) {
        address = (bus << 16) | (device << 11) | (function << 8);
    }

    uint16_t get_vendor_id();
    uint16_t get_device_id();

    uint8_t get_class_code();
    uint8_t get_subclass();
    uint8_t get_prog_if();
    uint8_t get_revision();
    uint8_t get_header_type();

    uint8_t read_byte(uint32_t offset);
    uint16_t read_word(uint32_t offset);
    uint32_t read_dword(uint32_t offset);

    void write_byte(uint32_t offset, uint8_t value);
    void write_word(uint32_t offset, uint16_t value);
    void write_dword(uint32_t offset, uint32_t value);

    pci_bar_t get_bar(uint32_t index);

    void enable_mmio();
    void become_master();
};

namespace pci {

void scan();

}
