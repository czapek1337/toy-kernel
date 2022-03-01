#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *str) {
  size_t len = 0;

  while (str[len]) {
    len++;
  }

  return len;
}

void *memset(void *dest, int value, size_t count) {
  uint8_t *dest_u8 = dest;

  for (size_t i = 0; i < count; i++) {
    dest_u8[i] = value;
  }

  return dest;
}

void *memcpy(void *dest, const void *source, size_t count) {
  uint8_t *dest_u8 = dest;
  uint8_t *source_u8 = (void *) source;

  for (size_t i = 0; i < count; i++) {
    dest_u8[i] = source_u8[i];
  }

  return dest;
}

int memcmp(const void *mem1, const void *mem2, size_t size) {
  const uint8_t *mem1_u8 = mem1;
  const uint8_t *mem2_u8 = mem2;

  for (size_t i = 0; i < size; i++) {
    if (mem1_u8[i] != mem2_u8[i])
      return mem1_u8[i] - mem2_u8[i];
  }

  return 0;
}
