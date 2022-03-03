#include <stdint.h>

#include <arch/gdt.h>
#include <mem/phys.h>
#include <mem/virt.h>
#include <proc/cpu.h>
#include <utils/print.h>

struct packed gdtr_t {
  uint16_t limit;
  uint64_t base;
};

void arch::init_gdt() {
  auto cpu = cpu::get();

  // thanks @mintsuki (https://github.com/limine-bootloader/limine/blob/trunk/common/sys/gdt.s2.c)
  cpu->gdt.entries[GDT_CODE_16 / 8] = {0xffff, 0x0000, 0x00, 0b10011010, 0b00000000, 0x00};
  cpu->gdt.entries[GDT_DATA_16 / 8] = {0xffff, 0x0000, 0x00, 0b10010010, 0b00000000, 0x00};
  cpu->gdt.entries[GDT_CODE_32 / 8] = {0xffff, 0x0000, 0x00, 0b10011010, 0b11001111, 0x00};
  cpu->gdt.entries[GDT_DATA_32 / 8] = {0xffff, 0x0000, 0x00, 0b10010010, 0b11001111, 0x00};
  cpu->gdt.entries[GDT_CODE_64 / 8] = {0x0000, 0x0000, 0x00, 0b10011010, 0b00100000, 0x00};
  cpu->gdt.entries[GDT_DATA_64 / 8] = {0x0000, 0x0000, 0x00, 0b10010010, 0b00000000, 0x00};
  cpu->gdt.entries[GDT_USER_CODE_64 / 8] = {0x0000, 0x0000, 0x00, 0b11111010, 0b00100000, 0x00};
  cpu->gdt.entries[GDT_USER_DATA_64 / 8] = {0x0000, 0x0000, 0x00, 0b11110010, 0b00000000, 0x00};

  gdtr_t gdtr;

  gdtr.limit = sizeof(cpu->gdt) - 1;
  gdtr.base = (uintptr_t) &cpu->gdt;

  asm("lgdt %0" : : "m"(gdtr) : "memory");
}

void arch::init_tss() {
  auto cpu = cpu::get();
  auto tss_address = (uintptr_t) &cpu->tss;

  cpu->gdt.tss_entry.low = {
    .limit = sizeof(arch::tss_t) - 1,
    .base_low = (uint16_t) tss_address,
    .base_mid = (uint8_t) (tss_address >> 16),
    .flags = 0b10001001,
    .granularity = 0x00,
    .base_high = (uint8_t) (tss_address >> 24),
  };

  cpu->gdt.tss_entry.base_high_ex = (uint32_t) (tss_address >> 32);
  cpu->gdt.tss_entry.reserved = 0;
  cpu->tss.rsp[0] = mem::phys_to_virt(mem::phys_alloc(2)) + KIB(8);

  asm("ltr %0" : : "r"((uint16_t) GDT_TSS));
}
