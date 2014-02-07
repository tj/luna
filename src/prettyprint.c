
//
// prettyprint.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "ast.h"
#include "vec.h"
#include "visitor.h"
#include "prettyprint.h"

// indentation level

static int indents = 0;

// output indentation

#define INDENT for (int j = 0; j < indents; ++j) printf("  ")

/*
 * Escape chars.
 */

static char escapes[] = {
  'a',
  'b',
  't',
  'n',
  'v',
  'f',
  'r'
};

/*
 * Return the length of an "inspected" string
 * including the null byte.
 */

static int
inspect_length(const char *str) {
  int len = 0;
  for (int i = 0; str[i]; ++i) {
    switch (str[i]) {
      case '\a':
      case '\b':
      case '\e':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
      case '\v':
        len += 2;
        break;
      default:
        ++len;
    }
  }
  return len + 1;
}

/*
 * Return an "inspected" version of the string.
 */

static const char *
inspect(const char *str) {
  int j = 0;
  int len = inspect_length(str);
  char *buf = malloc(len);
  for (int i = 0; str[i]; ++i) {
    switch (str[i]) {
      case '\a':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
      case '\v':
        buf[j++] = '\\';
        buf[j++] = escapes[str[i]-7];
        break;
      case '\e':
        buf[j++] = '\\';
        buf[j++] = 'e';
        break;
      default:
        buf[j++] = str[i];
    }
  }
  buf[j] = 0;
  return buf;
}

/*
 * Visit block `node`.
 */

static void
visit_block(luna_visitor_t *self, luna_block_node_t *node) {
  luna_vec_each(node->stmts, {
    if (i) printf("\n");
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
    if (!indents) printf("\n");
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
  node->val < 0.000001 ?
    printf("(float %e)", node->val):
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
 * Visit decl `node`.
 */

static void
visit_decl(luna_visitor_t *self, luna_decl_node_t *node) {
  printf("(decl %s:%s", node->name, node->type ? node->type : "");
  if (node->val) visit(node->val);
  printf(")");
}

/*
 * Visit string `node`.
 */

static void
visit_string(luna_visitor_t *self, luna_string_node_t *node) {
  printf("(string '%s')", inspect(node->val));
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
  if (node->let) {
    printf("(let %s ", luna_token_type_string(node->op));
  } else {
    printf("(%s ", luna_token_type_string(node->op));
  }
  visit(node->left);
  printf(" ");
  visit(node->right);
  printf(")");
}

/*
 * Visit array `node`.
 */

static void
visit_array(luna_visitor_t *self, luna_array_node_t *node) {
  printf("(array\n");
  ++indents;
  luna_vec_each(node->vals, {
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
    if (i != len - 1) printf("\n");
  });
  --indents;
  printf(")");
}

/*
 * Visit hash `node`.
 */

static void
visit_hash(luna_visitor_t *self, luna_hash_node_t *node) {
  printf("(hash\n");
  ++indents;
  luna_hash_each(node->vals, {
    INDENT;
    printf("%s: ", slot);
    visit((luna_node_t *) val->value.as_pointer);
    printf("\n");
  });
  --indents;
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
  if (luna_vec_length(node->args->vec)) {
    printf("\n");
    INDENT;
    luna_vec_each(node->args->vec, {
      visit((luna_node_t *) val->value.as_pointer);
      if (i != len - 1) printf(" ");
    });

    luna_hash_each(node->args->hash, {
      printf(" %s: ", slot);
      visit((luna_node_t *) val->value.as_pointer);
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
  printf("(function %s -> %s", node->name, node->type ? node->type : "");
  ++indents;
  luna_vec_each(node->params, {
    printf("\n");
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
  });
  --indents;
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
 * Visit `return` node.
 */

static void
visit_return(luna_visitor_t *self, luna_return_node_t *node) {
  printf("(return");
  if (node->expr) {
    ++indents;
    printf("\n");
    INDENT;
    visit((luna_node_t *) node->expr);
    --indents;
  }
  printf(")");
}

/*
 * Visit if `node`.
 */

static void
visit_if(luna_visitor_t *self, luna_if_node_t *node) {
  // if
  printf("(%s ", node->negate ? "unless" : "if");
  visit((luna_node_t *) node->expr);
  ++indents;
  printf("\n");
  visit((luna_node_t *) node->block);
  --indents;
  printf(")");

  // else ifs
  luna_vec_each(node->else_ifs, {
    luna_if_node_t *else_if = (luna_if_node_t *) val->value.as_pointer;
    printf("\n");
    INDENT;
    printf("(else if ");
    visit((luna_node_t *) else_if->expr);
    ++indents;
    printf("\n");
    visit((luna_node_t *) else_if->block);
    --indents;
    printf(")");
  });

  // else
  if (node->else_block) {
    printf("\n");
    INDENT;
    printf("(else\n");
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
    .visit_hash = visit_hash,
    .visit_array = visit_array,
    .visit_while = visit_while,
    .visit_block = visit_block,
    .visit_decl = visit_decl,
    .visit_float = visit_float,
    .visit_string = visit_string,
    .visit_return = visit_return,
    .visit_function = visit_function,
    .visit_unary_op = visit_unary_op,
    .visit_binary_op = visit_binary_op
  };

  luna_visit(&visitor, node);

  printf("\n");
}
