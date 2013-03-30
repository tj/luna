
//
// state.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_STATE__
#define __LUNA_STATE__

#include "khash.h"

// TODO: move

typedef struct {
  int len;
  char *val;
} luna_string_t;

KHASH_MAP_INIT_STR(str, luna_string_t *);

/*
 * Luna state.
 */

typedef struct {
  khash_t(str) *strs;
} luna_state_t;

// protos

void
luna_state_init(luna_state_t *self);

// TODO: move

luna_string_t *
luna_string(luna_state_t *state, char *val);

#endif /* __LUNA_STATE__ */