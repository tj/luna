
//
// object.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_OBJECT__
#define __LUNA_OBJECT__

#include "hash.h"

// TODO: consider pointer for immediate values
// TODO: inherit

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
 * Luna object.
 * 
 * A simple tagged union forming
 * the basis of a Luna values.
 */

struct luna_object_struct {
  luna_value type;
  luna_hash_t *hash;
  union {
    void *as_pointer;
    int as_int;
    float as_float;
  } value;
};

// protos

/*
 * Object size.
 */

#define luna_object_size(self) luna_hash_size((self)->hash)

/*
 * Iterate object slots and values, populating
 * `slot` and `val`.
 */

#define luna_object_each(self, block) luna_hash_each((self)->hash, block)

/*
 * Iterate object slots, populating `slot`.
 */

#define luna_object_each_slot(self, block) luna_hash_each_slot((self)->hash, block)

/*
 * Iterate object values, populating `val`.
 */

#define luna_object_each_val(self, block) luna_hash_each_val((self)->hash, block)

/*
 * Set object slot `key` to `val`.
 */

#define luna_object_set(self, key, val) luna_hash_set((self)->hash, key, val)

/*
 * Get object slot `key`.
 */

#define luna_object_get(self, key) luna_hash_get((self)->hash, key)

/*
 * Check if object has slot `key`.
 */

#define luna_object_has(self, key) luna_hash_has((self)->hash, key)

/*
 * Remove object slot `key`.
 */

#define luna_object_remove(self, key) luna_hash_remove((self)->hash, key)

#endif /* __LUNA_OBJECT__ */