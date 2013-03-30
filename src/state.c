
//
// state.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include "state.h"

/*
 * Initialize luna state:
 * 
 *   - initialize string vector
 */

void
luna_state_init(luna_state_t *self) {
  self->strs = kh_init(str);
}
