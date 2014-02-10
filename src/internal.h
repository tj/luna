#pragma once

//
// internal.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

// branch prediction

#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define likely(expr) __builtin_expect((expr), 1)
#define unlikely(expr) __builtin_expect((expr), 0)
#else
#define likely(expr) (expr)
#define unlikely(expr) (expr)
#endif
