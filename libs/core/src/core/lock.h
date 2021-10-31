#pragma once

#include <stdint.h>

namespace core {

template <typename T>
concept BasicLockable = requires(T &lock) {
    {lock.lock()};
    {lock.unlock()};
};

class SpinLock {
private:
    uint8_t m_value;

    bool try_lock();

public:
    void lock();
    void unlock();
};

template <BasicLockable T>
class LockGuard {
private:
    T &m_lock;

public:
    LockGuard(T &lock) : m_lock(lock) { m_lock.lock(); }
    ~LockGuard() { m_lock.unlock(); }
};

} // namespace core
