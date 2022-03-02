#include "cpu.h"
#include "../arch/msr.h"
#include "../mem/heap.h"
#include "../mem/virt.h"

cpu_info_t bsp_cpu;

void cpu_init_bsp() {
  bsp_cpu.self = &bsp_cpu;
  bsp_cpu.id = 0;
  bsp_cpu.lapic_id = 0;

  msr_write(MSR_GS_BASE, (uint64_t) &bsp_cpu);
  msr_write(MSR_GS_KERNEL_BASE, (uint64_t) &bsp_cpu);
}

void cpu_init(size_t id, size_t lapic_id) {
  cpu_info_t *cpu = heap_alloc(sizeof(cpu_info_t));

  cpu->self = cpu;
  cpu->id = id;
  cpu->lapic_id = lapic_id;

  msr_write(MSR_GS_BASE, (uint64_t) cpu);
  msr_write(MSR_GS_KERNEL_BASE, (uint64_t) cpu);
}

cpu_info_t *cpu_get() {
  cpu_info_t *cpu;

  asm("mov %%gs:0, %0" : "=r"(cpu));

  return cpu;
}
