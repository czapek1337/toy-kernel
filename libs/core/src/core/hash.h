#pragma once

#include <stdint.h>

namespace core {

template <typename T>
struct Hashable {
    static uint64_t hash(const T &value);
};

#define MAKE_INT_HASH(type)                                                                                                                \
    template <>                                                                                                                            \
    struct Hashable<type> {                                                                                                              \
        static uint64_t hash(const type &value) { return (uint64_t) value; }                                                               \
    };

MAKE_INT_HASH(int8_t)
MAKE_INT_HASH(int16_t)
MAKE_INT_HASH(int32_t)
MAKE_INT_HASH(int64_t)

MAKE_INT_HASH(uint8_t)
MAKE_INT_HASH(uint16_t)
MAKE_INT_HASH(uint32_t)
MAKE_INT_HASH(uint64_t)

#undef MAKE_INT_HASH

} // namespace core
