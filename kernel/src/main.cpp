#include "acpi/acpi.h"
#include "acpi/hpet.h"
#include "arch/cpu.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "boot/stivale2.h"
#include "dev/pci.h"
#include "dev/udev.h"
#include "fs/vfs.h"
#include "intr/apic.h"
#include "lib/addr.h"
#include "lib/console.h"
#include "lib/log.h"
#include "mm/heap.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "sched/sched.h"
#include "sched/syscall.h"

extern "C" void kernel_start(Stivale2Struct *boot_info) {
    auto mmap = (Stivale2StructMemoryMapTag *) boot_info->query_tag(STIVALE2_STRUCT_MMAP_TAG);
    auto pmrs = (Stivale2StructPmrsTag *) boot_info->query_tag(STIVALE2_STRUCT_PMRS_TAG);
    auto rsdp = (Stivale2StructRsdpTag *) boot_info->query_tag(STIVALE2_STRUCT_RSDP_TAG);
    auto framebuffer = (Stivale2StructFramebufferTag *) boot_info->query_tag(STIVALE2_STRUCT_FRAMEBUFFER_TAG);

    arch::init_bsp();
    intr::retain();

    assert_msg(rsdp, "Cannot proceed without a valid RSDP tag");

    initialize_gdt();
    initialize_idt();

    pmm::init(mmap);
    vmm::init(pmrs);

    initialize_tss();

    heap::init();
    console::init(framebuffer);
    acpi::init(rsdp);
    hpet::init();
    apic::init();
    syscall::init();
    vfs::init(boot_info);
    dev::init_udev(framebuffer);
    pci::scan();
    sched::init();

    // Create a new user task from passed in modules
    auto hello_elf = boot_info->query_module("hello.elf");

    Process::spawn(nullptr, "hello", (Elf64 *) hello_elf->base, true);
    Process::spawn(nullptr, "hello", (Elf64 *) hello_elf->base, true);
    Process::spawn(nullptr, "hello", (Elf64 *) hello_elf->base, true);
    Process::spawn(nullptr, "hello", (Elf64 *) hello_elf->base, true);
    Process::spawn(nullptr, "hello", (Elf64 *) hello_elf->base, true);
    Process::spawn(nullptr, "hello", (Elf64 *) hello_elf->base, true);

    // Start scheduling by enabling interrupts
    intr::release();

    // Halt indefinitely, the scheduler will take over from here
    while (true) {
        arch::halt();
    }
}
