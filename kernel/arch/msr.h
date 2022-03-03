#pragma once

#include <stdint.h>

class Msr {
public:
  Msr(uint32_t msr) : m_msr(msr) {
  }

  static Msr apic() {
    return Msr(0x1b);
  }

  static Msr fs_base() {
    return Msr(0xc0000100);
  }

  static Msr gs_base() {
    return Msr(0xc0000101);
  }

  static Msr gs_kernel_base() {
    return Msr(0xc0000102);
  }

  void write(uint64_t value);

  uint64_t read() const;

private:
  uint32_t m_msr;
};
