
#include <stdio.h>
#include <assert.h>
#include "object.h"
#include "array.h"

/*
 * Test luna_array_length().
 */

static void
test_array_length() {
  luna_array_t arr;
  luna_array_init(&arr);

  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.val.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.val.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.val.as_int = 3;

  assert(0 == luna_array_length(&arr));

  luna_array_push(&arr, &one);
  assert(1 == luna_array_length(&arr));

  luna_array_push(&arr, &two);
  assert(2 == luna_array_length(&arr));

  luna_array_push(&arr, &three);
  assert(3 == luna_array_length(&arr));
}

/*
 * Test luna_array_push().
 */

static void
test_array_push() {
  luna_array_t arr;
  luna_array_init(&arr);

  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.val.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.val.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.val.as_int = 3;

  assert(0 == luna_array_length(&arr));

  luna_array_push(&arr, &one);
  assert(1 == luna_array_pop(&arr)->val.as_int);

  luna_array_push(&arr, &one);
  luna_array_push(&arr, &one);
  assert(1 == luna_array_pop(&arr)->val.as_int);
  assert(1 == luna_array_pop(&arr)->val.as_int);

  luna_array_push(&arr, &one);
  luna_array_push(&arr, &two);
  luna_array_push(&arr, &three);
  assert(3 == luna_array_pop(&arr)->val.as_int);
  assert(2 == luna_array_pop(&arr)->val.as_int);
  assert(1 == luna_array_pop(&arr)->val.as_int);

  assert(NULL == luna_array_pop(&arr));
  assert(NULL == luna_array_pop(&arr));
  assert(NULL == luna_array_pop(&arr));
  luna_array_push(&arr, &one);
  assert(1 == luna_array_pop(&arr)->val.as_int);
}

/*
 * Test luna_array_at().
 */

static void
test_array_at() {
  luna_array_t arr;
  luna_array_init(&arr);

  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.val.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.val.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.val.as_int = 3;

  luna_array_push(&arr, &one);
  luna_array_push(&arr, &two);
  luna_array_push(&arr, &three);

  assert(1 == luna_array_at(&arr, 0)->val.as_int);
  assert(2 == luna_array_at(&arr, 1)->val.as_int);
  assert(3 == luna_array_at(&arr, 2)->val.as_int);
  
  assert(NULL == luna_array_at(&arr, -1123));
  assert(NULL == luna_array_at(&arr, 5));
  assert(NULL == luna_array_at(&arr, 1231231));
}

/*
 * Test luna_object_set().
 */

static void
test_object_set() {
  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.val.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.val.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.val.as_int = 3;

  luna_object_t *obj = luna_object_new();

  assert(0 == luna_object_size(obj));

  luna_object_set(obj, "one", &one);
  assert(1 == luna_object_size(obj));

  luna_object_set(obj, "two", &two);
  assert(2 == luna_object_size(obj));

  luna_object_set(obj, "three", &three);
  assert(3 == luna_object_size(obj));

  assert(&one == luna_object_get(obj, "one"));
  assert(&two == luna_object_get(obj, "two"));
  assert(&three == luna_object_get(obj, "three"));
  assert(NULL == luna_object_get(obj, "four"));

  luna_object_destroy(obj);
}

/*
 * Test the given `fn`.
 */

#define test(fn) \
  printf("    - \033[90m%s\033[0m\n", #fn); \
  test_##fn();

/*
 * Test suite title.
 */

#define suite(title) printf("  \033[90m%s\033[0m\n", title)

/*
 * Run all test suites.
 */

int
main(int argc, const char **argv){
  printf("\n");

  suite("array");
  test(array_length);
  test(array_push);
  test(array_at);

  suite("object");
  test(object_set);

  printf("\n");
  return 0;
}