#include <core/vector.h>

#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/vmm.h"
#include "acpi.h"

static core::Vector<SdtHeader *> tables;

static void handle_table(SdtHeader *header) {
    log_debug("Found ACPI table '{4}' at {#016x}", (const char *) header->signature, header);

    tables.push(header);
}

void acpi::init(Stivale2StructRsdpTag *rsdp) {
    auto rsdp_header = (RsdpHeader *) rsdp->addr;

    if (rsdp_header->revision == 0) {
        auto rsdt = (RsdtHeader *) phys_to_io((uint64_t) rsdp_header->rsdt_address);
        auto element_count = (rsdt->length - sizeof(SdtHeader)) / sizeof(uint32_t);

        for (auto i = 0; i < element_count; i++) {
            handle_table((SdtHeader *) phys_to_io((uint64_t) rsdt->entries[i]));
        }
    } else if (rsdp_header->revision == 2) {
        auto xsdt = (XsdtHeader *) phys_to_io(rsdp_header->xsdt_address);
        auto element_count = (xsdt->length - sizeof(SdtHeader)) / sizeof(uint64_t);

        for (auto i = 0; i < element_count; i++) {
            handle_table((SdtHeader *) phys_to_io(xsdt->entries[i]));
        }
    } else {
        log_fatal("Unknown ACPI version, expected revision 0 or 2, got {}", rsdp_header->revision);
    }
}

SdtHeader *acpi::find_table(const char *signature) {
    for (auto i = 0; i < tables.size(); i++) {
        auto table = tables[i];

        if (__builtin_strncmp(table->signature, signature, 4) == 0)
            return table;
    }

    return nullptr;
}
