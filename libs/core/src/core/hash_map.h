#pragma once

#include <stdint.h>

#include "hash.h"
#include "vector.h"

namespace core {

template <typename K, typename V>
class HashMap {
private:
    Vector<uint64_t> m_keys;
    Vector<V> m_values;

public:
    HashMap() = default;
    HashMap(const HashMap<K, V> &other) = default;
    HashMap(HashMap<K, V> &&other) = default;

    HashMap<K, V> &operator=(const HashMap<K, V> &other) = default;
    HashMap<K, V> &operator=(HashMap<K, V> &&other) = default;

    V &lookup(const K &key) {
        auto hash = Hashable<K>::hash(key);

        for (auto i = 0; i < m_keys.size(); i++) {
            if (m_keys[i] == hash)
                return m_values[i];
        }

        m_keys.push(hash);
        m_values.push(V());

        return m_values[m_values.size() - 1];
    }

    V &operator[](const K &key) { return lookup(key); }

    void insert(const K &key, const V &value) {
        auto hash = Hashable<K>::hash(key);

        for (auto i = 0; i < m_keys.size(); i++) {
            if (m_keys[i] == hash) {
                m_values[i] = value;
                return;
            }
        }

        m_keys.push(hash);
        m_values.push(value);
    }

    void remove(const K &key) {
        auto hash = Hashable<K>::hash(key);

        for (auto i = 0; i < m_keys.size(); i++) {
            if (m_keys[i] == hash) {
                m_keys.remove(hash);
                m_values.remove(m_values[i]);

                return;
            }
        }
    }

    bool contains(const K &key) const {
        auto hash = Hashable<K>::hash(key);

        for (auto i = 0; i < m_keys.size(); i++) {
            if (m_keys[i] == hash)
                return true;
        }

        return false;
    }

    uint64_t size() const { return m_keys.size(); }
    uint64_t capacity() const { return m_keys.capacity(); }
};

} // namespace core
