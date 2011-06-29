
//
// object.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "object.h"

/*
 * Alloc a luna value and assign the given `obj`.
 */

luna_value_t *
luna_obj(luna_object_t *obj) {
  luna_value_t *self = malloc(sizeof(luna_value_t));
  self->type = LUNA_TYPE_OBJECT;
  self->value.as_obj = obj;
  return self;
}

/*
 * Set hash `key` to `val`.
 */

inline void
luna_object_set(khash_t(value) *self, char *key, luna_value_t *val) {
  int ret;
  khiter_t k = kh_put(value, self, key, &ret);
  kh_value(self, k) = val;
}

/*
 * Get hash `key`, or NULL.
 */

inline luna_value_t *
luna_object_get(khash_t(value) *self, char *key) {
  khiter_t k = kh_get(value, self, key);
  return k == kh_end(self) ? NULL : kh_value(self, k);
}

/*
 * Check if hash `key` exists.
 */

inline int
luna_object_has(khash_t(value) *self, char *key) {
  khiter_t k = kh_get(value, self, key);
  return kh_exist(self, k);
}

/*
 * Remove hash `key`.
 */

void
luna_object_remove(khash_t(value) *self, char *key) {
  khiter_t k = kh_get(value, self, key);
  kh_del(value, self, k);
}