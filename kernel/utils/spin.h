#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace utils {

  class spin_lock_t {
  public:
    bool try_lock();

    void lock();
    void unlock();

  private:
    uint64_t m_lock;
  };

  template <typename T>
  class spin_lock_guard_t {
  public:
    spin_lock_guard_t(T &lock) : m_lock(&lock) {
      m_lock->lock();
    }

    ~spin_lock_guard_t() {
      m_lock->unlock();
    }

  private:
    T *m_lock;
  };

} // namespace utils
