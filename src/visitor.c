
//
// visitor.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "visitor.h"

/*
 * Visit the node when a `type` callback is present.
 */

#define VISIT(type) \
  if (!self->visit_##type) return; \
  self->visit_##type(self, (luna_##type##_node_t *) node); \
  break;

/*
 * Visit `node`, invoking the associated callback.
 */

void
luna_visit(luna_visitor_t *self, luna_node_t *node) {
  switch (node->type) {
    case LUNA_NODE_BLOCK: VISIT(block);
    case LUNA_NODE_ID: VISIT(id);
    case LUNA_NODE_DECL: VISIT(decl);
    case LUNA_NODE_INT: VISIT(int);
    case LUNA_NODE_FLOAT: VISIT(float);
    case LUNA_NODE_STRING: VISIT(string);
    case LUNA_NODE_SLOT: VISIT(slot);
    case LUNA_NODE_CALL: VISIT(call);
    case LUNA_NODE_IF: VISIT(if);
    case LUNA_NODE_WHILE: VISIT(while);
    case LUNA_NODE_UNARY_OP: VISIT(unary_op);
    case LUNA_NODE_BINARY_OP: VISIT(binary_op);
    case LUNA_NODE_FUNCTION: VISIT(function);
    case LUNA_NODE_ARRAY: VISIT(array);
    case LUNA_NODE_HASH: VISIT(hash);
    case LUNA_NODE_RETURN: VISIT(return);
  }
}