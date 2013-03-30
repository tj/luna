
//
// object.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_OBJECT__
#define __LUNA_OBJECT__

#include "hash.h"

/*
 * Check if `val` is the given type.
 */

#define luna_object_is(val, t) ((val)->type == LUNA_TYPE_##t)

/*
 * Specific type macros.
 */

#define luna_is_node(val) luna_object_is(val, NODE)
#define luna_is_array(val) luna_object_is(val, ARRAY)
#define luna_is_object(val) luna_object_is(val, OBJECT)
#define luna_is_string(val) luna_object_is(val, STRING)
#define luna_is_float(val) luna_object_is(val, FLOAT)
#define luna_is_int(val) luna_object_is(val, INT)
#define luna_is_bool(val) luna_object_is(val, BOOL)
#define luna_is_null(val) luna_object_is(val, NULL)

/*
 * Luna value types.
 */

typedef enum {
  LUNA_TYPE_NULL,
  LUNA_TYPE_NODE,
  LUNA_TYPE_BOOL,
  LUNA_TYPE_INT,
  LUNA_TYPE_FLOAT,
  LUNA_TYPE_STRING,
  LUNA_TYPE_OBJECT,
  LUNA_TYPE_ARRAY,
  LUNA_TYPE_LIST
} luna_object;

/*
 * Luna object.
 *
 * A simple tagged union forming
 * the basis of a Luna values.
 */

struct luna_object_struct {
  luna_object type;
  union {
    void *as_pointer;
    int as_int;
    float as_float;
  } value;
};

// protos

void
luna_object_inspect(luna_object_t *self);

luna_object_t *
luna_int_new(int val);

luna_object_t *
luna_float_new(float val);

#endif /* __LUNA_OBJECT__ */
