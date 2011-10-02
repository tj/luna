
//
// list.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "list.h"
#include "internal.h"

/*
 * Allocate a new list_t. NULL on failure.
 */

list_t *
list_new() {
  list_t *self = calloc(1, sizeof(list_t));
  if (unlikely(!self)) return NULL;
  return self;
}

/*
 * Free the list.
 */

void
list_destroy(list_t *self) {
  unsigned int len = self->len;
  list_node_t *next;
  list_node_t *curr = self->head;
  while (len--) {
    next = curr->next;
    if (self->free) self->free(curr->val);
    free(curr);
    curr = next;
  }
  free(self);
}

/*
 * Append the given node to the list
 * and return the node, NULL on failure.
 */

list_node_t *
list_rpush(list_t *self, list_node_t *node) {
  if (unlikely(!node)) return NULL;
  if (self->len) {
    node->prev = self->tail;
    node->next = NULL;
    self->tail->next = node;
    self->tail = node;
  } else {
    self->head = self->tail = node;
    node->prev = node->next = NULL;
  }
  ++self->len;
  return node;
}

/*
 * Return / detach the last node in the list, or NULL.
 */

list_node_t *
list_rpop(list_t *self) {
  if (!self->len) return NULL;
  list_node_t *node = self->tail;
  if (--self->len) {
    (self->tail = node->prev)->next = NULL;
  } else {
    self->tail = self->head = NULL;
  }
  node->next = node->prev = NULL;
  return node;
}

/*
 * Return / detach the first node in the list, or NULL.
 */

list_node_t *
list_lpop(list_t *self) {
  if (!self->len) return NULL;
  list_node_t *node = self->head;
  if (--self->len) {
    (self->head = node->next)->prev = NULL;
  } else {
    self->head = self->tail = NULL;
  }
  node->next = node->prev = NULL;
  return node;
}

/*
 * Prepend the given node to the list 
 * and return the node, NULL on failure.
 */

list_node_t *
list_lpush(list_t *self, list_node_t *node) {
  if (!node) return NULL;
  if (self->len) {
    node->next = self->head;
    node->prev = NULL;
    self->head->prev = node;
    self->head = node;
  } else {
    self->head = self->tail = node;
    node->prev = node->next = NULL;
  }
  ++self->len;
  return node;
}

/*
 * Return the node associated to val or NULL.
 */

list_node_t *
list_find(list_t *self, void *val) {
  list_iterator_t *it = list_iterator_new(self, LIST_HEAD);
  list_node_t *node;
  while ((node = list_iterator_next(it))) {
    if (self->match) {
      if (self->match(val, node->val)) {
        list_iterator_destroy(it);
        return node;
      }
    } else {
      if (val == node->val) {
        list_iterator_destroy(it);
        return node;
      }
    }
  }
  list_iterator_destroy(it);
  return NULL;
}

/*
 * Return the node at the given index or NULL.
 */

list_node_t *
list_at(list_t *self, int index) {
  list_direction_t direction = LIST_HEAD;

  if (index < 0) {
    direction = LIST_TAIL;
    index = ~index;
  }

  if (index < self->len) {
    list_iterator_t *it = list_iterator_new(self, direction);
    list_node_t *node = list_iterator_next(it);
    while (index--) {
      node = list_iterator_next(it);
    };
    list_iterator_destroy(it);
    return node;
  }

  return NULL;
}

/*
 * Remove the given node from the list, freeing it and it's value.
 */

void 
list_remove(list_t *self, list_node_t *node) {
  node->prev
    ? (node->prev->next = node->next)
    : (self->head = node->next);
  node->next
    ? (node->next->prev = node->prev)
    : (self->tail = node->prev);
  if (self->free) self->free(node);
  free(node);
  --self->len;
}

/*
 * Allocate a new list_iterator_t. NULL on failure.
 * Accepts a direction, which may be LIST_HEAD or LIST_TAIL.
 */

list_iterator_t *
list_iterator_new(list_t *list, list_direction_t direction) {
  list_node_t *node = direction == LIST_HEAD
    ? list->head
    : list->tail;
  return list_iterator_new_from_node(node, direction);
}

/*
 * Allocate a new list_iterator_t with the given start
 * node. NULL on failure. 
 */

list_iterator_t *
list_iterator_new_from_node(list_node_t *node, list_direction_t direction) {
  list_iterator_t *self = malloc(sizeof(list_iterator_t));
  if (unlikely(!self)) return NULL;
  self->next = node;
  self->direction = direction;
  return self;
}

/*
 * Return the next list_node_t or NULL when no more
 * nodes remain in the list.
 */

list_node_t *
list_iterator_next(list_iterator_t *self) {
  list_node_t *curr = self->next;
  if (curr) {
    self->next = self->direction == LIST_HEAD
      ? curr->next
      : curr->prev;
  }
  return curr;
}

/*
 * Free the list iterator.
 */

void
list_iterator_destroy(list_iterator_t *self) {
  free(self);
}

/*
 * Allocates a new list_node_t. NULL on failure.
 */

list_node_t *
list_node_new(void *val) {
  list_node_t *self = calloc(1, sizeof(list_node_t));
  if (unlikely(!self)) return NULL;
  self->val = val;
  return self;
}
