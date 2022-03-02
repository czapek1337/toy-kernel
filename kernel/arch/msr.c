#include "msr.h"

void msr_write(uint32_t msr, uint64_t value) {
  uint32_t low = value & 0xffffffff;
  uint32_t high = value >> 32;

  asm("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

uint64_t msr_read(uint32_t msr) {
  uint32_t low, high;

  asm("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

  return ((uint64_t) high << 32) | low;
}
