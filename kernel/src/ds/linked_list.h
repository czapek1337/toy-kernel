#pragma once

// This is an adapted version of Embedded Artistry's circular doubly linked list implementation that's
// contained in their libmemory repository: https://github.com/embeddedartistry/libmemory/

template <typename T>
class linked_list_node_t {
private:
    T *m_prev;
    T *m_next;

public:
    linked_list_node_t() = default;
    linked_list_node_t(T *prev, T *next) : m_prev(prev), m_next(next) {}

    constexpr T *prev() { return m_prev; }
    constexpr T *next() { return m_next; }

    constexpr void insert(T *prev, T *next) {
        m_prev = prev;
        m_next = next;

        next->m_prev = (T *) this;
        prev->m_next = (T *) this;
    }

    constexpr void insert_front(T *head) { insert(head, head->m_next); }
    constexpr void insert_back(T *head) { insert(head->m_prev, head); }

    constexpr void remove() {
        m_next->m_prev = m_prev;
        m_prev->m_next = m_next;

        m_prev = nullptr;
        m_next = nullptr;
    }
};
