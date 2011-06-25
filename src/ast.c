
//
// ast.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "ast.h"

/*
 * Alloc and initialize a new block node.
 */

luna_block_node_t *
luna_block_node_new() {
  luna_block_node_t *self = malloc(sizeof(luna_block_node_t));
  self->base.type = LUNA_NODE_BLOCK;
  self->stmts = luna_array_new();
  return self;
}

/*
 * Alloc and initialize a new int node with the given `val`.
 */

luna_int_node_t *
luna_int_node_new(int val) {
  luna_int_node_t *self = malloc(sizeof(luna_int_node_t));
  self->base.type = LUNA_NODE_INT;
  self->val = val;
  return self;
}