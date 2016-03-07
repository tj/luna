
//
// object.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include <assert.h>
#include <stdio.h>
#include "object.h"
#include "internal.h"

/*
 * Print `self` to stdout.
 */

void
luna_object_inspect(luna_object_t *self) {
  switch (self->type) {
    case LUNA_TYPE_FLOAT:
      printf("%2f\n", self->value.as_float);
      break;
    case LUNA_TYPE_INT:
      printf("%d\n", self->value.as_int);
      break;
	case LUNA_TYPE_BOOL:
	  printf("%s\n", self->value.as_int ? "true" : "false");
	  break;
    default:
      assert(0 && "unhandled");
  }
}

/*
 * Allocate an initialize a new object of the given `type`.
 */

static luna_object_t *
alloc_object(luna_object type) {
  luna_object_t *self = malloc(sizeof(luna_object_t));
  if (unlikely(!self)) return NULL;
  self->type = type;
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

/*
 * Allocate a new float object with the given `val`.
 */

luna_object_t *
luna_float_new(float val) {
  luna_object_t *self = alloc_object(LUNA_TYPE_FLOAT);
  if (unlikely(!self)) return NULL;
  self->value.as_float = val;
  return self;
}

/*
 * Allocate a new bool object with the given `val`.
 */

luna_object_t *
luna_bool_new(bool val) {
  luna_object_t *self = alloc_object(LUNA_TYPE_BOOL);
  if (unlikely(!self)) return NULL;
  self->value.as_int = val;
  return self;
}

/*
 * Allocate a new string object with the given `val`.
 */

luna_object_t *
luna_string_new(const char *val) {
  luna_object_t *self = alloc_object(LUNA_TYPE_STRING);
  if (unlikely(!self)) return NULL;
  self->value.as_pointer = strdup(val);
  return self;
}

void
luna_object_free(luna_object_t *self) {
  switch(self->type) {
	case LUNA_TYPE_STRING:
      free(self->value.as_pointer);
	  break;
    default: break;
  }
  free(self);
}
