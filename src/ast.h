
//
// ast.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_AST__
#define __LUNA_AST__

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

#endif /* __LUNA_AST__ */