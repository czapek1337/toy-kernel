#include <arch/port.h>

extern "C" void port_out8(uint16_t port, uint8_t data);
extern "C" void port_out16(uint16_t port, uint16_t data);
extern "C" void port_out32(uint16_t port, uint32_t data);

extern "C" uint8_t port_in8(uint16_t port);
extern "C" uint16_t port_in16(uint16_t port);
extern "C" uint32_t port_in32(uint16_t port);

void Port::out8(uint8_t value) {
  port_out8(m_port, value);
}

void Port::out16(uint16_t value) {
  port_out16(m_port, value);
}

void Port::out32(uint32_t value) {
  port_out32(m_port, value);
}

uint8_t Port::read8() const {
  return port_in8(m_port);
}

uint16_t Port::read16() const {
  return port_in16(m_port);
}

uint32_t Port::read32() const {
  return port_in32(m_port);
}
