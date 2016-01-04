
//
// vec.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_VEC_H
#define LUNA_VEC_H

#include "object.h"
#include "kvec.h"

/*
 * Luna array.
 */

typedef kvec_t(luna_object_t *) luna_vec_t;

/*
 * Initialize an array.
 */

#define luna_vec_init(self) kv_init(*self)

/*
 * Return the array length.
 */

#define luna_vec_length(self) kv_size(*self)

/*
 * Push `obj` into the array.
 */

#define luna_vec_push(self, obj) \
  kv_push(luna_object_t *, *self, obj)

/*
 * Pop an object out of the array.
 */

#define luna_vec_pop(self) \
  (luna_vec_length(self) \
    ? kv_pop(*self) \
    : NULL)

/*
 * Return the object at `i`.
 */

#define luna_vec_at(self, i) \
  (((i) >= 0 && (i) < luna_vec_length(self)) \
    ? kv_A(*self, (i)) \
    : NULL)

/*
 * Iterate the array, populating `i` and `val`.
 */

#define luna_vec_each(self, block) { \
    luna_object_t *val; \
    int len = luna_vec_length(self); \
    for (int i = 0; i < len; ++i) { \
      val = luna_vec_at(self, i); \
      block; \
    } \
  }

// protos

luna_vec_t *
luna_vec_new();

#endif /* LUNA_VEC_H */