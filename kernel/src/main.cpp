#include "acpi/acpi.h"
#include "acpi/hpet.h"
#include "arch/cpu.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "boot/stivale2.h"
#include "fs/vfs.h"
#include "intr/apic.h"
#include "lib/addr.h"
#include "lib/log.h"
#include "mm/heap.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "proc/sched.h"
#include "proc/syscall.h"

void kernel_main(stivale2_struct_t *boot_info) {
    // Grab pointers to struct tags from the boot info
    auto mmap = (stivale2_struct_mmap_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_MMAP_TAG);
    auto pmrs = (stivale2_struct_pmrs_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_PMRS_TAG);
    auto rsdp = (stivale2_struct_rsdp_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_RSDP_TAG);
    auto modules = (stivale2_struct_modules_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_MODULES_TAG);

    // Initialize the per-CPU structures as early as possible
    arch::init_bsp();

    // Make sure interrupts are disabled until the scheduler is initialized
    intr::retain();

    // Make sure we have a valid RSDP tag
    if (!rsdp)
        log_fatal("Cannot proceed without a valid RSDP tag");

    arch::init_gdt(); // Set up necessary x86_64 structures like GDT and an IDT
    arch::init_idt(); // That will let us handle interrupts from external devices and CPU exceptions

    pmm::init(mmap); // Initialize virtual and physical memory managers
    vmm::init(pmrs); // This will let us allocate and map physical memory

    heap::init();       // Initialize heap for dynamic memory allocation
    arch::init_tss();   // Initialize the TSS
    acpi::init(rsdp);   // Scan the ACPI tables
    hpet::init();       // Initialize the HPET timer
    apic::init();       // Configure the local APIC
    syscall::init();    // Configure the syscall interface
    vfs::init(modules); // Initialize and test the VFS
    task::init_sched(); // Initialize the scheduler and kernel idle task

    // Create a new user task from passed in modules
    for (auto i = 0; i < modules->count; i++) {
        auto module = &modules->modules[i];

        if (__builtin_strcmp(module->name, "hello") != 0)
            continue;

        task::create_task_from_elf(module->name, (elf64_t *) module->base, kib(4), true);
    }

    // Start scheduling by enabling interrupts
    intr::release();

    // Halt indefinitely, the scheduler will take over from here
    while (true) {
        arch::halt();
    }
}
