#pragma once

namespace utils {

  template <typename T>
  class linked_list_node_t {
  private:
    T *m_prev;
    T *m_next;

  public:
    linked_list_node_t() = default;
    linked_list_node_t(T *prev, T *next) : m_prev(prev), m_next(next) {
    }

    T *prev() {
      return m_prev;
    }

    T *next() {
      return m_next;
    }

    void insert(T *prev, T *next) {
      m_prev = prev;
      m_next = next;

      next->m_prev = (T *) this;
      prev->m_next = (T *) this;
    }

    void insert_front(T *head) {
      insert(head, head->m_next);
    }

    void insert_back(T *head) {
      insert(head->m_prev, head);
    }

    void remove() {
      m_next->m_prev = m_prev;
      m_prev->m_next = m_next;

      m_prev = nullptr;
      m_next = nullptr;
    }
  };

} // namespace utils
