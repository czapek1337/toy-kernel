#pragma once

#include <stddef.h>
#include <stdint.h>

#include <utils/print.h>

namespace utils {

  template <typename T>
  class vector_t {
  public:
    vector_t(size_t capacity = 0) {
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

        for (auto i = 0u; i < m_count; i++)
          new_values[i] = m_values[i];

        if (m_values)
          delete[] m_values;

        m_values = new_values;
      }

      m_values[m_count++] = value;
    }

    void pop() {
      if (m_count == 0)
        klog_panic("Tried to pop a value from an empty vector");

      m_count--;
    }

    void remove(T value) {
      for (auto i = 0u; i < m_count; i++) {
        if (m_values[i] == value) {
          for (auto j = i; j < m_count - 1; j++)
            m_values[j] = m_values[j + 1];

          m_count--;

          break;
        }
      }
    }
    T &operator[](uint64_t index) {
      return m_values[index];
    }

    const T &operator[](uint64_t index) const {
      return m_values[index];
    }

    T &at(uint64_t index) {
      return m_values[index];
    }

    const T &at(uint64_t index) const {
      return m_values[index];
    }

    size_t size() const {
      return m_count;
    }

    size_t capacity() const {
      return m_capacity;
    }

  private:
    T *m_values;

    size_t m_capacity;
    size_t m_count;
  };

} // namespace utils
