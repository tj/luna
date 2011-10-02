
//
// luna.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA__
#define __LUNA__

/*
 * Luna version.
 */

#define LUNA_VERSION "0.0.1"

// branch prediction

#define likely(val) __builtin_expect((val), 1)
#define unlikely(val) __builtin_expect((val), 0)

#endif /* __LUNA__ */