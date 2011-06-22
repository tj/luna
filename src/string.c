
//
// string.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include <string.h>
#include "state.h"

/*
 * Return a new luna_string_t for the given `val`,
 * allocating space in the strings hash unless present,
 * or NULL on failure.
 */

luna_string_t *
luna_string_new(luna_state_t *state, char *val) {
  khiter_t k = kh_get(str, state->strs, val);

  // exists
  int exists = kh_size(state->strs) && kh_exist(state->strs, k);
  if (exists) return kh_value(state->strs, k);

  // alloc
  int ret;
  luna_string_t *self = calloc(1, sizeof(luna_string_t));
  self->len = strlen(val);
  self->val = malloc(self->len + 1);
  if (!self->val) return NULL;
  memcpy(self->val, val, self->len);
  self->val[self->len] = 0;
  k = kh_put(str, state->strs, val, &ret);

  return kh_value(state->strs, k) = self;
}