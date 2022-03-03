#include <acpi/acpi.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <boot/stivale2.h>
#include <interrupts/apic.h>
#include <mem/heap.h>
#include <mem/phys.h>
#include <mem/virt.h>
#include <proc/cpu.h>
#include <proc/sched.h>
#include <utils/print.h>

aligned(16) static uint8_t stack[8192];

static stivale2_header_tag_smp smp_tag = {
  .tag = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = 0},
  .flags = 1 << 1,
};

static stivale2_header_tag_framebuffer framebuffer_tag = {
  .tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = (uintptr_t) &smp_tag},
  .framebuffer_width = 0,
  .framebuffer_height = 0,
  .framebuffer_bpp = 0,
  .unused = 0,
};

__attribute__((section(".stivale2hdr"), used)) //
static stivale2_header stivale_hdr = {
  .entry_point = 0,
  .stack = (uintptr_t) stack + sizeof(stack),
  .flags = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4,
  .tags = (uintptr_t) &framebuffer_tag,
};

template <typename T>
static T *find_tag(stivale2_struct *boot_info, uint64_t id) {
  auto tag = (stivale2_tag *) boot_info->tags;

  while (tag) {
    if (tag->identifier == id)
      return (T *) tag;

    tag = (stivale2_tag *) tag->next;
  }

  return nullptr;
}

extern "C" void kernel_bsp_main(stivale2_struct *boot_info) {
  cpu::init_bsp();

  arch::init_gdt();
  arch::init_idt();

  auto pmrs_tag = find_tag<stivale2_struct_tag_pmrs>(boot_info, STIVALE2_STRUCT_TAG_PMRS_ID);
  auto kernel_base_tag = find_tag<stivale2_struct_tag_kernel_base_address>(boot_info, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
  auto mmap_tag = find_tag<stivale2_struct_tag_memmap>(boot_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
  auto modules_tag = find_tag<stivale2_struct_tag_modules>(boot_info, STIVALE2_STRUCT_TAG_MODULES_ID);
  auto kernel_file_tag = find_tag<stivale2_struct_tag_kernel_file>(boot_info, STIVALE2_STRUCT_TAG_KERNEL_FILE_ID);
  auto hhdm_tag = find_tag<stivale2_struct_tag_hhdm>(boot_info, STIVALE2_STRUCT_TAG_HHDM_ID);
  auto rsdp_tag = find_tag<stivale2_struct_tag_rsdp>(boot_info, STIVALE2_STRUCT_TAG_RSDP_ID);
  auto smp_tag = find_tag<stivale2_struct_tag_smp>(boot_info, STIVALE2_STRUCT_TAG_SMP_ID);

  assert(pmrs_tag && kernel_base_tag && mmap_tag && modules_tag && hhdm_tag && rsdp_tag && smp_tag);

  if (kernel_file_tag)
    utils::load_kernel_symbols((elf64_header_t *) kernel_file_tag->kernel_file);

  mem::init_pmm(mmap_tag);
  mem::init_paging(pmrs_tag, kernel_base_tag, hhdm_tag);
  mem::init_heap();

  arch::init_tss();
  acpi::init(rsdp_tag);
  apic::init();

  scheduler::init();

  asm("sti" ::: "memory");

  while (1) {
    asm("hlt");
  }
}
