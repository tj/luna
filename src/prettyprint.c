
//
// prettyprint.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "ast.h"
#include "prettyprint.h"

// indentation level

static int indents = 0;

// forward declarations

static void visit(luna_node_t *node);

/*
 * Pretty-print the given `node` to stdout.
 */

void
luna_prettyprint(luna_node_t *node) {
  visit(node);
}

/*
 * Visit block `node`.
 */

static void
visit_block(luna_block_node_t *node) {
  luna_array_each(node->stmts, {
    visit((luna_node_t *) val->value.as_obj);
    printf("\n");
    for (int j = 0; j < indents; ++j) printf("  ");
  });
}

/*
 * Visit int `node`.
 */

static void
visit_int(luna_int_node_t *node) {
  printf("(int %d)", node->val);
}

/*
 * Visit float `node`.
 */

static void
visit_float(luna_float_node_t *node) {
  printf("(float %f)", node->val);
}

/*
 * Visit id `node`.
 */

static void
visit_id(luna_id_node_t *node) {
  printf("(id %s)", node->val);
}

/*
 * Visit string `node`.
 */

static void
visit_string(luna_string_node_t *node) {
  printf("(string '%s')", node->val);
}

/*
 * Visit unary op `node`.
 */

static void
visit_unary_op(luna_unary_op_node_t *node) {
  printf("(%s ", luna_token_type_string(node->op));
  visit(node->expr);
  printf(")");
}

/*
 * Visit binary op `node`.
 */

static void
visit_binary_op(luna_binary_op_node_t *node) {
  printf("(%s ", luna_token_type_string(node->op));
  visit(node->left);
  printf(" ");
  visit(node->right);
  printf(")");
}

/*
 * Visit slot `node`.
 */

static void
visit_slot(luna_slot_node_t *node) {
  visit(node->expr);
  printf(" (slot %s)", node->slot);
}

/*
 * Visit function `node`.
 */

static void
visit_function(luna_function_node_t * node) {
  printf("(function");
  ++indents;
  visit((luna_node_t *) node->block);
  --indents;
  printf(")");
}

/*
 * Visit `node`.
 */

static void
visit(luna_node_t *node) {
  switch (node->type) {
    case LUNA_NODE_BLOCK:
      visit_block((luna_block_node_t *) node);
      break;
    case LUNA_NODE_ID:
      visit_id((luna_id_node_t *) node);
      break;
    case LUNA_NODE_INT:
      visit_int((luna_int_node_t *) node);
      break;
    case LUNA_NODE_FLOAT:
      visit_float((luna_float_node_t *) node);
      break;
    case LUNA_NODE_STRING:
      visit_string((luna_string_node_t *) node);
      break;
    case LUNA_NODE_SLOT:
      visit_slot((luna_slot_node_t *) node);
      break;
    case LUNA_NODE_UNARY_OP:
      visit_unary_op((luna_unary_op_node_t *) node);
      break;
    case LUNA_NODE_BINARY_OP:
      visit_binary_op((luna_binary_op_node_t *) node);
      break;
    case LUNA_NODE_FUNCTION:
      visit_function((luna_function_node_t *) node);
      break;
  }
}

