#include <stdlib.h>

#include "ll.h"

void ll_insert(ll_node_t *node, ll_node_t *prev, ll_node_t *next) {
  next->prev = node;
  prev->next = node;

  node->prev = prev;
  node->next = next;
}

void ll_add_front(ll_node_t *node, ll_node_t *head) {
  ll_insert(node, head, head->next);
}

void ll_add_back(ll_node_t *node, ll_node_t *head) {
  ll_insert(node, head->prev, head);
}

void ll_join(ll_node_t *prev, ll_node_t *next) {
  next->prev = prev;
  prev->next = next;
}

void ll_delete(ll_node_t *node) {
  ll_join(node->prev, node->next);

  node->prev = NULL;
  node->next = NULL;
}
