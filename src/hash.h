
//
// hash.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_HASH_H
#define LUNA_HASH_H

#include "khash.h"

// luna object

typedef struct luna_object_struct luna_object_t;

// value hash

KHASH_MAP_INIT_STR(value, luna_object_t *);

/*
 * Luna hash.
 */

typedef khash_t(value) luna_hash_t;

/*
 * Allocate a new hash.
 */

#define luna_hash_new() kh_init(value)

/*
 * Destroy the hash.
 */

#define luna_hash_destroy(self) kh_destroy(value, self)

/*
 * Hash size.
 */

#define luna_hash_size kh_size

/*
 * Iterate hash slots and values, populating
 * `slot` and `val`.
 */

#define luna_hash_each(self, block) { \
   const char *slot; \
   luna_object_t *val; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      slot = kh_key(self, k); \
      val = kh_value(self, k); \
      block; \
    } \
  }

/*
 * Iterate hash slots, populating `slot`.
 */

#define luna_hash_each_slot(self, block) { \
    const char *slot; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      slot = kh_key(self, k); \
      block; \
    } \
  }

/*
 * Iterate hash values, populating `val`.
 */

#define luna_hash_each_val(self, block) { \
    luna_object_t *val; \
    for (khiter_t k = kh_begin(self); k < kh_end(self); ++k) { \
      if (!kh_exist(self, k)) continue; \
      val = kh_value(self, k); \
      block; \
    } \
  }

// protos

void
luna_hash_set(khash_t(value) *self, char *key, luna_object_t *val);

luna_object_t *
luna_hash_get(khash_t(value) *self, char *key);

int
luna_hash_has(khash_t(value) *self, char *key);

void
luna_hash_remove(khash_t(value) *self, char *key);

#endif /* LUNA_HASH_H */