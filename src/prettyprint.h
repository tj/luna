
//
// prettyprint.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_PP__
#define __LUNA_PP__

#include "ast.h"

void
luna_set_prettyprint_func(int (*func)(const char *format, ...));

void
luna_prettyprint(luna_node_t *node);

#endif /* __LUNA_PP__ */
