
//
// vec.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include "vec.h"
#include "internal.h"

/*
 * Alloc and initialize a new array.
 */

luna_vec_t *
luna_vec_new() {
  luna_vec_t *self = malloc(sizeof(luna_vec_t));
  if (unlikely(!self)) return NULL;
  luna_vec_init(self);
  return self;
}