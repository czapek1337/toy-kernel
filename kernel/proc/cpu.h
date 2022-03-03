#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../arch/gdt.h"
#include "../proc/thread.h"

typedef struct cpu_info {
  struct cpu_info *self;

  size_t id;
  size_t lapic_id;

  thread_t *thread;
  tss_t tss;
} cpu_info_t;

void cpu_init_bsp();
void cpu_init(size_t id, size_t lapic_id);

cpu_info_t *cpu_get();
