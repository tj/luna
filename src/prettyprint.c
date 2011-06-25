
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
  });
}

/*
 * Visit int `node`.
 */

static void
visit_int(luna_int_node_t *node) {
  printf("(int %d)\n", node->val);
}

/*
 * Visit float `node`.
 */

static void
visit_float(luna_float_node_t *node) {
  printf("(float %f)\n", node->val);
}

/*
 * Visit string `node`.
 */

static void
visit_string(luna_string_node_t *node) {
  printf("(string '%s')\n", node->val);
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
    case LUNA_NODE_INT:
      visit_int((luna_int_node_t *) node);
      break;
    case LUNA_NODE_FLOAT:
      visit_float((luna_float_node_t *) node);
      break;
    case LUNA_NODE_STRING:
      visit_string((luna_string_node_t *) node);
      break;
  }
}

