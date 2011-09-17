
//
// object.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "object.h"
#include "hash.h"

/*
 * Allocate an initialize a new object of the given `type`.
 */

static luna_value_t *
alloc_value(luna_value type) {
  luna_value_t *self = malloc(sizeof(luna_value_t));
  if (!self) return NULL;
  self->type = type;
  self->hash = luna_hash_new();
  return self;
}

/*
 * Allocate a new int object with the given `val`.
 */

luna_value_t *
luna_int_new(int val) {
  luna_value_t *self = alloc_value(LUNA_TYPE_INT);
  if (!self) return NULL;
  self->value.as_int = val;
  return self;
}