
//
// object.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_OBJECT_H__
#define __LUNA_OBJECT_H__

#include "khash.h"

// TODO: consider pointer for immediate values

/*
 * Check if `val` is the given type.
 */

#define luna_value_is(val, t) ((val)->type == LUNA_TYPE_##t)

/*
 * Specific type macros.
 */

#define luna_is_node(val) luna_value_is(val, NODE)
#define luna_is_list(val) luna_value_is(val, LIST)
#define luna_is_array(val) luna_value_is(val, ARRAY)
#define luna_is_object(val) luna_value_is(val, OBJECT)
#define luna_is_string(val) luna_value_is(val, STRING)
#define luna_is_float(val) luna_value_is(val, FLOAT)
#define luna_is_int(val) luna_value_is(val, INT)
#define luna_is_bool(val) luna_value_is(val, BOOL)
#define luna_is_null(val) luna_value_is(val, NULL)

/*
 * Luna value types.
 */

typedef enum {
    LUNA_TYPE_NULL
  , LUNA_TYPE_NODE
  , LUNA_TYPE_BOOL
  , LUNA_TYPE_INT
  , LUNA_TYPE_FLOAT
  , LUNA_TYPE_STRING
  , LUNA_TYPE_OBJECT
  , LUNA_TYPE_ARRAY
  , LUNA_TYPE_LIST
} luna_value;

/*
 * Luna value.
 */

typedef struct {
  luna_value type;
  union {
    void *obj;
    int as_int;
    float as_float;
  } val;
} luna_value_t;

KHASH_MAP_INIT_STR(value, luna_value_t *);

/*
 * Luna object.
 */

typedef khash_t(value) luna_object_t;

/*
 * Allocate a new object.
 */

#define luna_object_new() kh_init(value)

/*
 * Destroy the object.
 */

#define luna_object_destroy(self) kh_destroy(value, self)

/*
 * Object size.
 */

#define luna_object_size kh_size

/*
 * Iterate object slots and values, populating
 * `slot` and `val`.
 */

#define luna_object_each(self, block) { \
   char *slot; \
   luna_value_t *val; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      slot = kh_key(self, k); \
      val = kh_value(self, k); \
      block; \
    } \
  }

/*
 * Iterate object slots, populating `slot`.
 */

#define luna_object_each_slot(self, block) { \
    char *slot; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      slot = kh_key(self, k); \
      block; \
    } \
  }

/*
 * Iterate object values, populating `val`.
 */

#define luna_object_each_val(self, block) { \
    luna_value_t *val; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      val = kh_value(self, k); \
      block; \
    } \
  }

// protos

void
luna_object_set(khash_t(value) *self, char *key, luna_value_t *val);

luna_value_t *
luna_object_get(khash_t(value) *self, char *key);

int
luna_object_has(khash_t(value) *self, char *key);

void
luna_object_remove(khash_t(value) *self, char *key);

#endif /* __LUNA_OBJECT_H__ */