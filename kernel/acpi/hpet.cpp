#include <acpi/hpet.h>
#include <mem/phys.h>
#include <mem/virt.h>
#include <utils/print.h>

#define HPET_GENERAL_CAPS_AND_ID 0x00
#define HPET_GENERAL_CONFIG 0x10
#define HPET_MAIN_COUNTER 0xf0

static uintptr_t hpet_base;
static size_t hpet_period;

static uint64_t hpet_read(size_t reg) {
  return *(volatile uint64_t *) mem::phys_to_virt(hpet_base + reg);
}

static void hpet_write(size_t reg, uint64_t val) {
  *(volatile uint64_t *) mem::phys_to_virt(hpet_base + reg) = val;
}

void acpi::init_hpet(hpet_header_t *table) {
  kassert_msg(table->address.address_space_id == 0, "Unsupported HPET address space: %d", table->address.address_space_id);

  hpet_base = table->address.address;
  hpet_period = hpet_read(HPET_GENERAL_CAPS_AND_ID) >> 32;

  klog_info("HPET period is equal to %dns", hpet_period / 1000000);

  hpet_write(HPET_GENERAL_CONFIG, 0);
  hpet_write(HPET_MAIN_COUNTER, 0);
  hpet_write(HPET_GENERAL_CONFIG, 1);
}

void acpi::sleep(size_t nanoseconds) {
  auto target = hpet_read(HPET_MAIN_COUNTER) + (nanoseconds * 1000000) / hpet_period;

  while (hpet_read(HPET_MAIN_COUNTER) < target) {
    // Spin! :D
  }
}
