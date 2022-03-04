#include <string.h>

#include <arch/msr.h>
#include <mem/heap.h>
#include <mem/virt.h>
#include <proc/cpu.h>

static cpu::cpu_info_t bsp_cpu;

void cpu::init_bsp() {
  bsp_cpu.self = &bsp_cpu;
  bsp_cpu.id = 0;
  bsp_cpu.lapic_id = 0;

  Msr::gs_base().write((uintptr_t) &bsp_cpu);
  Msr::gs_kernel_base().write((uintptr_t) &bsp_cpu);
}

void cpu::init_ap(size_t id, size_t lapic_id) {
  auto cpu = frg::construct<cpu_info_t>(mem::kernel_allocator);

  cpu->self = cpu;
  cpu->id = id;
  cpu->lapic_id = lapic_id;

  Msr::gs_base().write((uintptr_t) cpu);
  Msr::gs_kernel_base().write((uintptr_t) cpu);
}

cpu::cpu_info_t *cpu::get() {
  cpu_info_t *cpu;

  asm("mov %%gs:0, %0" : "=r"(cpu));

  return cpu;
}
