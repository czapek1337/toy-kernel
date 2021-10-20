#pragma once

#include <stdint.h>

#include "hash.h"
#include "vector.h"

namespace core {

template <typename K, typename V>
class hash_map_t {
private:
    vector_t<uint64_t> m_keys;
    vector_t<V> m_values;

public:
    hash_map_t() = default;
    hash_map_t(const hash_map_t<K, V> &other) = default;
    hash_map_t(hash_map_t<K, V> &&other) = default;

    hash_map_t<K, V> &operator=(const hash_map_t<K, V> &other) = default;
    hash_map_t<K, V> &operator=(hash_map_t<K, V> &&other) = default;

    V &lookup(const K &key) {
        auto hash = hashable_t<K>::hash(key);

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
        auto hash = hashable_t<K>::hash(key);

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
        auto hash = hashable_t<K>::hash(key);

        for (auto i = 0; i < m_keys.size(); i++) {
            if (m_keys[i] == hash) {
                m_keys.remove(hash);
                m_values.remove(m_values[i]);

                return;
            }
        }
    }

    bool contains(const K &key) const {
        auto hash = hashable_t<K>::hash(key);

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
