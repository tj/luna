
//
// ast.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_AST__
#define __LUNA_AST__

#include "token.h"
#include "object.h"
#include "array.h"

/*
 * Nodes.
 */

#define LUNA_NODE_LIST \
  n(BLOCK) \
  n(EXPR_STMT) \
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
 * Luna node.
 */

typedef struct {
  luna_node type;
  int lineno;
} luna_node_t;

/*
 * Luna block node.
 */

typedef struct {
  luna_node_t base;
  luna_array_t *stmts;
} luna_block_node_t;

/*
 * Luna binary operation node.
 */

typedef struct {
  luna_node_t base;
  luna_token type;
  luna_node_t *left;
  luna_node_t *right;
} luna_binary_op_node_t;

/*
 * Luna int node.
 */

typedef struct {
  int val;
} luna_int_node_t;

// protos

luna_block_node_t *
luna_block_node_new();

#endif /* __LUNA_AST__ */