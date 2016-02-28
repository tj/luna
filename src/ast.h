
//
// ast.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_AST_H
#define LUNA_AST_H

#include "token.h"
#include "vec.h"
#include "object.h"

/*
 * Nodes.
 */

#define LUNA_NODE_LIST \
  n(BLOCK) \
  n(EXPR_STMT) \
  n(RETURN) \
  n(DEF) \
  n(IF) \
  n(WHILE) \
  n(FOR) \
  n(UNARY_OP) \
  n(BINARY_OP) \
  n(TERNARY_OP) \
  n(BOOL) \
  n(NULL) \
  n(ID) \
  n(DECL) \
  n(LET) \
  n(CALL) \
  n(ARGS) \
  n(INT) \
  n(FLOAT) \
  n(STRING) \
  n(ARRAY) \
  n(HASH_PAIR) \
  n(HASH) \
  n(FUNCTION) \
  n(TYPE) \
  n(SLOT) \
  n(SUBSCRIPT)

/*
 * Nodes enum.
 */

typedef enum {
#define n(node) LUNA_NODE_##node,
LUNA_NODE_LIST
#undef n
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
  luna_vec_t *stmts;
} luna_block_node_t;

/*
 * Luna args node.
 */

typedef struct {
  luna_node_t base;
  luna_vec_t *vec;
  luna_hash_t *hash;
} luna_args_node_t;

/*
 * Luna subscript node.
 */

typedef struct {
  luna_node_t base;
  luna_node_t *left;
  luna_node_t *right;
} luna_subscript_node_t;

/*
 * Luna slot access node.
 */

typedef struct {
  luna_node_t base;
  luna_node_t *left;
  luna_node_t *right;
} luna_slot_node_t;

/*
 * Luna unary operation node.
 */

typedef struct {
  luna_node_t base;
  luna_token op;
  luna_node_t *expr;
  int postfix;
} luna_unary_op_node_t;

/*
 * Luna binary operation node.
 */

typedef struct {
  luna_node_t base;
  luna_token op;
  luna_node_t *left;
  luna_node_t *right;
  int let;
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
 * Luna declaration node.
 */

typedef struct {
  luna_node_t base;
  luna_vec_t *vec;
  luna_node_t *type;
} luna_decl_node_t;

/*
 * Luna let node.
 */

typedef struct {
  luna_node_t base;
  luna_node_t *decl;
  luna_node_t *val;
} luna_let_node_t;

/*
 * Luna string node.
 */

typedef struct {
  luna_node_t base;
  const char *val;
} luna_string_node_t;

/*
 * Luna array node.
 */

typedef struct {
  luna_node_t base;
  luna_vec_t *vals;
} luna_array_node_t;

/*
 * Luna hash pair node.
 */
typedef struct {
  luna_node_t base;
  luna_node_t *key;
  luna_node_t *val;
} luna_hash_pair_node_t;

/*
 * Luna hash node.
 */

typedef struct {
  luna_node_t base;
  luna_vec_t *pairs;
} luna_hash_node_t;

/*
 * Luna call node.
 */

typedef struct {
  luna_node_t base;
  luna_node_t *expr;
  luna_args_node_t *args;
} luna_call_node_t;

/*
 * Luna function node.
 */

typedef struct {
  luna_node_t base;
  const char *name;
  luna_node_t *type;
  luna_block_node_t *block;
  luna_vec_t *params;
} luna_function_node_t;

/*
 * Luna type node.
 */

typedef struct {
  luna_node_t base;
  const char *name;
  luna_vec_t *fields;
} luna_type_node_t;

/*
 * Luna if stmt node.
 */

typedef struct {
  luna_node_t base;
  int negate;
  luna_node_t *expr;
  luna_block_node_t *block;
  luna_block_node_t *else_block;
  luna_vec_t *else_ifs;
} luna_if_node_t;

/*
 * Luna while loop stmt node.
 */

typedef struct {
  luna_node_t base;
  int negate;
  luna_node_t *expr;
  luna_block_node_t *block;
} luna_while_node_t;

/*
 * Luna return node.
 */

typedef struct {
  luna_node_t base;
  luna_node_t *expr;
} luna_return_node_t;

// protos

luna_object_t *
luna_node(luna_node_t *node);

luna_block_node_t *
luna_block_node_new();

luna_function_node_t *
luna_function_node_new(const char *name, luna_node_t *type, luna_block_node_t *block, luna_vec_t *params);

luna_function_node_t *
luna_function_node_new_from_expr(luna_node_t *expr, luna_vec_t *params);

luna_subscript_node_t *
luna_subscript_node_new(luna_node_t *left, luna_node_t *right);

luna_slot_node_t *
luna_slot_node_new(luna_node_t *left, luna_node_t *right);

luna_call_node_t *
luna_call_node_new(luna_node_t *expr);

luna_unary_op_node_t *
luna_unary_op_node_new(luna_token op, luna_node_t *expr, int postfix);

luna_binary_op_node_t *
luna_binary_op_node_new(luna_token op, luna_node_t *left, luna_node_t *right);

luna_id_node_t *
luna_id_node_new(const char *val);

luna_decl_node_t *
luna_decl_node_new(luna_vec_t *vec, luna_node_t *type);

luna_let_node_t *
luna_let_node_new(luna_node_t *decl, luna_node_t *val);

luna_int_node_t *
luna_int_node_new(int val);

luna_float_node_t *
luna_float_node_new(float val);

luna_array_node_t *
luna_array_node_new();

luna_hash_pair_node_t *
luna_hash_pair_node_new();

luna_hash_node_t *
luna_hash_node_new();

luna_string_node_t *
luna_string_node_new(const char *val);

luna_if_node_t *
luna_if_node_new(int negate, luna_node_t *expr, luna_block_node_t *block);

luna_while_node_t *
luna_while_node_new(int negate, luna_node_t *expr, luna_block_node_t *block);

luna_return_node_t *
luna_return_node_new(luna_node_t *expr);

luna_args_node_t *
luna_args_node_new();

luna_type_node_t *
luna_type_node_new(const char *name);

#endif /* LUNA_AST_H */