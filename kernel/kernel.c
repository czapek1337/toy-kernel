#include "arch/idt.h"
#include "boot/stivale2.h"
#include "mem/heap.h"
#include "mem/phys.h"
#include "mem/virt.h"
#include "utils/print.h"

__attribute__((aligned(16))) //
static uint8_t stack[8192];

static struct stivale2_header_tag_framebuffer framebuffer_tag = {
  .tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = 0},
  .framebuffer_width = 0,
  .framebuffer_height = 0,
  .framebuffer_bpp = 0,
};

__attribute__((section(".stivale2hdr"), used)) //
static struct stivale2_header stivale_hdr = {
  .entry_point = 0,
  .stack = (uint64_t) stack + sizeof(stack),
  .flags = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4,
  .tags = (uint64_t) &framebuffer_tag,
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

void kernel_main(struct stivale2_struct *boot_info) {
  struct stivale2_struct_tag_pmrs *pmrs_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_PMRS_ID);
  struct stivale2_struct_tag_kernel_base_address *kernel_base_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
  struct stivale2_struct_tag_memmap *mmap_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
  struct stivale2_struct_tag_kernel_file *kernel_file_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_KERNEL_FILE_ID);
  struct stivale2_struct_tag_hhdm *hhdm_tag = find_tag(boot_info, STIVALE2_STRUCT_TAG_HHDM_ID);

  assert_msg(pmrs_tag && kernel_base_tag && mmap_tag && hhdm_tag, "Cannot proceed without one or more required struct tags");

  if (kernel_file_tag)
    panic_load_symbols((elf64_header_t *) kernel_file_tag->kernel_file);

  idt_init();
  phys_init(mmap_tag);
  virt_init(pmrs_tag, kernel_base_tag, hhdm_tag);
  heap_init();

  asm("int $0x80");

  klog_info("Hello, world!");

  while (1) {
    asm("hlt");
  }
}
