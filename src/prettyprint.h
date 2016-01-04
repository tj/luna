
//
// prettyprint.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_PP_H
#define LUNA_PP_H

#include "ast.h"

void
luna_set_prettyprint_func(int (*func)(const char *format, ...));

void
luna_prettyprint(luna_node_t *node);

#endif /* LUNA_PP_H */
