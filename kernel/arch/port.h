#pragma once

#include <stdint.h>

class Port {
public:
  Port(uint16_t port) : m_port(port) {
  }

  void out8(uint8_t value);
  void out16(uint16_t value);
  void out32(uint32_t value);

  uint8_t read8() const;
  uint16_t read16() const;
  uint32_t read32() const;

private:
  uint16_t m_port;
};
