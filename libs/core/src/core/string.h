#pragma once

#include <stdint.h>

namespace core {

class String {
private:
    char *m_data;

    uint64_t m_size;
    uint64_t m_capacity;

public:
    String();
    String(const String &other);
    String(String &&other);

    String &operator=(const String &other);
    String &operator=(String &&other);

    String(const char *data);
    String(const char *data, uint64_t length);

    ~String();

    bool operator==(const String &other) const;
    bool operator!=(const String &other) const;

    void push(char ch);
    void push(const char *data);
    void push(const char *data, uint64_t length);

    uint64_t size() const;
    uint64_t capacity() const;

    const char *data() const;
};

} // namespace core
