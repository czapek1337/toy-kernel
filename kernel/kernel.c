#include "acpi/acpi.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "boot/stivale2.h"
#include "interrupts/apic.h"
#include "mem/heap.h"
#include "mem/phys.h"
#include "mem/virt.h"
#include "proc/cpu.h"
#include "proc/sched.h"
#include "proc/smp.h"
#include "utils/print.h"

__attribute__((aligned(16))) //
static uint8_t stack[8192];

static struct stivale2_header_tag_smp smp_tag = {
  .tag = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = 0},
  .flags = 1 << 1,
};

static struct stivale2_header_tag_framebuffer framebuffer_tag = {
  .tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = (uintptr_t) &smp_tag},
  .framebuffer_width = 0,
  .framebuffer_height = 0,
  .framebuffer_bpp = 0,
};

__attribute__((section(".stivale2hdr"), used)) //
static struct stivale2_header stivale_hdr = {
  .entry_point = 0,
  .stack = (uintptr_t) stack + sizeof(stack),
  .flags = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4,
  .tags = (uintptr_t) &framebuffer_tag,
};

static void *find_tag(struct stivale2_struct *boot_info, uint64_t id) {
  struct stivale2_tag *tag = (void *) boot_info->tags;

  while (tag) {
    if (tag->identifier == id)
      return tag;

    tag = (void *) tag->next;
  }

  return tag;
}

void kernel_bsp_main(struct stivale2_struct *boot_info) {
  gdt_init();
  idt_init();
  cpu_init_bsp();

  struct stivale2_struct_tag_pmrs *pmrs_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_PMRS_ID);
  struct stivale2_struct_tag_kernel_base_address *kernel_base_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
  struct stivale2_struct_tag_memmap *mmap_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
  struct stivale2_struct_tag_kernel_file *kernel_file_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_KERNEL_FILE_ID);
  struct stivale2_struct_tag_hhdm *hhdm_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_HHDM_ID);
  struct stivale2_struct_tag_rsdp *rsdp_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_RSDP_ID);
  struct stivale2_struct_tag_smp *smp_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_SMP_ID);

  assert_msg(pmrs_tag && kernel_base_tag && mmap_tag && hhdm_tag && rsdp_tag && smp_tag, //
             "Cannot proceed without one or more required struct tags");

  if (kernel_file_tag)
    panic_load_symbols((elf64_header_t *) kernel_file_tag->kernel_file);

  phys_init(mmap_tag);
  virt_init(pmrs_tag, kernel_base_tag, hhdm_tag);
  heap_init();
  gdt_init_tss();

  acpi_init(rsdp_tag);

  // TODO: Make the scheduler SMP aware :^)
  // smp_init(smp_tag);

  sched_init();
  apic_init();

  while (1) {
    asm("hlt");
  }
}

void kernel_ap_main(struct stivale2_smp_info *ap_info) {
  idt_init();
  gdt_init();

  smp_init_cpu(ap_info);
  gdt_init_tss();
  apic_init();

  while (1) {
    asm("hlt");
  }
}
