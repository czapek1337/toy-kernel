#include <arch/msr.h>

void Msr::write(uint64_t value) {
  uint32_t low = value & 0xffffffff;
  uint32_t high = value >> 32;

  asm("wrmsr" : : "a"(low), "d"(high), "c"(m_msr));
}

uint64_t Msr::read() const {
  uint32_t low, high;

  asm("rdmsr" : "=a"(low), "=d"(high) : "c"(m_msr));

  return ((uint64_t) high << 32) | low;
}
