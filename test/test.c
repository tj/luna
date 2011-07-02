
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "khash.h"
#include "state.h"
#include "object.h"
#include "array.h"

/*
 * Test luna_is_* macros.
 */

static void
test_value_is() {
  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;
  assert(luna_is_int(&one));
  assert(!luna_is_string(&one));

  luna_value_t two = { .type = LUNA_TYPE_NULL };
  assert(luna_is_null(&two));
}

/*
 * Test luna_array_length().
 */

static void
test_array_length() {
  luna_array_t arr;
  luna_array_init(&arr);

  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.value.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.value.as_int = 3;

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
  one.value.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.value.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.value.as_int = 3;

  assert(0 == luna_array_length(&arr));

  luna_array_push(&arr, &one);
  assert(1 == luna_array_pop(&arr)->value.as_int);

  luna_array_push(&arr, &one);
  luna_array_push(&arr, &one);
  assert(1 == luna_array_pop(&arr)->value.as_int);
  assert(1 == luna_array_pop(&arr)->value.as_int);

  luna_array_push(&arr, &one);
  luna_array_push(&arr, &two);
  luna_array_push(&arr, &three);
  assert(3 == luna_array_pop(&arr)->value.as_int);
  assert(2 == luna_array_pop(&arr)->value.as_int);
  assert(1 == luna_array_pop(&arr)->value.as_int);

  assert(NULL == luna_array_pop(&arr));
  assert(NULL == luna_array_pop(&arr));
  assert(NULL == luna_array_pop(&arr));
  luna_array_push(&arr, &one);
  assert(1 == luna_array_pop(&arr)->value.as_int);
}

/*
 * Test luna_array_at().
 */

static void
test_array_at() {
  luna_array_t arr;
  luna_array_init(&arr);

  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.value.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.value.as_int = 3;

  luna_array_push(&arr, &one);
  luna_array_push(&arr, &two);
  luna_array_push(&arr, &three);

  assert(1 == luna_array_at(&arr, 0)->value.as_int);
  assert(2 == luna_array_at(&arr, 1)->value.as_int);
  assert(3 == luna_array_at(&arr, 2)->value.as_int);
  
  assert(NULL == luna_array_at(&arr, -1123));
  assert(NULL == luna_array_at(&arr, 5));
  assert(NULL == luna_array_at(&arr, 1231231));
}

/*
 * Test array iteration.
 */

static void
test_array_iteration() {
  luna_array_t arr;
  luna_array_init(&arr);

  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.value.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.value.as_int = 3;

  luna_array_push(&arr, &one);
  luna_array_push(&arr, &two);
  luna_array_push(&arr, &three);

  luna_array_each(&arr, {
    
  });
}

/*
 * Test luna_object_set().
 */

static void
test_object_set() {
  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.value.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.value.as_int = 3;

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
 * Test luna_object_has().
 */

static void
test_object_has() {
  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;

  luna_object_t *obj = luna_object_new();

  luna_object_set(obj, "one", &one);

  assert(1 == luna_object_has(obj, "one"));
  assert(0 == luna_object_has(obj, "foo"));

  luna_object_destroy(obj);
}

/*
 * Test luna_object_remove().
 */

static void
test_object_remove() {
  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;

  luna_object_t *obj = luna_object_new();

  luna_object_set(obj, "one", &one);
  assert(&one == luna_object_get(obj, "one"));

  luna_object_remove(obj, "one");
  assert(NULL == luna_object_get(obj, "one"));

  luna_object_set(obj, "one", &one);
  assert(&one == luna_object_get(obj, "one"));

  luna_object_remove(obj, "one");
  assert(NULL == luna_object_get(obj, "one"));

  luna_object_destroy(obj);
}

/*
 * Check if the given `slot` is valid.
 */

static int
valid_slot(char *slot) {
  return 0 == strcmp("one", slot)
    || 0 == strcmp("two", slot)
    || 0 == strcmp("three", slot)
    || 0 == strcmp("four", slot)
    || 0 == strcmp("five", slot);
}

/*
 * Test object iteration macros.
 */

static void
test_object_iteration() {
  luna_value_t one = { .type = LUNA_TYPE_INT };
  one.value.as_int = 1;

  luna_value_t two = { .type = LUNA_TYPE_INT };
  two.value.as_int = 2;

  luna_value_t three = { .type = LUNA_TYPE_INT };
  three.value.as_int = 3;

  luna_object_t *obj = luna_object_new();

  assert(0 == luna_object_size(obj));

  luna_object_set(obj, "one", &one);
  luna_object_set(obj, "two", &two);
  luna_object_set(obj, "three", &three);
  luna_object_set(obj, "four", &three);
  luna_object_set(obj, "five", &three);

  char *slots[luna_object_size(obj)];
  int i = 0;
  luna_object_each_slot(obj, { slots[i++] = slot; });
  for (int i = 0; i < 5; ++i) assert(valid_slot(slots[i]));

  char *slots2[luna_object_size(obj)];
  i = 0;
  luna_object_each_slot(obj, slots2[i++] = slot);
  for (int i = 0; i < 5; ++i) assert(valid_slot(slots2[i]));

  char *slots3[luna_object_size(obj)];
  i = 0;
  luna_object_each(obj, slots3[i++] = slot);
  for (int i = 0; i < 5; ++i) assert(valid_slot(slots3[i]));

  luna_object_destroy(obj);
}

/*
 * Test strings.
 */

static void
test_string() {
  luna_state_t state;
  luna_state_init(&state);

  luna_string_t *str = luna_string(&state, "foo bar baz");
  assert(0 == strcmp("foo bar baz", str->val));
  
  str = luna_string(&state, "foo bar baz");
  assert(0 == strcmp("foo bar baz", str->val));

  for (int i = 0; i < 200; ++i) str = luna_string(&state, "foo");
  assert(0 == strcmp("foo", str->val));

  assert(2 == kh_size(state.strs));
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

#define suite(title) \
  printf("\n  \033[90m%s\033[0m\n", title)

/*
 * Run all test suites.
 */

int
main(int argc, const char **argv){
  clock_t start = clock();

  suite("value");
  test(value_is);

  suite("array");
  test(array_length);
  test(array_push);
  test(array_at);
  test(array_iteration);

  suite("object");
  test(object_set);
  test(object_has);
  test(object_remove);
  test(object_iteration);

  suite("string");
  test(string);

  printf("\n");
  printf("  \033[90mcompleted in %.5fs\033[0m\n", (float) (clock() - start) / CLOCKS_PER_SEC);
  printf("\n");
  return 0;
}