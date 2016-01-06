
//
// visitor.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_VISITOR_H
#define LUNA_VISITOR_H

#include "ast.h"

/*
 * Visit the given `node`.
 */

#define visit(node) luna_visit(self, node)

/*
 * Visitor struct.
 */

typedef struct luna_visitor {
  void *data;
  void (* visit_block)(struct luna_visitor *self, luna_block_node_t *node);
  void (* visit_id)(struct luna_visitor *self, luna_id_node_t *node);
  void (* visit_int)(struct luna_visitor *self, luna_int_node_t *node);
  void (* visit_float)(struct luna_visitor *self, luna_float_node_t *node);
  void (* visit_string)(struct luna_visitor *self, luna_string_node_t *node);
  void (* visit_slot)(struct luna_visitor *self, luna_slot_node_t *node);
  void (* visit_call)(struct luna_visitor *self, luna_call_node_t *node);
  void (* visit_while)(struct luna_visitor *self, luna_while_node_t *node);
  void (* visit_unary_op)(struct luna_visitor *self, luna_unary_op_node_t *node);
  void (* visit_binary_op)(struct luna_visitor *self, luna_binary_op_node_t *node);
  void (* visit_function)(struct luna_visitor *self, luna_function_node_t *node);
  void (* visit_array)(struct luna_visitor *self, luna_array_node_t *node);
  void (* visit_hash)(struct luna_visitor *self, luna_hash_node_t *node);
  void (* visit_return)(struct luna_visitor *self, luna_return_node_t *node);
  void (* visit_decl)(struct luna_visitor *self, luna_decl_node_t *node);
  void (* visit_if)(struct luna_visitor *self, luna_if_node_t *node);

  // TODO: discover why the program crashes if i put this somewhere else
  void (* visit_subscript)(struct luna_visitor *self, luna_subscript_node_t *node);
} luna_visitor_t;

// protos

void
luna_visit(luna_visitor_t *self, luna_node_t *node);

#endif /* LUNA_VISITOR_H */
