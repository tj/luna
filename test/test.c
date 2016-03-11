
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include "utils.h"
#include "errors.h"
#include "lexer.h"
#include "prettyprint.h"
#include "parser.h"
#include "khash.h"
#include "state.h"
#include "object.h"
#include "hash.h"
#include "vec.h"


// print func for prettyprint

char *print_buf;

int
bprintf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int r = vsprintf(print_buf + strlen(print_buf), format, ap);
  va_end(ap);
  return r;
}

/*
 * Test luna_is_* macros.
 */

static void
test_value_is() {
  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  assert(luna_is_int(&one));
  assert(!luna_is_string(&one));

  luna_object_t two = { .type = LUNA_TYPE_NULL };
  assert(luna_is_null(&two));
}

/*
 * Test luna_vec_length().
 */

static void
test_array_length() {
  luna_vec_t arr;
  luna_vec_init(&arr);

  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  luna_object_t two = { .type = LUNA_TYPE_INT, .value.as_int = 2 };
  luna_object_t three = { .type = LUNA_TYPE_INT, .value.as_int = 3 };

  assert(0 == luna_vec_length(&arr));

  luna_vec_push(&arr, &one);
  assert(1 == luna_vec_length(&arr));

  luna_vec_push(&arr, &two);
  assert(2 == luna_vec_length(&arr));

  luna_vec_push(&arr, &three);
  assert(3 == luna_vec_length(&arr));
}

/*
 * Test luna_vec_push().
 */

static void
test_array_push() {
  luna_vec_t arr;
  luna_vec_init(&arr);

  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  luna_object_t two = { .type = LUNA_TYPE_INT, .value.as_int = 2 };
  luna_object_t three = { .type = LUNA_TYPE_INT, .value.as_int = 3 };

  assert(0 == luna_vec_length(&arr));

  luna_vec_push(&arr, &one);
  assert(1 == luna_vec_pop(&arr)->value.as_int);

  luna_vec_push(&arr, &one);
  luna_vec_push(&arr, &one);
  assert(1 == luna_vec_pop(&arr)->value.as_int);
  assert(1 == luna_vec_pop(&arr)->value.as_int);

  luna_vec_push(&arr, &one);
  luna_vec_push(&arr, &two);
  luna_vec_push(&arr, &three);
  assert(3 == luna_vec_pop(&arr)->value.as_int);
  assert(2 == luna_vec_pop(&arr)->value.as_int);
  assert(1 == luna_vec_pop(&arr)->value.as_int);

  assert(NULL == luna_vec_pop(&arr));
  assert(NULL == luna_vec_pop(&arr));
  assert(NULL == luna_vec_pop(&arr));
  luna_vec_push(&arr, &one);
  assert(1 == luna_vec_pop(&arr)->value.as_int);
}

/*
 * Test luna_vec_at().
 */

static void
test_array_at() {
  luna_vec_t arr;
  luna_vec_init(&arr);

  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  luna_object_t two = { .type = LUNA_TYPE_INT, .value.as_int = 2 };
  luna_object_t three = { .type = LUNA_TYPE_INT, .value.as_int = 3 };

  luna_vec_push(&arr, &one);
  luna_vec_push(&arr, &two);
  luna_vec_push(&arr, &three);

  assert(1 == luna_vec_at(&arr, 0)->value.as_int);
  assert(2 == luna_vec_at(&arr, 1)->value.as_int);
  assert(3 == luna_vec_at(&arr, 2)->value.as_int);

  assert(NULL == luna_vec_at(&arr, -1123));
  assert(NULL == luna_vec_at(&arr, 5));
  assert(NULL == luna_vec_at(&arr, 1231231));
}

/*
 * Test array iteration.
 */

static void
test_array_iteration() {
  luna_vec_t arr;
  luna_vec_init(&arr);

  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  luna_object_t two = { .type = LUNA_TYPE_INT, .value.as_int = 2 };
  luna_object_t three = { .type = LUNA_TYPE_INT, .value.as_int = 3 };

  luna_vec_push(&arr, &one);
  luna_vec_push(&arr, &two);
  luna_vec_push(&arr, &three);

  int vals[3];
  int k = 0;

  luna_vec_each(&arr, { vals[k++] = val->value.as_int; });
  assert(1 == vals[0]);
  assert(2 == vals[1]);
  assert(3 == vals[2]);
}

/*
 * Test luna_hash_set().
 */

static void
test_hash_set() {
  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  luna_object_t two = { .type = LUNA_TYPE_INT, .value.as_int = 2 };
  luna_object_t three = { .type = LUNA_TYPE_INT, .value.as_int = 3 };

  luna_hash_t *obj = luna_hash_new();

  assert(0 == luna_hash_size(obj));

  luna_hash_set(obj, "one", &one);
  assert(1 == luna_hash_size(obj));

  luna_hash_set(obj, "two", &two);
  assert(2 == luna_hash_size(obj));

  luna_hash_set(obj, "three", &three);
  assert(3 == luna_hash_size(obj));

  assert(&one == luna_hash_get(obj, "one"));
  assert(&two == luna_hash_get(obj, "two"));
  assert(&three == luna_hash_get(obj, "three"));
  assert(NULL == luna_hash_get(obj, "four"));

  luna_hash_destroy(obj);
}

