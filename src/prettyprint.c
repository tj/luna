
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

// print function

int 
(*print_func)(const char *format, ...);

void
luna_set_prettyprint_func(int (*func)(const char *format, ...)) {
    print_func = func;
}

// output indentation

#define INDENT for (int j = 0; j < indents; ++j) print_func("  ")

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
    if (i) print_func("\n");
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
    if (!indents) print_func("\n");
  });
}

/*
 * Visit int `node`.
 */

static void
visit_int(luna_visitor_t *self, luna_int_node_t *node) {
  print_func("(int %d)", node->val);
}

/*
 * Visit float `node`.
 */

static void
visit_float(luna_visitor_t *self, luna_float_node_t *node) {
  print_func("(float %f)", node->val);
}

/*
 * Visit id `node`.
 */

static void
visit_id(luna_visitor_t *self, luna_id_node_t *node) {
  print_func("(id %s)", node->val);
}

/*
 * Visit decl `node`.
 */

static void
visit_decl(luna_visitor_t *self, luna_decl_node_t *node) {
  print_func("(decl");
  indents++;
  luna_vec_each(node->vec, {
    print_func("\n");
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
  });

  if (node->type) {
    print_func("\n");
    INDENT;
    print_func(": ");
    visit(node->type);
  }

  print_func(")");
  indents--;
}

/*
 * Visit let `node`.
 */

static void
visit_let(luna_visitor_t *self, luna_let_node_t *node) {
  print_func("(let\n");
  indents++;
  INDENT;
  visit(node->decl);

  if (node->val) {
    print_func("\n");
    INDENT;
    print_func(" = ");
    visit(node->val);
  }
  print_func(")");
  indents--;
}

/*
 * Visit string `node`.
 */

static void
visit_string(luna_visitor_t *self, luna_string_node_t *node) {
  print_func("(string '%s')", inspect(node->val));
}

/*
 * Visit unary op `node`.
 */

static void
visit_unary_op(luna_visitor_t *self, luna_unary_op_node_t *node) {
  int c = node->postfix ? '@' : 0;
  print_func("(%c%s ", c, luna_token_type_string(node->op));
  visit(node->expr);
  print_func(")");
}

/*
 * Visit binary op `node`.
 */

static void
visit_binary_op(luna_visitor_t *self, luna_binary_op_node_t *node) {
  if (node->let) {
    print_func("(let %s ", luna_token_type_string(node->op));
  } else {
    print_func("(%s ", luna_token_type_string(node->op));
  }
  visit(node->left);
  print_func(" ");
  visit(node->right);
  print_func(")");
}

/*
 * Visit array `node`.
 */

static void
visit_array(luna_visitor_t *self, luna_array_node_t *node) {
  print_func("(array\n");
  ++indents;
  luna_vec_each(node->vals, {
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
    if (i != len - 1) print_func("\n");
  });
  --indents;
  print_func(")");
}

/*
 * Visit hash `node`.
 */

static void
visit_hash(luna_visitor_t *self, luna_hash_node_t *node) {
  print_func("(hash\n");
  ++indents;
  luna_vec_each(node->pairs, {
    INDENT;
    visit(((luna_hash_pair_node_t *)val->value.as_pointer)->key);
    print_func(": ");
    visit(((luna_hash_pair_node_t *)val->value.as_pointer)->val);
    print_func("\n");
  });
  --indents;
  print_func(")");
}

/*
 * Visit subscript `node`.
 */

static void
visit_subscript(luna_visitor_t *self, luna_subscript_node_t *node) {
  print_func("(subscript\n");
  ++indents;
  INDENT;
  visit(node->left);
  print_func("\n");
  INDENT;
  visit(node->right);
  --indents;
  print_func(")");
}

/*
 * Visit slot `node`.
 */

static void
visit_slot(luna_visitor_t *self, luna_slot_node_t *node) {
  print_func("(slot\n");
  ++indents;
  INDENT;
  visit(node->left);
  print_func("\n");
  INDENT;
  visit(node->right);
  --indents;
  print_func(")");
}

/*
 * Visit call `node`.
 */

static void
visit_call(luna_visitor_t *self, luna_call_node_t * node) {
  print_func("(call\n");
  ++indents;
  INDENT;
  visit((luna_node_t *) node->expr);
  if (luna_vec_length(node->args->vec)) {
    print_func("\n");
    INDENT;
    luna_vec_each(node->args->vec, {
      visit((luna_node_t *) val->value.as_pointer);
      if (i != len - 1) print_func(" ");
    });

    luna_hash_each(node->args->hash, {
      print_func(" %s: ", slot);
      visit((luna_node_t *) val->value.as_pointer);
    });
  }
  --indents;
  print_func(")");
}

/*
 * Visit function `node`.
 */

static void
visit_function(luna_visitor_t *self, luna_function_node_t * node) {
  print_func("(function %s -> %s", node->name, node->type ? node->type : "");
  ++indents;
  luna_vec_each(node->params, {
    print_func("\n");
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
  });
  --indents;
  print_func("\n");
  ++indents;
  visit((luna_node_t *) node->block);
  --indents;
  print_func(")");
}

/*
 * Visit `type` node.
 */

static void
visit_type(luna_visitor_t *self, luna_type_node_t *node) {
  print_func("(type %s", node->name);
  ++indents;
  luna_vec_each(node->fields, {
    print_func("\n");
    INDENT;
    visit((luna_node_t *) val->value.as_pointer);
  });
  --indents;
  print_func(")");
}

/*
 * Visit `while` node.
 */

static void
visit_while(luna_visitor_t *self, luna_while_node_t *node) {
  // while | until
  print_func("(%s ", node->negate ? "until" : "while");
  visit((luna_node_t *) node->expr);
  ++indents;
  print_func("\n");
  visit((luna_node_t *) node->block);
  --indents;
  print_func(")\n");
}

/*
 * Visit `return` node.
 */

static void
visit_return(luna_visitor_t *self, luna_return_node_t *node) {
  print_func("(return");
  if (node->expr) {
    ++indents;
    print_func("\n");
    INDENT;
    visit((luna_node_t *) node->expr);
    --indents;
  }
  print_func(")");
}

/*
 * Visit if `node`.
 */

static void
visit_if(luna_visitor_t *self, luna_if_node_t *node) {
  // if
  print_func("(%s ", node->negate ? "unless" : "if");
  visit((luna_node_t *) node->expr);
  ++indents;
  print_func("\n");
  visit((luna_node_t *) node->block);
  --indents;
  print_func(")");

  // else ifs
  luna_vec_each(node->else_ifs, {
    luna_if_node_t *else_if = (luna_if_node_t *) val->value.as_pointer;
    print_func("\n");
    INDENT;
    print_func("(else if ");
    visit((luna_node_t *) else_if->expr);
    ++indents;
    print_func("\n");
    visit((luna_node_t *) else_if->block);
    --indents;
    print_func(")");
  });

  // else
  if (node->else_block) {
    print_func("\n");
    INDENT;
    print_func("(else\n");
    ++indents;
    visit((luna_node_t *) node->else_block);
    --indents;
    print_func(")");
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
    .visit_let = visit_let,
    .visit_float = visit_float,
    .visit_string = visit_string,
    .visit_return = visit_return,
    .visit_function = visit_function,
    .visit_unary_op = visit_unary_op,
    .visit_binary_op = visit_binary_op,
    .visit_subscript = visit_subscript,
    .visit_type = visit_type
  };

  luna_visit(&visitor, node);

  print_func("\n");
}
