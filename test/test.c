
#include <stdio.h>
#include <assert.h>
#include "array.h"

static void
test_array_push() {
  luna_array_t arr;
  luna_array_init(&arr);

  luna_object_t one = { .type = LUNA_TYPE_INT };
  one.val.as_int = 1;

  luna_object_t two = { .type = LUNA_TYPE_INT };
  two.val.as_int = 2;

  luna_object_t three = { .type = LUNA_TYPE_INT };
  three.val.as_int = 3;

  assert(0 == luna_array_length(&arr) && "asdf");

  luna_array_push(&arr, &one);
  assert(1 == luna_array_length(&arr) && "asdf");

  luna_array_push(&arr, &two);
  assert(2 == luna_array_length(&arr) && "asdf");

  luna_array_push(&arr, &three);
  assert(3 == luna_array_length(&arr) && "asdf");
}

#define test(fn) \
  printf("    - \033[90m%s\033[0m\n", #fn); \
  test_##fn();

static void
suite(const char *title) {
  printf("  \033[90m%s\033[0m\n", title);
}

int
main(int argc, const char **argv){
  printf("\n");
  suite("array");
  test(array_push);
  printf("\n");
  return 0;
}