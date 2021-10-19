#pragma once

#include <stdint.h>

template <typename T>
class vector_t {
private:
    T *m_values;

    uint64_t m_capacity;
    uint64_t m_count;

public:
    vector_t(uint64_t capacity = 0) {
        m_values = nullptr;
        m_capacity = capacity;
        m_count = 0;
    }

    ~vector_t() {
        if (m_values)
            delete[] m_values;
    }

    void push(T value) {
        if (m_count >= m_capacity) {
            if (m_capacity == 0)
                m_capacity = 1;

            m_capacity *= 2;

            auto new_values = new T[m_capacity];

            for (auto i = 0; i < m_count; i++)
                new_values[i] = m_values[i];

            if (m_values)
                delete[] m_values;

            m_values = new_values;
        }

        m_values[m_count++] = value;
    }

    void pop() {
        if (m_count == 0)
            return;

        m_count--;
    }

    void remove(T value) {
        for (auto i = 0; i < m_count; i++) {
            if (m_values[i] == value) {
                for (auto j = i; j < m_count - 1; j++)
                    m_values[j] = m_values[j + 1];

                m_count--;

                break;
            }
        }
    }

    T &operator[](uint64_t index) { return m_values[index]; }
    T &at(uint64_t index) { return m_values[index]; }

    const T &operator[](uint64_t index) const { return m_values[index]; }
    const T &at(uint64_t index) const { return m_values[index]; }

    uint64_t size() const { return m_count; }
    uint64_t capacity() const { return m_capacity; }
};
