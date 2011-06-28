
//
// ast.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_AST__
#define __LUNA_AST__

#include "token.h"
#include "array.h"
#include "object.h"

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
  n(ID) \
  n(INT) \
  n(FLOAT) \
  n(STRING) \
  n(FUNCTION) \
  n(SLOT)


/*
 * Nodes enum.
 */

typedef enum {
#define n(node) LUNA_NODE_##node,
LUNA_NODE_LIST
#undef t
} luna_node_type;

/*
 * Luna node.
 */

typedef struct {
  luna_node_type type;
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
 * Luna slot access node.
 */

typedef struct {
  luna_node_t base;
  luna_node_t *expr;
  const char *slot;
} luna_slot_node_t;

/*
 * Luna unary operation node.
 */

typedef struct {
  luna_node_t base;
  luna_token op;
  luna_node_t *expr;
} luna_unary_op_node_t;

/*
 * Luna binary operation node.
 */

typedef struct {
  luna_node_t base;
  luna_token op;
  luna_node_t *left;
  luna_node_t *right;
} luna_binary_op_node_t;

/*
 * Luna int node.
 */

typedef struct {
  luna_node_t base;
  int val;
} luna_int_node_t;

/*
 * Luna float node.
 */

typedef struct {
  luna_node_t base;
  float val;
} luna_float_node_t;

/*
 * Luna id node.
 */

typedef struct {
  luna_node_t base;
  const char *val;
} luna_id_node_t;

/*
 * Luna string node.
 */

typedef struct {
  luna_node_t base;
  const char *val;
} luna_string_node_t;

// protos

luna_value_t *
luna_node(luna_node_t *node);

luna_block_node_t *
luna_block_node_new();

luna_slot_node_t *
luna_slot_node_new(luna_node_t *expr, const char *slot);

luna_unary_op_node_t *
luna_unary_op_node_new(luna_token op, luna_node_t *expr);

luna_binary_op_node_t *
luna_binary_op_node_new(luna_token op, luna_node_t *left, luna_node_t *right);

luna_id_node_t *
luna_id_node_new(const char *val);

luna_int_node_t *
luna_int_node_new(int val);

luna_float_node_t *
luna_float_node_new(float val);

luna_string_node_t *
luna_string_node_new(const char *val);

#endif /* __LUNA_AST__ */