
//
// ast.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "vec.h"
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
luna_block_node_new() {
  luna_block_node_t *self = malloc(sizeof(luna_block_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_BLOCK;
  self->stmts = luna_vec_new();
  return self;
}

/*
 * Alloc and initialize a new int node with the given `val`.
 */

luna_int_node_t *
luna_int_node_new(int val) {
  luna_int_node_t *self = malloc(sizeof(luna_int_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_INT;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new float node with the given `val`.
 */

luna_float_node_t *
luna_float_node_new(float val) {
  luna_float_node_t *self = malloc(sizeof(luna_float_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_FLOAT;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new id node with the given `val`.
 */

luna_id_node_t *
luna_id_node_new(const char *val) {
  luna_id_node_t *self = malloc(sizeof(luna_id_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_ID;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new string node with the given `val`.
 */

luna_string_node_t *
luna_string_node_new(const char *val) {
  luna_string_node_t *self = malloc(sizeof(luna_string_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_STRING;
  self->val = val;
  return self;
}

/*
 * Alloc and initialize a new call node with the given `expr`.
 */

luna_call_node_t *
luna_call_node_new(luna_node_t *expr) {
  luna_call_node_t *self = malloc(sizeof(luna_call_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_CALL;
  self->expr = expr;
  self->args = NULL;
  return self;
}

/*
 * Alloc and initialize slot access node with `left` and `right`.
 */

luna_slot_node_t *
luna_slot_node_new(luna_node_t *left, luna_node_t *right) {
  luna_slot_node_t *self = malloc(sizeof(luna_slot_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_SLOT;
  self->left = left;
  self->right = right;
  return self;
}

/*
 * Alloc and initialize a unary `op` node with `expr` node.
 */

luna_unary_op_node_t *
luna_unary_op_node_new(luna_token op, luna_node_t *expr, int postfix) {
  luna_unary_op_node_t *self = malloc(sizeof(luna_unary_op_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_UNARY_OP;
  self->op = op;
  self->expr = expr;
  self->postfix = postfix;
  return self;
}

/*
 * Alloc and initialize a binary `op` node with `left` and `right` nodes.
 */

luna_binary_op_node_t *
luna_binary_op_node_new(luna_token op, luna_node_t *left, luna_node_t *right) {
  luna_binary_op_node_t *self = malloc(sizeof(luna_binary_op_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_BINARY_OP;
  self->op = op;
  self->left = left;
  self->right = right;
  return self;
}

/*
 * Alloc and initialize a new array node.
 */

luna_array_node_t *
luna_array_node_new() {
  luna_array_node_t *self = malloc(sizeof(luna_array_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_ARRAY;
  self->vals = luna_vec_new();
  return self;
}

/*
 * Alloc and initialize a new function node with the given `block`.
 */

luna_function_node_t *
luna_function_node_new(luna_block_node_t *block, luna_vec_t *params) {
  luna_function_node_t *self = malloc(sizeof(luna_function_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_FUNCTION;
  self->params = params;
  self->block = block;
  return self;
}

/*
 * Alloc and initialize a new function node with the given `expr`,
 * with an implicit return.
 */

luna_function_node_t *
luna_function_node_new_from_expr(luna_node_t *expr, luna_vec_t *params) {
  luna_function_node_t *self = malloc(sizeof(luna_function_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_FUNCTION;
  self->params = params;

  // block
  self->block = luna_block_node_new();
  if (unlikely(!self->block)) return NULL;

  // return
  luna_return_node_t *ret = luna_return_node_new(expr);
  ret->expr = expr; // TODO: ...?
  luna_vec_push(self->block->stmts, luna_node(ret));

  return self;
}

/*
 * Alloc and initialize a new if stmt node, negated for "unless",
 * with required `expr` and `block`.
 */

luna_if_node_t *
luna_if_node_new(int negate, luna_node_t *expr, luna_block_node_t *block) {
  luna_if_node_t *self = malloc(sizeof(luna_if_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_IF;
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
luna_while_node_new(int negate, luna_node_t *expr, luna_block_node_t *block) {
  luna_while_node_t *self = malloc(sizeof(luna_while_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_WHILE;
  self->negate = negate;
  self->expr = expr;
  self->block = block;
  return self;
}

/*
 * Alloc and initialize a new return node with the given `expr`.
 */

luna_return_node_t *
luna_return_node_new(luna_node_t *expr) {
  luna_return_node_t *self = malloc(sizeof(luna_return_node_t));
  if (unlikely(!self)) return NULL;
  self->base.type = LUNA_NODE_RETURN;
  self->expr = expr;
  return self;
}
