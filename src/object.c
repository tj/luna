
//
// object.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "object.h"
#include "hash.h"
#include "list.h"
#include "internal.h"

/*
 * Allocate an initialize a new object of the given `type`.
 */

static luna_object_t *
alloc_object(luna_object type) {
  luna_object_t *self = malloc(sizeof(luna_object_t));
  if (unlikely(!self)) return NULL;
  self->type = type;
  self->hash = luna_hash_new();
  return self;
}

/*
 * Allocate a new int object with the given `val`.
 */

luna_object_t *
luna_int_new(int val) {
  luna_object_t *self = alloc_object(LUNA_TYPE_INT);
  if (unlikely(!self)) return NULL;
  self->value.as_int = val;
  return self;
}