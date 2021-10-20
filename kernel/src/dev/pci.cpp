#include "pci.h"
#include "../arch/asm.h"
#include "../lib/log.h"

constexpr static uint64_t PCI_CONFIG_ADDRESS_PORT = 0xcf8;
constexpr static uint64_t PCI_CONFIG_DATA_PORT = 0xcfc;

static const char *get_pci_vendor_name(uint16_t vendor) {
    switch (vendor) {
    case PCI_VENDOR_QEMU: return "QEMU";
    case PCI_VENDOR_INTEL: return "Intel";
    }

    return "Unknown";
}

static const char *get_class_name(uint16_t class_code) {
    switch (class_code) {
    case PCI_CLASS_UNKNOWN: return "Unknown";
    case PCI_CLASS_MASS_STORAGE_CONTROLLER: return "Mass storage controller";
    case PCI_CLASS_NETWORK_CONTROLLER: return "Network controller";
    case PCI_CLASS_DISPLAY_CONTROLLER: return "Display controller";
    case PCI_CLASS_BRIDGE_DEVICE: return "Bridge device";
    }

    return "Unknown";
}

static const char *get_subclass_name(uint16_t class_code, uint8_t subclass) {
    switch (class_code) {
    case PCI_CLASS_UNKNOWN:
        switch (subclass) {
        case PCI_SUBCLASS_UNKNOWN_NON_VGA_COMPAT: return "Unknown non-VGA compatible";
        case PCI_SUBCLASS_UNKNOWN_VGA_COMPAT: return "Unknown VGA compatible";
        }

        return "Unknown";
    case PCI_CLASS_MASS_STORAGE_CONTROLLER:
        switch (subclass) {
        case PCI_SUBCLASS_MSC_IDE_CONTROLLER: return "IDE controller";
        case PCI_SUBCLASS_MSC_SATA_CONTROLLER: return "SATA controller";
        case PCI_SUBCLASS_MSC_NVM_CONTROLLER: return "NVM controller";
        }

        return "Unknown";
    case PCI_CLASS_NETWORK_CONTROLLER:
        switch (subclass) {
        case PCI_SUBCLASS_NET_ETHERNET_CONTROLLER: return "Ethernet controller";
        case PCI_SUBCLASS_NET_OTHER: return "Other network controller";
        }

        return "Unknown";
    case PCI_CLASS_DISPLAY_CONTROLLER:
        switch (subclass) {
        case PCI_SUBCLASS_DISPLAY_VGA_COMPAT: return "VGA compatible";
        case PCI_SUBCLASS_DISPLAY_3D_CONTROLLER: return "3D controller";
        case PCI_SUBCLASS_DISPLAY_OTHER: return "Other display controller";
        }

        return "Unknown";
    case PCI_CLASS_BRIDGE_DEVICE:
        switch (subclass) {
        case PCI_SUBCLASS_BRIDGE_HOST: return "Host bridge";
        case PCI_SUBCLASS_BRIDGE_ISA: return "ISA bridge";
        }

        return "Unknown";
    }

    return "Unknown";
}

static void set_pci_address(uint32_t address, uint32_t offset) {
    arch::io_outd(PCI_CONFIG_ADDRESS_PORT, address | (offset & 0xfc) | 0x80000000);
}

uint16_t pci_header_t::get_vendor_id() {
    return read_word(0x00);
}

uint16_t pci_header_t::get_device_id() {
    return read_word(0x02);
}

uint8_t pci_header_t::get_class_code() {
    return read_byte(0x0b);
}

uint8_t pci_header_t::get_subclass() {
    return read_byte(0x0a);
}

uint8_t pci_header_t::get_prog_if() {
    return read_byte(0x09);
}

uint8_t pci_header_t::get_revision() {
    return read_byte(0x08);
}

uint8_t pci_header_t::get_header_type() {
    return read_byte(0x0e);
}

uint8_t pci_header_t::read_byte(uint32_t offset) {
    set_pci_address(address, offset);

    return arch::io_inb(PCI_CONFIG_DATA_PORT + (offset & 3));
}

uint16_t pci_header_t::read_word(uint32_t offset) {
    assert_msg((offset & 1) == 0, "PCI word read from unaligned offset");

    set_pci_address(address, offset);

    return arch::io_inw(PCI_CONFIG_DATA_PORT + (offset & 3));
}

uint32_t pci_header_t::read_dword(uint32_t offset) {
    assert_msg((offset & 3) == 0, "PCI dword read from unaligned offset");

    set_pci_address(address, offset);

    return arch::io_ind(PCI_CONFIG_DATA_PORT + (offset & 3));
}

void pci_header_t::write_byte(uint32_t offset, uint8_t value) {
    set_pci_address(address, offset);

    arch::io_outb(PCI_CONFIG_DATA_PORT + (offset & 3), value);
}

void pci_header_t::write_word(uint32_t offset, uint16_t value) {
    assert_msg((offset & 1) == 0, "PCI word write to unaligned offset");

    set_pci_address(address, offset);

    arch::io_outw(PCI_CONFIG_DATA_PORT + (offset & 3), value);
}

void pci_header_t::write_dword(uint32_t offset, uint32_t value) {
    assert_msg((offset & 3) == 0, "PCI dword write to unaligned offset");

    set_pci_address(address, offset);

    arch::io_outd(PCI_CONFIG_DATA_PORT + (offset & 3), value);
}

pci_bar_t pci_header_t::get_bar(uint32_t index) {
    // TODO: Implement

    return {};
}

void pci_header_t::enable_mmio() {
    // TODO: Implement
}

void pci_header_t::become_master() {
    // TODO: Implement
}

static void handle_pci_device(uint64_t bus, uint64_t device) {
    auto header = pci_header_t(bus, device, 0);

    if (header.get_vendor_id() == 0xffff)
        return;

    auto vendor_name = get_pci_vendor_name(header.get_vendor_id());
    auto class_name = get_class_name(header.get_class_code());
    auto subclass_name = get_subclass_name(header.get_class_code(), header.get_subclass());

    log_debug("Found device {} on bus {}, vendor={}, device={#04x}, class={}, subclass={}, prog_if={#02x}, header_type={#02x}", device, bus,
              vendor_name, header.get_device_id(), class_name, subclass_name, header.get_prog_if(), header.get_header_type());
}

static void scan_pci_bus(uint64_t bus) {
    for (auto device = 0; device < 32; device++) {
        handle_pci_device(bus, device);
    }
}

void pci::scan() {
    for (auto bus = 0; bus < 255; bus++) {
        scan_pci_bus(bus);
    }
}
