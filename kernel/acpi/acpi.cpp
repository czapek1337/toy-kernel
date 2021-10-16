#include "acpi.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/vmm.h"

static void handle_table(sdt_header_t *header) {
    log_debug("Found ACPI table '{4}' at {#016x}", (const char *) &header->signature[0], header);
}

void acpi::init(stivale2_struct_rsdp_tag_t *rsdp) {
    auto rsdp_header = (rsdp_header_t *) rsdp->addr;

    log_info("The RSDP has a signature '{8}' and is located at {#016x}", rsdp_header->signature, rsdp_header);

    if (rsdp_header->revision == 0) {
        auto rsdt = (rsdt_t *) phys_to_io((uint64_t) rsdp_header->rsdt_address);
        auto element_count = (rsdt->header.length - sizeof(sdt_header_t)) / sizeof(uint32_t);

        log_info("This system uses ACPI 1.0, the RSDT is located at {#016x}", rsdt);

        for (auto i = 0; i < element_count; i++) {
            handle_table((sdt_header_t *) phys_to_io((uint64_t) rsdt->entries[i]));
        }
    } else if (rsdp_header->revision == 2) {
        auto xsdt = (xsdt_t *) phys_to_io(rsdp_header->xsdt_address);
        auto element_count = (xsdt->header.length - sizeof(sdt_header_t)) / sizeof(uint64_t);

        log_info("This system uses ACPI 2.0, the XSDT is located at {#016x}", xsdt);

        for (auto i = 0; i < element_count; i++) {
            handle_table((sdt_header_t *) phys_to_io(xsdt->entries[i]));
        }
    } else {
        log_fatal("Unknown ACPI version, expected revision 0 or 2, got {}", rsdp_header->revision);
    }
}
