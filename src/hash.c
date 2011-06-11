
//
// hash.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "hash.h"

/*
 * Set hash `key` to `val`.
 */

inline void
hash_set(khash_t(str) *self, char *key, char *val) {
  int ret;
  khiter_t k = kh_put(str, self, key, &ret);
  kh_value(self, k) = val;
}

/*
 * Get hash `key`, or NULL.
 */

inline char *
hash_get(khash_t(str) *self, char *key) {
  khiter_t k = kh_get(str, self, key);
  return k == kh_end(self) ? NULL : kh_value(self, k);
}

/*
 * Check if hash `key` exists.
 */

inline int
hash_has(khash_t(str) *self, char *key) {
  khiter_t k = kh_get(str, self, key);
  return kh_exist(self, k);
}

/*
 * Remove hash `key`.
 */

void
hash_remove(khash_t(str) *self, char *key) {
  khiter_t k = kh_get(str, self, key);
  kh_del(str, self, k);
}