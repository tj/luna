
//
// prettyprint.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "ast.h"
#include "prettyprint.h"

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
 * Visit `block`.
 */

static void
block(luna_block_node_t *block) {
  printf("block\n");
}

/*
 * Visit `node`.
 */

static void
visit(luna_node_t *node) {
  switch (node->type) {
    case LUNA_NODE_BLOCK:
      block((luna_block_node_t *) node);
      break;
  }
}

