
//
// array.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "array.h"

/*
 * Alloc and initialize a new array.
 */

luna_array_t *
luna_array_new() {
  luna_array_t *self = malloc(sizeof(luna_array_t));
  luna_array_init(self);
  return self;
}