/*
 * Test luna_hash_has().
 */

static void
test_hash_has() {
  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };

  luna_hash_t *obj = luna_hash_new();

  luna_hash_set(obj, "one", &one);

  assert(1 == luna_hash_has(obj, "one"));
  assert(0 == luna_hash_has(obj, "foo"));

  luna_hash_destroy(obj);
}

/*
 * Test luna_hash_remove().
 */

static void
test_hash_remove() {
  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };

  luna_hash_t *obj = luna_hash_new();

  luna_hash_set(obj, "one", &one);
  assert(&one == luna_hash_get(obj, "one"));

  luna_hash_remove(obj, "one");
  assert(NULL == luna_hash_get(obj, "one"));

  luna_hash_set(obj, "one", &one);
  assert(&one == luna_hash_get(obj, "one"));

  luna_hash_remove(obj, "one");
  assert(NULL == luna_hash_get(obj, "one"));

  luna_hash_destroy(obj);
}

/*
 * Check if the given `slot` is valid.
 */

static int
valid_slot(const char *slot) {
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
test_hash_iteration() {
  luna_object_t one = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  luna_object_t two = { .type = LUNA_TYPE_INT, .value.as_int = 2 };
  luna_object_t three = { .type = LUNA_TYPE_INT, .value.as_int = 3 };

  luna_hash_t *obj = luna_hash_new();

  assert(0 == luna_hash_size(obj));

  luna_hash_set(obj, "one", &one);
  luna_hash_set(obj, "two", &two);
  luna_hash_set(obj, "three", &three);
  luna_hash_set(obj, "four", &three);
  luna_hash_set(obj, "five", &three);

  const char *slots[luna_hash_size(obj)];
  int i = 0;
  luna_hash_each_slot(obj, { slots[i++] = slot; });
  for (int i = 0; i < 5; ++i) assert(valid_slot(slots[i]));

  const char *slots2[luna_hash_size(obj)];
  i = 0;
  luna_hash_each_slot(obj, slots2[i++] = slot);
  for (int i = 0; i < 5; ++i) assert(valid_slot(slots2[i]));

  const char *slots3[luna_hash_size(obj)];
  i = 0;
  luna_hash_each(obj, slots3[i++] = slot);
  for (int i = 0; i < 5; ++i) assert(valid_slot(slots3[i]));

  luna_hash_destroy(obj);
}

/*
 * Test mixins.
 */

static void
test_hash_mixins() {
  luna_object_t type = { .type = LUNA_TYPE_INT, .value.as_int = 1 };
  luna_vec_t arr;
  luna_vec_init(&arr);
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
 * Test parser.
 */

static void
_test_parser(const char *source_path, const char *out_path) {
  luna_lexer_t lexer;
  luna_parser_t parser;
  luna_block_node_t *root;

  char *source = file_read(source_path);
  assert(source != NULL);
  char *expected = file_read(out_path);
  assert(expected != NULL);

  luna_lexer_init(&lexer, source, source_path);
  luna_parser_init(&parser, &lexer);

  if (!(root = luna_parse(&parser))) {
    luna_report_error(&parser);
    exit(1);
  }

  char buf[1024] = {0};
  print_buf = buf;
  luna_set_prettyprint_func(bprintf);
  luna_prettyprint((luna_node_t *) root);

  assert(strcmp(expected, print_buf) == 0);
}

static void
test_assign() {
  _test_parser("test/parser/assign.luna", "test/parser/assign.out");
}

static void
test_assign_chain() {
  _test_parser("test/parser/assign.chain.luna", "test/parser/assign.chain.out");
}

static void
test_subscript() {
  _test_parser("test/parser/subscript.luna", "test/parser/subscript.out");
}

static void
test_declaration() {
  _test_parser("test/parser/declaration.luna", "test/parser/declaration.out");
}

static void
test_return() {
  _test_parser("test/parser/return.luna", "test/parser/return.out");
}

static void
test_use() {
  _test_parser("test/parser/use.luna", "test/parser/use.out");
}

/*
 * Test the given `fn`.
 */

#define test(fn) \
  printf("    \e[92m✓ \e[90m%s\e[0m\n", #fn); \
  test_##fn();

/*
 * Test suite title.
 */

#define suite(title) \
  printf("\n  \e[36m%s\e[0m\n", title)

/*
 * Report sizeof.
 */

#define size(type) \
  printf("\n  \e[90m%s: %ld bytes\e[0m\n", #type, sizeof(type));

/*
 * Run all test suites.
 */

int
main(int argc, const char **argv){
  clock_t start = clock();

  size(luna_object_t);

  suite("value");
  test(value_is);

  suite("array");
  test(array_length);
  test(array_push);
  test(array_at);
  test(array_iteration);

  suite("hash");
  test(hash_set);
  test(hash_has);
  test(hash_remove);
  test(hash_iteration);
  test(hash_mixins);

  suite("string");
  test(string);

  suite("parser");
  test(assign);
  test(assign_chain);
  test(subscript);
  test(declaration);
  test(return);
  test(use);

  printf("\n");
  printf("  \e[90mcompleted in \e[32m%.5fs\e[0m\n", (float) (clock() - start) / CLOCKS_PER_SEC);
  printf("\n");
  return 0;
}
