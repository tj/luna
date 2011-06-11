
//
// hash.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_HASH_T__
#define __LUNA_HASH_T__

#include "khash.h"

KHASH_MAP_INIT_STR(str, char *);

/*
 * Hash size.
 */

#define hash_size kh_size

/*
 * Number of buckets available.
 */

#define hash_buckets kh_n_buckets

/*
 * Iterate hash key / value pairs.
 */

#define hash_each(self, block) { \
    char *key, *val; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      key = kh_key(self, k); \
      val = kh_value(self, k); \
      block \
    } \
  }

/*
 * Iterate hash keys.
 */

#define hash_each_key(self, block) { \
    char *key; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      key = kh_key(self, k); \
      block \
    } \
  }

/*
 * Iterate hash values.
 */

#define hash_each_val(self, block) { \
    char *val; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      val = kh_value(self, k); \
      block \
    } \
  }

// protos

void
hash_set(khash_t(str) *self, char *key, char *val);

char *
hash_get(khash_t(str) *self, char *key);

int
hash_has(khash_t(str) *self, char *key);

void
hash_remove(khash_t(str) *self, char *key);

#endif /* __LUNA_HASH_T__ */