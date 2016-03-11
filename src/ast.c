
//
// ast.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include "vec.h"
#include "hash.h"
#include "ast.h"
#include "internal.h"

/*
 * Alloc a luna value and assign the given `node`.
 */

luna_object_t *
luna_node(luna_node_t *node) {
  luna_object_t *self = malloc(sizeof(luna_object_t));
  if (unlikely(!self)) return NULL;
  self->type = LUNA_TYPE_NODE;
  self->value.as_pointer = node;
  return self;
}

/*
 * Alloc and initialize a new block node.
 */

luna_block_node_t *
luna_block_node_new(int lineno) {
  luna_block_node_t *self = malloc(sizeof(luna_block_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_BLOCK;
  self->base.lineno = lineno;
  self->stmts = luna_vec_new();
  return self;
}

/*
 * Alloc and initialize a new args node.
 */

luna_args_node_t *
luna_args_node_new(int lineno) {
  luna_args_node_t *self = malloc(sizeof(luna_args_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_ARGS;
  self->base.lineno = lineno;
  self->vec = luna_vec_new();
  self->hash = luna_hash_new();
  return self;
}

/*
 * Alloc and initialize a new int node with the given `val`.
 */

luna_int_node_t *
luna_int_node_new(int val, int lineno) {
  luna_int_node_t *self = malloc(sizeof(luna_int_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_INT;
  self->base.lineno = lineno;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new float node with the given `val`.
 */

luna_float_node_t *
luna_float_node_new(float val, int lineno) {
  luna_float_node_t *self = malloc(sizeof(luna_float_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_FLOAT;
  self->base.lineno = lineno;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new id node with the given `val`.
 */

luna_id_node_t *
luna_id_node_new(const char *val, int lineno) {
  luna_id_node_t *self = malloc(sizeof(luna_id_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_ID;
  self->base.lineno = lineno;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new declaration node with the
 * given `name`, `type`, and `val`.
 */

luna_decl_node_t *
luna_decl_node_new(luna_vec_t *vec, luna_node_t *type, int lineno) {
  luna_decl_node_t *self = malloc(sizeof(luna_decl_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_DECL;
  self->base.lineno = lineno;
  self->vec = vec;
  self->type = type;
  return self;
}

/*
 * Alloc and initialize a new let node with the
 * given `decl` and `val`.
 */

luna_let_node_t *
luna_let_node_new(luna_vec_t *vec, int lineno) {
  luna_let_node_t *self = malloc(sizeof(luna_let_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_LET;
  self->base.lineno = lineno;
  self->vec = vec;
  return self;
}

/*
 * Alloc and initialize a new string node with the given `val`.
 */

luna_string_node_t *
luna_string_node_new(const char *val, int lineno) {
  luna_string_node_t *self = malloc(sizeof(luna_string_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_STRING;
  self->base.lineno = lineno;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new call node with the given `expr`.
 */

luna_call_node_t *
luna_call_node_new(luna_node_t *expr, int lineno) {
  luna_call_node_t *self = malloc(sizeof(luna_call_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_CALL;
  self->base.lineno = lineno;
  self->expr = expr;
  self->args = luna_args_node_new(lineno);
  if (unlikely(!self->args)) return NULL;
  return self;
}

/*
 * Alloc and initialize subscript node with `left` and `right`.
 */
 
luna_subscript_node_t *
luna_subscript_node_new(luna_node_t *left, luna_node_t *right, int lineno) {
  luna_subscript_node_t *self = malloc(sizeof(luna_subscript_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_SUBSCRIPT;
  self->base.lineno = lineno;
  self->left = left;
  self->right = right;
  return self;
}

/*
 * Alloc and initialize slot access node with `left` and `right`.
 */

luna_slot_node_t *
luna_slot_node_new(luna_node_t *left, luna_node_t *right, int lineno) {
  luna_slot_node_t *self = malloc(sizeof(luna_slot_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_SLOT;
  self->base.lineno = lineno;
  self->left = left;
  self->right = right;
  return self;
}

/*
 * Alloc and initialize a unary `op` node with `expr` node.
 */

luna_unary_op_node_t *
luna_unary_op_node_new(luna_token op, luna_node_t *expr, int postfix, int lineno) {
  luna_unary_op_node_t *self = malloc(sizeof(luna_unary_op_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_UNARY_OP;
  self->base.lineno = lineno;
  self->op = op;
  self->expr = expr;
  self->postfix = postfix;
  return self;
}

/*
 * Alloc and initialize a binary `op` node with `left` and `right` nodes.
 */

luna_binary_op_node_t *
luna_binary_op_node_new(luna_token op, luna_node_t *left, luna_node_t *right, int lineno) {
  luna_binary_op_node_t *self = malloc(sizeof(luna_binary_op_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_BINARY_OP;
  self->base.lineno = lineno;
  self->op = op;
  self->left = left;
  self->right = right;
  self->let = 0;
  return self;
}

/*
 * Alloc and initialize a new array node.
 */

luna_array_node_t *
luna_array_node_new(int lineno) {
  luna_array_node_t *self = malloc(sizeof(luna_array_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_ARRAY;
  self->base.lineno = lineno;
  self->vals = luna_vec_new();
  return self;
}

/*
 * Alloc and initialize a new hash node.
 */
 
luna_hash_pair_node_t *
luna_hash_pair_node_new(int lineno) {
  luna_hash_pair_node_t *self = malloc(sizeof(luna_hash_pair_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_HASH_PAIR;
  self->base.lineno = lineno;
  self->key = NULL;
  self->val = NULL;
  return self;
}

/*
 * Alloc and initialize a new hash node.
 */

luna_hash_node_t *
luna_hash_node_new(int lineno) {
  luna_hash_node_t *self = malloc(sizeof(luna_hash_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_HASH;
  self->base.lineno = lineno;
  self->pairs = luna_vec_new();
  return self;
}

/*
 * Alloc and initialize a new function node with the given `name`,
 * `type`, `block` of statements and `params`.
 */

luna_function_node_t *
luna_function_node_new(const char *name, luna_node_t *type, luna_block_node_t *block, luna_vec_t *params, int lineno) {
  luna_function_node_t *self = malloc(sizeof(luna_function_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_FUNCTION;
  self->base.lineno = lineno;
  self->params = params;
  self->block = block;
  self->type = type;
  self->name = name;
  return self;
}

/*
 * Alloc and initialize a new function node with the given `expr`,
 * with an implicit return.
 */

luna_function_node_t *
luna_function_node_new_from_expr(luna_node_t *expr, luna_vec_t *params, int lineno) {
  luna_function_node_t *self = malloc(sizeof(luna_function_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_FUNCTION;
  self->base.lineno = lineno;
  self->params = params;

  // block
  self->block = luna_block_node_new(lineno);
  if (unlikely(!self->block)) return NULL;

  // return
  luna_return_node_t *ret = luna_return_node_new(expr, lineno);
  luna_vec_push(self->block->stmts, luna_node((luna_node_t *) ret));

  return self;
}

/*
 * Alloc and initialize a new type noe with the given `name`.
 */

luna_type_node_t *
luna_type_node_new(const char *name, int lineno) {
  luna_type_node_t *self = malloc(sizeof(luna_type_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_TYPE;
  self->base.lineno = lineno;
  self->name = name;
  self->fields = luna_vec_new();
  return self;
}

/*
 * Alloc and initialize a new if stmt node, negated for "unless",
 * with required `expr` and `block`.
 */

luna_if_node_t *
luna_if_node_new(int negate, luna_node_t *expr, luna_block_node_t *block, int lineno) {
  luna_if_node_t *self = malloc(sizeof(luna_if_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_IF;
  self->base.lineno = lineno;
  self->negate = negate;
  self->expr = expr;
  self->block = block;
  self->else_block = NULL;
  self->else_ifs = luna_vec_new();
  return self;
}

/*
 * Alloc and initialize a new while loop node, negated for "until",
 * otherwise "while", with required `expr` and `block`.
 */

luna_while_node_t *
luna_while_node_new(int negate, luna_node_t *expr, luna_block_node_t *block, int lineno) {
  luna_while_node_t *self = malloc(sizeof(luna_while_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_WHILE;
  self->base.lineno = lineno;
  self->negate = negate;
  self->expr = expr;
  self->block = block;
  return self;
}

/*
 * Alloc and initialize a new return node with the given `expr`.
 */

luna_return_node_t *
luna_return_node_new(luna_node_t *expr, int lineno) {
  luna_return_node_t *self = malloc(sizeof(luna_return_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_RETURN;
  self->base.lineno = lineno;
  self->expr = expr;
  return self;
}

/*
 * Alloc and initialize a new use node.
 */

luna_use_node_t *
luna_use_node_new(int lineno) {
  luna_use_node_t *self = malloc(sizeof(luna_use_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_USE;
  self->base.lineno = lineno;
  self->module = NULL;
  self->alias = NULL;
  return self;
}