#include "bitmap.h"

#define BM_VALUE_INDEX(x) (x / 8)
#define BM_VALUE_MASK(x) (1 << (x % 8))

Bitmap::Bitmap(uint8_t *bitmap, uint64_t size) {
    m_bitmap = bitmap;
    m_size = size;
}

bool Bitmap::test(uint64_t index, bool value) {
    auto mask = BM_VALUE_MASK(index);
    auto masked = m_bitmap[BM_VALUE_INDEX(index)] & mask;

    return value ? masked == mask : masked == 0;
}

bool Bitmap::test_range(uint64_t index, uint64_t size, bool value) {
    for (auto i = 0; i < size; i++) {
        if (!test(index + i, value))
            return false;
    }

    return true;
}

void Bitmap::set(uint64_t index, bool value) {
    if (value)
        m_bitmap[BM_VALUE_INDEX(index)] |= BM_VALUE_MASK(index);
    else
        m_bitmap[BM_VALUE_INDEX(index)] &= ~BM_VALUE_MASK(index);
}

void Bitmap::set_range(uint64_t index, uint64_t size, bool value) {
    for (auto i = 0; i < size; i++) {
        set(index + i, value);
    }
}

uint64_t Bitmap::find_first(uint64_t index, bool value) {
    for (auto i = index; i < m_size * 8; i++) {
        if (test(i, value))
            return i;
    }

    return ~0;
}

uint64_t Bitmap::find_first_range(uint64_t index, uint64_t size, bool value) {
    for (auto i = index; i < m_size * 8 - size; i++) {
        if (test_range(i, size, value))
            return i;
    }

    return ~0;
}
