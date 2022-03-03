#include <stddef.h>
#include <stdint.h>

extern "C" size_t strlen(const char *str) {
  size_t len = 0;

  while (str[len]) {
    len++;
  }

  return len;
}

extern "C" void *memset(void *dest, int value, size_t count) {
  auto dest_u8 = (uint8_t *) dest;

  for (auto i = 0u; i < count; i++) {
    dest_u8[i] = value;
  }

  return dest;
}

extern "C" void *memcpy(void *dest, const void *source, size_t count) {
  auto dest_u8 = (uint8_t *) dest;
  auto source_u8 = (uint8_t *) source;

  for (auto i = 0u; i < count; i++) {
    dest_u8[i] = source_u8[i];
  }

  return dest;
}

extern "C" int memcmp(const void *mem1, const void *mem2, size_t size) {
  auto mem1_u8 = (uint8_t *) mem1;
  auto mem2_u8 = (uint8_t *) mem2;

  for (auto i = 0u; i < size; i++) {
    if (mem1_u8[i] != mem2_u8[i])
      return mem1_u8[i] - mem2_u8[i];
  }

  return 0;
}
