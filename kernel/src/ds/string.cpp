#include "string.h"
#include "../lib/log.h"

string_t::string_t() {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

string_t::string_t(const char *data) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(data);
}

string_t::string_t(const string_t &other) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(other.m_data, other.m_size);
}

string_t::string_t(string_t &&other) {
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
}

string_t::string_t(const char *data, uint64_t length) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(data, length);
}

string_t &string_t::operator=(const string_t &other) {
    if (m_data)
        delete[] m_data;

    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;

    push(other.m_data, other.m_size);

    return *this;
}

string_t &string_t::operator=(string_t &&other) {
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

string_t::~string_t() {
    if (m_data)
        delete[] m_data;
}

bool string_t::operator==(const string_t &other) const {
    if (m_size != other.m_size)
        return false;

    return __builtin_strncmp(m_data, other.m_data, m_size) == 0;
}

bool string_t::operator!=(const string_t &other) const {
    return !(*this == other);
}

void string_t::push(char ch) {
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

void string_t::push(const char *data) {
    push(data, __builtin_strlen(data));
}

void string_t::push(const char *data, uint64_t length) {
    for (auto i = 0; i < length; i++) {
        push(data[i]);
    }
}

uint64_t string_t::size() const {
    return m_size;
}

uint64_t string_t::capacity() const {
    return m_capacity;
}

const char *string_t::data() const {
    return m_data;
}

template <>
void formatter_t<string_t>::format(const string_t &value, const format_options_t &options) {
    detail::format_arg(value.data(), options);
}
