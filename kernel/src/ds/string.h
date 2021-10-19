#pragma once

#include <stdint.h>

class string_t {
private:
    char *m_data;

    uint64_t m_size;
    uint64_t m_capacity;

public:
    string_t();
    string_t(const string_t &other);
    string_t(string_t &&other);

    string_t &operator=(const string_t &other);
    string_t &operator=(string_t &&other);

    string_t(const char *data);
    string_t(const char *data, uint64_t length);

    ~string_t();

    bool operator==(const string_t &other) const;
    bool operator!=(const string_t &other) const;

    void push(char ch);
    void push(const char *data);
    void push(const char *data, uint64_t length);

    uint64_t size() const;
    uint64_t capacity() const;

    const char *data() const;
};
