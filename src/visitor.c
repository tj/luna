
//
// visitor.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "visitor.h"

/*
 * Visit `node`, invoking the associated callback.
 */

void
luna_visit(luna_visitor_t *self, luna_node_t *node) {
  switch (node->type) {
    case LUNA_NODE_BLOCK:
      if (!self->visit_block) return;
      self->visit_block(self, (luna_block_node_t *) node);
      break;
    case LUNA_NODE_ID:
      if (!self->visit_id) return;
      self->visit_id(self, (luna_id_node_t *) node);
      break;
    case LUNA_NODE_INT:
      if (!self->visit_int) return;
      self->visit_int(self, (luna_int_node_t *) node);
      break;
    case LUNA_NODE_FLOAT:
      if (!self->visit_float) return;
      self->visit_float(self, (luna_float_node_t *) node);
      break;
    case LUNA_NODE_STRING:
      if (!self->visit_string) return;
      self->visit_string(self, (luna_string_node_t *) node);
      break;
    case LUNA_NODE_SLOT:
      if (!self->visit_slot) return;
      self->visit_slot(self, (luna_slot_node_t *) node);
      break;
    case LUNA_NODE_CALL:
      if (!self->visit_call) return;
      self->visit_call(self, (luna_call_node_t *) node);
      break;
    case LUNA_NODE_IF:
      if (!self->visit_if) return;
      self->visit_if(self, (luna_if_node_t *) node);
      break;
    case LUNA_NODE_WHILE:
      if (!self->visit_while) return;
      self->visit_while(self, (luna_while_node_t *) node);
      break;
    case LUNA_NODE_UNARY_OP:
      if (!self->visit_unary_op) return;
      self->visit_unary_op(self, (luna_unary_op_node_t *) node);
      break;
    case LUNA_NODE_BINARY_OP:
      if (!self->visit_binary_op) return;
      self->visit_binary_op(self, (luna_binary_op_node_t *) node);
      break;
    case LUNA_NODE_FUNCTION:
      if (!self->visit_function) return;
      self->visit_function(self, (luna_function_node_t *) node);
      break;
    case LUNA_NODE_ARRAY:
      if (!self->visit_array) return;
      self->visit_array(self, (luna_array_node_t *) node);
      break;
  }
}