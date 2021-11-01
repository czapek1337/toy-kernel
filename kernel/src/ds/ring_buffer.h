#pragma once

#include <stdint.h>

template <typename T, uint64_t N>
class RingBuffer {
private:
    T m_buffer[N];

    uint64_t m_head;
    uint64_t m_tail;

public:
    RingBuffer() : m_head(0), m_tail(0) {}

    bool empty() const { return m_head == m_tail; }

    void push(T value) {
        m_buffer[m_head] = value;
        m_head = (m_head + 1) % N;
    }

    T read() {
        auto value = m_buffer[m_tail];

        m_tail = (m_tail + 1) % N;

        return value;
    }
};
