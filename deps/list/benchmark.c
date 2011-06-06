
#include <stdio.h>
#include <time.h>
#include "src/list.h"

static void
bm(char *label, void (*fn)()) {
  printf(" %18s", label);
  fflush(stdout);
  fn();
}

static int nnodes = 10000000;
static clock_t startTime;

static void
start() {
  startTime = clock();
}

static void
stop() {
  float duration = (float) (clock() - startTime) / CLOCKS_PER_SEC;
  printf(": \x1b[32m%.4f\x1b[0ms\n", duration);
}

static void
bm_rpush() {
  start();
  int n = nnodes;
  list_t *list = list_new();
  while (n--) {
    list_rpush(list, list_node_new("foo"));
  }
  stop();
}

static void
bm_lpush() {
  start();
  int n = nnodes;
  list_t *list = list_new();
  while (n--) {
    list_lpush(list, list_node_new("foo"));
  }
  stop();
}

static void
bm_find() {
  int n = nnodes;
  list_t *list = list_new();
  while (n--) {
    list_lpush(list, list_node_new("foo"));
  }
  list_rpush(list, list_node_new("bar"));
  start();
  list_find(list, "bar");
  stop();
}

static void
bm_iterate() {
  int n = nnodes;
  list_t *list = list_new();
  while (n--) {
    list_lpush(list, list_node_new("foo"));
  }
  list_iterator_t *it = list_iterator_new(list, LIST_HEAD);
  list_node_t *node;
  start();
  while ((node = list_iterator_next(it)))
    ;
  stop();
}

static void
bm_rpop() {
  int n = nnodes;
  list_t *list = list_new();
  while (n--) {
    list_lpush(list, list_node_new("foo"));
  }
  list_node_t *node;
  start();
  while ((node = list_rpop(list)))
    ;
  stop();
}

static void
bm_lpop() {
  int n = nnodes;
  list_t *list = list_new();
  while (n--) {
    list_lpush(list, list_node_new("foo"));
  }
  list_node_t *node;
  start();
  while ((node = list_lpop(list)))
    ;
  stop();
}

static list_t *list;

static void
bm_at() {
  start();
  list_at(list, 100000);
  stop();
}

static void
bm_at2() {
  start();
  list_at(list, 1000000);
  stop();
}

static void
bm_at3() {
  start();
  list_at(list, -100000);
  stop();
}

int
main(int argc, const char **argv){
  int n = nnodes;
  list = list_new();
  while (n--) list_lpush(list, list_node_new("foo"));
  puts("\n 10,000,000 nodes\n");
  bm("lpush", bm_lpush);  
  bm("rpush", bm_rpush);
  bm("lpop", bm_lpop);  
  bm("rpop", bm_rpop);  
  bm("find (last node)", bm_find);  
  bm("iterate", bm_iterate);  
  bm("at(100,000)", bm_at);  
  bm("at(1,000,000)", bm_at2);  
  bm("at(-100,000)", bm_at3);  
  puts("");
  return 0;
}