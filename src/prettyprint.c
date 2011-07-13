
//
// prettyprint.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "ast.h"
#include "array.h"
#include "visitor.h"
#include "prettyprint.h"

// indentation level

static int indents = 0;

// output indentation

#define INDENT for (int j = 0; j < indents; ++j) printf("  ")

/*
 * Visit block `node`.
 */

static void
visit_block(luna_visitor_t *self, luna_block_node_t *node) {
  luna_array_each(node->stmts, {
    if (i) printf("\n");
    INDENT;
    visit((luna_node_t *) val->value.as_obj);
  });
}

/*
 * Visit int `node`.
 */

static void
visit_int(luna_visitor_t *self, luna_int_node_t *node) {
  printf("(int %d)", node->val);
}

/*
 * Visit float `node`.
 */

static void
visit_float(luna_visitor_t *self, luna_float_node_t *node) {
  printf("(float %f)", node->val);
}

/*
 * Visit id `node`.
 */

static void
visit_id(luna_visitor_t *self, luna_id_node_t *node) {
  printf("(id %s)", node->val);
}

/*
 * Visit string `node`.
 */

static void
visit_string(luna_visitor_t *self, luna_string_node_t *node) {
  printf("(string '%s')", node->val);
}

/*
 * Visit unary op `node`.
 */

static void
visit_unary_op(luna_visitor_t *self, luna_unary_op_node_t *node) {
  int c = node->postfix ? '@' : 0;
  printf("(%c%s ", c, luna_token_type_string(node->op));
  visit(node->expr);
  printf(")");
}

/*
 * Visit binary op `node`.
 */

static void
visit_binary_op(luna_visitor_t *self, luna_binary_op_node_t *node) {
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
visit_slot(luna_visitor_t *self, luna_slot_node_t *node) {
  printf("(slot\n");
  ++indents;
  INDENT;
  visit(node->left);
  printf("\n");
  INDENT;
  visit(node->right);
  --indents;
  printf(")");
}

/*
 * Visit call `node`.
 */

static void
visit_call(luna_visitor_t *self, luna_call_node_t * node) {
  printf("(call\n");
  ++indents;
  INDENT;
  visit((luna_node_t *) node->expr);
  if (node->args) {
    printf("\n");
    INDENT;
    luna_array_each(node->args, {
      visit((luna_node_t *) val->value.as_obj);
      if (i != len - 1) printf(" ");
    });
  }
  --indents;
  printf(")");
}

/*
 * Visit function `node`.
 */

static void
visit_function(luna_visitor_t *self, luna_function_node_t * node) {
  printf("(function ");
  luna_array_each(node->params, {
    printf("%s ", ((luna_id_node_t *) val->value.as_obj)->val);
  });
  printf("\n");
  ++indents;
  visit((luna_node_t *) node->block);
  --indents;
  printf(")");
}

/*
 * Visit `while` node.
 */

static void
visit_while(luna_visitor_t *self, luna_while_node_t *node) {
  // while | until
  printf("(%s ", node->negate ? "until" : "while");
  visit((luna_node_t *) node->expr);
  ++indents;
  printf("\n");
  visit((luna_node_t *) node->block);
  --indents;
  printf(")\n");
}

/*
 * Visit if `node`.
 */

static void
visit_if(luna_visitor_t *self, luna_if_node_t *node) {
  // if
  printf("(if ");
  visit((luna_node_t *) node->expr);
  ++indents;
  printf("\n");
  visit((luna_node_t *) node->block);
  --indents;
  printf(")");

  // else ifs
  luna_array_each(node->else_ifs, {
    luna_if_node_t *else_if = (luna_if_node_t *) val->value.as_obj;
    printf("\n(else if ");
    visit((luna_node_t *) else_if->expr);
    ++indents;
    printf("\n");
    visit((luna_node_t *) else_if->block);
    --indents;
    printf(")");
  });

  // else
  if (node->else_block) {
    printf("\n(else\n");
    ++indents;
    visit((luna_node_t *) node->else_block);
    --indents;
    printf(")");
  }
}

/*
 * Pretty-print the given `node` to stdout.
 */

void
luna_prettyprint(luna_node_t *node) {
  luna_visitor_t visitor = {
    .visit_if = visit_if,
    .visit_id = visit_id,
    .visit_int = visit_int,
    .visit_slot = visit_slot,
    .visit_call = visit_call,
    .visit_while = visit_while,
    .visit_block = visit_block,
    .visit_float = visit_float,
    .visit_string = visit_string,
    .visit_function = visit_function,
    .visit_unary_op = visit_unary_op,
    .visit_binary_op = visit_binary_op
  };

  luna_visit(&visitor, node);
  printf("\n");
}
