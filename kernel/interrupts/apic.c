#include "apic.h"
#include "../acpi/hpet.h"
#include "../arch/msr.h"
#include "../mem/phys.h"
#include "../mem/virt.h"
#include "../proc/sched.h"
#include "../utils/print.h"
#include "interrupts.h"

#define LAPIC_CPU_ID 0x20
#define LAPIC_REG_EOI 0xb0
#define LAPIC_REG_ICR0 0x300
#define LAPIC_REG_ICR1 0x310
#define LAPIC_REG_SPURIOUS 0xf0

#define LVT_REG_INIT_COUNT 0x380
#define LVT_REG_CURRENT_COUNT 0x390
#define LVT_REG_TIMER 0x320
#define LVT_REG_DIVIDE 0x3e0

static paddr_t apic_base;
static size_t timer_vec;

static uint32_t apic_read(size_t reg) {
  return *(volatile uint32_t *) phys_to_virt(apic_base + reg);
}

static void apic_write(size_t reg, uint32_t val) {
  *(volatile uint32_t *) phys_to_virt(apic_base + reg) = val;
}

static void apic_timer_handler(isr_frame_t *frame) {
  sched_preempt(frame);
}

static void apic_spurious_irq_handler(isr_frame_t *frame) {
  (void) frame;

  klog_warn("A spurious interrupt was fired, possible hardware failure");
}

void apic_init() {
  apic_base = msr_read(MSR_APIC) & ~0xfff;

  apic_write(LAPIC_REG_SPURIOUS, 0x1ff);

  timer_vec = interrupt_alloc_vec();

  interrupt_register(0xff, apic_spurious_irq_handler);
  interrupt_register(timer_vec, apic_timer_handler);

  apic_timer_init();
}

void apic_timer_init() {
  apic_write(LVT_REG_DIVIDE, 3 /* 16 */);
  apic_write(LVT_REG_INIT_COUNT, 0xffffffff);

  hpet_sleep(1000 * 1000 * 10);
  apic_write(LVT_REG_TIMER, 0x10000 /* masked */);

  size_t ticks_in_10ms = 0xffffffff - apic_read(LVT_REG_CURRENT_COUNT);

  apic_write(LVT_REG_DIVIDE, 3 /* 16 */);
  apic_write(LVT_REG_TIMER, timer_vec | 0x20000 /* periodic on irq 32 */);
  apic_write(LVT_REG_INIT_COUNT, ticks_in_10ms * 10);
}

void apic_eoi() {
  apic_write(LAPIC_REG_EOI, 0);
}

void apic_ipi(size_t lapic_id, uint64_t id) {
  apic_write(LAPIC_REG_ICR1, lapic_id << 24);
  apic_write(LAPIC_REG_ICR0, id | 1 << 14);
}

size_t apic_get_lapic() {
  if (apic_base)
    return apic_read(LAPIC_CPU_ID);

  return 0;
}
