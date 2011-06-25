
//
// ast.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_AST__
#define __LUNA_AST__

#include "object.h"

/*
 * Nodes.
 */

#define LUNA_NODE_LIST \
  n(RETURN) \
  n(IF) \
  n(FOR) \
  n(UNARY_OP) \
  n(BINARY_OP) \
  n(TERNARY_OP) \
  n(BOOL) \
  n(NULL) \
  n(INT) \
  n(FLOAT) \
  n(STRING) \
  n(FUNCTION)


/*
 * Nodes enum.
 */

typedef enum {
#define n(node) LUNA_NODE_##node,
LUNA_NODE_LIST
#undef t
} luna_node;

/*
 * Luna ast node.
 */

typedef struct {
  luna_node type;
  luna_value_t args[3];
} luna_node_t;

#endif /* __LUNA_AST__ */