
//
// rewriter.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "ast.h"
#include "array.h"
#include "visitor.h"
#include "rewriter.h"

/*
 * Optimize the AST, starting by visiting `node`.
 */

void
luna_rewrite(luna_node_t *node) {
  luna_visitor_t visitor = {
    // .visit_if = visit_if,
    // .visit_id = visit_id,
    // .visit_int = visit_int,
    // .visit_slot = visit_slot,
    // .visit_call = visit_call,
    // .visit_while = visit_while,
    // .visit_block = visit_block,
    // .visit_float = visit_float,
    // .visit_string = visit_string,
    // .visit_function = visit_function,
    // .visit_unary_op = visit_unary_op,
    // .visit_binary_op = visit_binary_op
  };

  luna_visit(&visitor, node);

  printf("\n");
}