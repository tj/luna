
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
      self->visit_block(self, (luna_block_node_t *) node);
      break;
    case LUNA_NODE_ID:
      self->visit_id(self, (luna_id_node_t *) node);
      break;
    case LUNA_NODE_INT:
      self->visit_int(self, (luna_int_node_t *) node);
      break;
    case LUNA_NODE_FLOAT:
      self->visit_float(self, (luna_float_node_t *) node);
      break;
    case LUNA_NODE_STRING:
      self->visit_string(self, (luna_string_node_t *) node);
      break;
    case LUNA_NODE_SLOT:
      self->visit_slot(self, (luna_slot_node_t *) node);
      break;
    case LUNA_NODE_CALL:
      self->visit_call(self, (luna_call_node_t *) node);
      break;
    case LUNA_NODE_IF:
      self->visit_if(self, (luna_if_node_t *) node);
      break;
    case LUNA_NODE_WHILE:
      self->visit_while(self, (luna_while_node_t *) node);
      break;
    case LUNA_NODE_UNARY_OP:
      self->visit_unary_op(self, (luna_unary_op_node_t *) node);
      break;
    case LUNA_NODE_BINARY_OP:
      self->visit_binary_op(self, (luna_binary_op_node_t *) node);
      break;
    case LUNA_NODE_FUNCTION:
      self->visit_function(self, (luna_function_node_t *) node);
      break;
  }
}