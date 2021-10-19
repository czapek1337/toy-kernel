#include <stdint.h>

extern "C" uint64_t strlen(const char *str) {
    auto length = 0;

    while (*str++) {
        length++;
    }

    return length;
}

extern "C" uint64_t strcmp(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        str1++;
        str2++;
    }

    return *str1 - *str2;
}

extern "C" uint64_t strncmp(const char *str1, const char *str2, uint64_t count) {
    while (count--) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        str1++;
        str2++;
    }

    return 0;
}
