#include "string.h"

core::String::String() {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

core::String::String(const char *data) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(data);
}

core::String::String(const String &other) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(other.m_data, other.m_size);
}

core::String::String(String &&other) {
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
}

core::String::String(const char *data, uint64_t length) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(data, length);
}

core::String &core::String::operator=(const String &other) {
    if (m_data)
        delete[] m_data;

    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(other.m_data, other.m_size);

    return *this;
}

core::String &core::String::operator=(String &&other) {
    if (m_data)
        delete[] m_data;

    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;

    return *this;
}

core::String::~String() {
    if (m_data)
        delete[] m_data;
}

bool core::String::operator==(const String &other) const {
    if (m_size != other.m_size)
        return false;

    return __builtin_strncmp(m_data, other.m_data, m_size) == 0;
}

bool core::String::operator!=(const String &other) const {
    return !(*this == other);
}

void core::String::push(char ch) {
    if (m_size >= m_capacity) {
        if (m_capacity == 0)
            m_capacity = 1;

        m_capacity *= 2;

        auto new_values = new char[m_capacity];

        __builtin_memset(new_values, 0, m_capacity);

        if (m_data)
            __builtin_memcpy(new_values, m_data, m_size);

        if (m_data)
            delete[] m_data;

        m_data = new_values;
    }

    m_data[m_size++] = ch;
}

void core::String::push(const char *data) {
    push(data, __builtin_strlen(data));
}

void core::String::push(const char *data, uint64_t length) {
    for (auto i = 0; i < length; i++) {
        push(data[i]);
    }
}

uint64_t core::String::size() const {
    return m_size;
}

uint64_t core::String::capacity() const {
    return m_capacity;
}

const char *core::String::data() const {
    return m_data;
}
