
//
// object.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "object.h"
#include "hash.h"

/*
 * Allocate a new int object with the given `val`.
 */

luna_value_t *
luna_int_new(int val) {
  luna_value_t *self = malloc(sizeof(luna_value_t));
  if (!self) return NULL;
  self->type = LUNA_TYPE_INT;
  self->hash = luna_hash_new();
  self->value.as_int = val;
  return self;
}