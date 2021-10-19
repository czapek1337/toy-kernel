#include "acpi/acpi.h"
#include "acpi/hpet.h"
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

void kernel_idle_thread() {
    log_info("Started the kernel idle task");

    // Halt until the next interrupt comes in
    while (true) {
        arch::halt();
    }
}

void kernel_main(stivale2_struct_t *boot_info) {
    // Grab pointers to struct tags from the boot info
    auto mmap = (stivale2_struct_mmap_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_MMAP_TAG);
    auto pmrs = (stivale2_struct_pmrs_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_PMRS_TAG);
    auto rsdp = (stivale2_struct_rsdp_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_RSDP_TAG);
    auto modules = (stivale2_struct_modules_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_MODULES_TAG);

    // Make sure we have a valid RSDP tag
    if (!rsdp)
        log_fatal("Cannot proceed without a valid RSDP tag");

    // Set up necessary x86_64 structures like GDT and an IDT
    // That will let us handle interrupts (and exceptions) from external devices
    arch::init_gdt();
    arch::init_idt();

    // Initialize both virtual and physical memory managers
    // This will let us allocate and map physical memory for the kernel
    pmm::init(mmap);
    vmm::init(pmrs);

    // Initialize heap for dynamic memory allocation
    heap::init();

    // Initialize the TSS
    arch::init_tss();

    // Scan the ACPI tables
    acpi::init(rsdp);

    // Initialize the HPET timer
    hpet::init();

    // Make sure interrupts are disabled until the scheduler is initialized
    intr::retain();

    // Configure the local APIC
    apic::init();

    // Configure the syscall interface
    syscall::init();

    // Initialize and test the VFS
    vfs::init();

    // Initialize the scheduler and kernel idle task
    task::init_sched();
    task::create_task("idle", (uint64_t) kernel_idle_thread, kib(4), false);

    // Create new user tasks from passed in modules
    // for (auto i = 0; i < modules->count; i++) {
    //     auto module = &modules->modules[i];

    //     task::create_task_from_elf(module->name, (elf64_t *) module->base, kib(4), true);
    // }

    // Start scheduling by enabling interrupts
    intr::release();

    // Halt indefinitely - once we have a scheduler, it will take over from here
    while (true) {
        arch::halt();
    }
}
