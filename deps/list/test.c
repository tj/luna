
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "src/list.h"

// Helpers

#define test(fn) \
  puts("... \x1b[33m" # fn "\x1b[0m"); \
  test_##fn();

static int freeProxyCalls = 0;

void
freeProxy(void *val) {
  ++freeProxyCalls;
  free(val);
}

typedef struct {
  char *name;
} User;

static int
User_equal(User *a, User *b) {
  return 0 == strcmp(a->name, b->name); 
}

// Tests

static void
test_list_node_new() {
  char *val = "some value";
  list_node_t *node = list_node_new(val);
  assert(node->val == val);
}

static void
test_list_rpush() {
  // Setup
  list_t *list = list_new();
  list_node_t *a = list_node_new("a");
  list_node_t *b = list_node_new("b");
  list_node_t *c = list_node_new("c");

  // a b c 
  list_rpush(list, a);
  list_rpush(list, b);
  list_rpush(list, c);

  // Assertions
  assert(list->head == a);
  assert(list->tail == c);
  assert(list->len == 3);
  assert(a->next == b);
  assert(a->prev == NULL);
  assert(b->next == c);
  assert(b->prev == a);
  assert(c->next == NULL);
  assert(c->prev == b);
}

static void
test_list_lpush() {
  // Setup
  list_t *list = list_new();
  list_node_t *a = list_node_new("a");
  list_node_t *b = list_node_new("b");
  list_node_t *c = list_node_new("c");

  // c b a 
  list_rpush(list, a);
  list_lpush(list, b);
  list_lpush(list, c);

  // Assertions
  assert(list->head == c);
  assert(list->tail == a);
  assert(list->len == 3);
  assert(a->next == NULL);
  assert(a->prev == b);
  assert(b->next == a);
  assert(b->prev == c);
  assert(c->next == b);
  assert(c->prev == NULL);
}

static void
test_list_at() {
  // Setup
  list_t *list = list_new();
  list_node_t *a = list_node_new("a");
  list_node_t *b = list_node_new("b");
  list_node_t *c = list_node_new("c");

  // a b c
  list_rpush(list, a);
  list_rpush(list, b);
  list_rpush(list, c);

  // Assertions
  assert(a == list_at(list, 0));
  assert(b == list_at(list, 1));
  assert(c == list_at(list, 2));
  assert(NULL == list_at(list, 3));

  assert(c == list_at(list, -1));
  assert(b == list_at(list, -2));
  assert(a == list_at(list, -3));
  assert(NULL == list_at(list, -4));
}

static void
test_list_destroy() {
  // Setup
  list_t *a = list_new();
  list_destroy(a);
  
  // a b c
  list_t *b = list_new();
  list_rpush(b, list_node_new("a"));
  list_rpush(b, list_node_new("b"));
  list_rpush(b, list_node_new("c"));
  list_destroy(b);

  // Assertions
  list_t *c = list_new();
  c->free = freeProxy;
  list_rpush(c, list_node_new(list_node_new("a")));
  list_rpush(c, list_node_new(list_node_new("b")));
  list_rpush(c, list_node_new(list_node_new("c")));
  list_destroy(c);
  assert(freeProxyCalls == 3);
}

static void
test_list_find() {
  // Setup
  list_t *langs = list_new();
  list_node_t *js = list_rpush(langs, list_node_new("js"));
  list_node_t *ruby = list_rpush(langs, list_node_new("ruby"));

  list_t *users = list_new();
  users->match = User_equal;
  User userTJ = { "tj" };
  User userSimon = { "simon" };
  User userTaylor = { "taylor" };
  list_node_t *tj = list_rpush(users, list_node_new(&userTJ));
  list_node_t *simon = list_rpush(users, list_node_new(&userSimon));

  // Assertions
  list_node_t *a = list_find(langs, "js");
  list_node_t *b = list_find(langs, "ruby");
  list_node_t *c = list_find(langs, "foo");
  assert(a == js);
  assert(b == ruby);
  assert(c == NULL);

  a = list_find(users, &userTJ);
  b = list_find(users, &userSimon);
  c = list_find(users, &userTaylor);
  assert(a == tj);
  assert(b == simon);
  assert(c == NULL);
}

static void
test_list_remove() {
  // Setup
  list_t *list = list_new();
  list_node_t *a = list_rpush(list, list_node_new("a"));
  list_node_t *b = list_rpush(list, list_node_new("b"));
  list_node_t *c = list_rpush(list, list_node_new("c"));

  // Assertions
  assert(list->len == 3);

  list_remove(list, b);
  assert(list->len == 2);
  assert(list->head == a);
  assert(list->tail == c);
  assert(a->next == c);
  assert(a->prev == NULL);
  assert(c->next == NULL);
  assert(c->prev == a);

  list_remove(list, a);
  assert(list->len == 1);
  assert(list->head == c);
  assert(list->tail == c);
  assert(c->next == NULL);
  assert(c->prev == NULL);

  list_remove(list, c);
  assert(list->len == 0);
  assert(list->head == NULL);
  assert(list->tail == NULL);
}

static void
test_list_rpop() {
  // Setup
  list_t *list = list_new();
  list_node_t *a = list_rpush(list, list_node_new("a"));
  list_node_t *b = list_rpush(list, list_node_new("b"));
  list_node_t *c = list_rpush(list, list_node_new("c"));

  // Assertions
  assert(3 == list->len);
  
  assert(c == list_rpop(list));
  assert(2 == list->len);
  assert(a == list->head);
  assert(b == list->tail);
  assert(a == b->prev);
  assert(NULL == list->tail->next && "new tail node next is not NULL");
  assert(NULL == c->prev && "detached node prev is not NULL");
  assert(NULL == c->next && "detached node next is not NULL");
  
  assert(b == list_rpop(list));
  assert(1 == list->len);
  assert(a == list->head);
  assert(a == list->tail);
  
  assert(a == list_rpop(list));
  assert(0 == list->len);
  assert(NULL == list->head);
  assert(NULL == list->tail);
  
  assert(NULL == list_rpop(list));
  assert(0 == list->len);
}

static void
test_list_lpop() {
  // Setup
  list_t *list = list_new();
  list_node_t *a = list_rpush(list, list_node_new("a"));
  list_node_t *b = list_rpush(list, list_node_new("b"));
  list_node_t *c = list_rpush(list, list_node_new("c"));

  // Assertions
  assert(3 == list->len);

  assert(a == list_lpop(list));
  assert(2 == list->len);
  assert(b == list->head);
  assert(NULL == list->head->prev && "new head node prev is not NULL");
  assert(NULL == a->prev && "detached node prev is not NULL");
  assert(NULL == a->next && "detached node next is not NULL");
  
  assert(b == list_lpop(list));
  assert(1 == list->len);
  
  assert(c == list_lpop(list));
  assert(0 == list->len);
  assert(NULL == list->head);
  assert(NULL == list->tail);
  
  assert(NULL == list_lpop(list));
  assert(0 == list->len);
}

static void
test_list_iterator_t() {
  // Setup
  list_t *list = list_new();
  list_node_t *tj = list_node_new("tj");
  list_node_t *taylor = list_node_new("taylor");
  list_node_t *simon = list_node_new("simon");
  
  // tj taylor simon
  list_rpush(list, tj);
  list_rpush(list, taylor);
  list_rpush(list, simon);
  
  // Assertions
  
  // From head
  list_iterator_t *it = list_iterator_new(list, LIST_HEAD);
  list_node_t *a = list_iterator_next(it);
  list_node_t *b = list_iterator_next(it);
  list_node_t *c = list_iterator_next(it);
  list_node_t *d = list_iterator_next(it);
  
  assert(a == tj);
  assert(b == taylor);
  assert(c == simon);
  assert(d == NULL);

  // From tail
  it = list_iterator_new(list, LIST_TAIL);
  list_node_t *a2 = list_iterator_next(it);
  list_node_t *b2 = list_iterator_next(it);
  list_node_t *c2 = list_iterator_next(it);
  list_node_t *d2 = list_iterator_next(it);
  
  assert(a2 == simon);
  assert(b2 == taylor);
  assert(c2 == tj);
  assert(d2 == NULL);
  list_iterator_destroy(it);
}

int
main(int argc, const char **argv){
  printf("\nlist_t: %db\n", sizeof(list_t));
  printf("list_node_t: %db\n", sizeof(list_node_t));
  printf("list_iterator_t: %db\n\n", sizeof(list_iterator_t));
  test(list_node_new);
  test(list_rpush);
  test(list_lpush);
  test(list_find);
  test(list_at);
  test(list_remove);
  test(list_rpop);
  test(list_lpop);
  test(list_destroy);
  test(list_iterator_t);
  puts("... \x1b[32m100%\x1b[0m\n");
  return 0;
}
