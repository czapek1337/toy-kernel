#pragma once

#include <stdint.h>

namespace core {

template <typename T>
concept basic_lockable = requires(T &lock) {
    {lock.lock()};
    {lock.unlock()};
};

class lock_t {
private:
    uint8_t m_value;

    bool try_lock();

public:
    void lock();
    void unlock();
};

template <basic_lockable T>
class lock_guard_t {
private:
    T &m_lock;

public:
    lock_guard_t(T &lock) : m_lock(lock) { m_lock.lock(); }
    ~lock_guard_t() { m_lock.unlock(); }
};

} // namespace core
