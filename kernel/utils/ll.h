#pragma once

typedef struct ll_head_t {
  struct ll_head_t *prev;
  struct ll_head_t *next;
} ll_node_t;

#define offset_of(type, member) ((size_t) & ((type *) 0)->member)
#define container_of(ptr, type, member) ((type *) ((uintptr_t) (ptr) -offset_of(type, member)))

#define ll_entry(ptr, type, member) container_of(ptr, type, member)
#define ll_first_entry(head, type, member) ll_entry((head)->next, type, member)
#define ll_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)
#define ll_for_each_safe(pos, node, head) for (pos = (head)->next, node = pos->next; pos != (head); pos = node, node = pos->next)

#define ll_for_each_entry(pos, head, member)                                                                                               \
  for (pos = ll_entry((head)->next, __typeof__(*pos), member); &pos->member != (head);                                                     \
       pos = ll_entry(pos->member.next, __typeof__(*pos), member))

#define ll_for_each_entry_safe(pos, node, head, member)                                                                                    \
  for (pos = ll_entry((head)->next, __typeof__(*pos), member), node = ll_entry(pos->member.next, __typeof__(*pos), member);                \
       &pos->member != (head); pos = node, node = ll_entry(node->member.next, __typeof__(*node), member))

void ll_insert(ll_node_t *node, ll_node_t *prev, ll_node_t *next);
void ll_add_front(ll_node_t *node, ll_node_t *head);
void ll_add_back(ll_node_t *node, ll_node_t *head);
void ll_join(ll_node_t *prev, ll_node_t *next);
void ll_delete(ll_node_t *node);
