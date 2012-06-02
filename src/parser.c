
//
// parser.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "prettyprint.h"
#include "parser.h"
#include "vec.h"
#include "token.h"

// TODO: test contextual errors

// -DEBUG_PARSER

#ifdef EBUG_PARSER
#define debug(name) \
  fprintf(stderr, "\n\e[90m%s\e[0m\n", name); \
  luna_token_inspect(&self->lex->tok);
#else
#define debug(name)
#endif

/*
 * Accept a token, advancing the lexer.
 */

#ifdef EBUG_PARSER
#define accept(t) \
  (peek->type == LUNA_TOKEN_##t \
    ? (fprintf(stderr, "\e[90maccepted \e[33m%s\e[0m\n", #t), \
      (self->lb = *self->la, self->la = NULL, &self->lb)) \
    : 0)
#else
#define accept(t) \
  (peek->type == LUNA_TOKEN_##t \
    ? (self->lb = *self->la, self->la = NULL, &self->lb) \
    : 0)
#endif

/*
 * Return the next token.
 */

#define advance (luna_scan(self->lex), &self->lex->tok)

/*
 * Previous token look-behind.
 */

#define prev (&self->lb)

/*
 * Return the next token, previously peeked token.
 */

#define next \
  (self->la \
    ? (self->tmp = self->la, self->la = NULL, self->tmp) \
    : advance)

/*
 * Single token look-ahead.
 */

#define peek (self->la ? self->la : (self->la = advance))

/*
 * Check if the next token is `t`.
 */

#define is(t) (peek->type == LUNA_TOKEN_##t)

/*
 * Set error context `str`, used in error reporting.
 */

#define context(str) (self->ctx = str)

/*
 * Set error `str` when not previously set.
 */

#define error(str) \
  ((self->err = self->err \
    ? self->err \
    : str), NULL)

// forward declarations

static luna_block_node_t *block(luna_parser_t *self);
static luna_node_t *expr(luna_parser_t *self);
static luna_node_t *call_expr(luna_parser_t *self);
static luna_node_t *not_expr(luna_parser_t *self);

/*
 * Initialize with the given lexer.
 */

void
luna_parser_init(luna_parser_t *self, luna_lexer_t *lex) {
  self->lex = lex;
  self->la = NULL;
  self->ctx = NULL;
  self->err = NULL;
}

/*
 * newline*
 */

static void
whitespace(luna_parser_t *self) {
  while (accept(NEWLINE)) ;
}

/*
 * '(' expr ')'
 */

static luna_node_t *
paren_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("paren_expr");
  if (!accept(LPAREN)) return NULL;
  if (!(node = expr(self))) return NULL;
  if (!accept(RPAREN)) return error("expression missing closing ')'");
  return node;
}

/*
 *   expr ','?
 * | expr ',' arg_list
 */

int
arg_list(luna_parser_t *self, luna_array_node_t *arr, luna_token delim) {
  // trailing ','
  if (delim == peek->type) return 1;

  // expr
  luna_node_t *val;
  if (!(val = expr(self))) return 0;
  luna_vec_push(arr->vals, luna_node(val));

  // ',' arg_list
  if (accept(COMMA)) {
    if (!arg_list(self, arr, delim)) return 0;
  }

  return 1;
}

/*
 * '[' arg_list? ']'
 */

static luna_node_t *
array_expr(luna_parser_t *self) {
  luna_array_node_t *node = luna_array_node_new();
  debug("array_expr");

  if (!accept(LBRACK)) return NULL;
  context("array");
  if (!arg_list(self, node, LUNA_TOKEN_RBRACK)) return NULL;
  if (!accept(RBRACK)) return error("array missing closing ']'");
  return (luna_node_t *) node;
}

/*
 *   id ':' expr
 * | id ':' expr ',' hash_pairs
 */

int
hash_pairs(luna_parser_t *self, luna_hash_node_t *hash, luna_token delim) {
  // trailing ','
  if (delim == peek->type) return 1;

  // id
  if (!is(ID)) return error("hash pair key expected"), 0;
  char *id = next->value.as_string;

  // :
  if (!accept(COLON)) return error("hash pair ':' missing"), 0;

  // expr
  luna_node_t *val;
  if (!(val = expr(self))) return 0;
  luna_hash_set(hash->vals, id, luna_node(val));

  // ',' hash_pairs
  if (accept(COMMA)) {
    if (!hash_pairs(self, hash, delim)) return 0;
  }

  return 1;
}

/*
 * '{' hash_pairs? '}'
 */

static luna_node_t *
hash_expr(luna_parser_t *self) {
  luna_hash_node_t *node = luna_hash_node_new();
  debug("hash_expr");

  if (!accept(LBRACE)) return NULL;
  context("hash");
  if (!hash_pairs(self, node, LUNA_TOKEN_RBRACE)) return NULL;
  if (!accept(RBRACE)) return error("hash missing closing '}'");
  return (luna_node_t *) node;
}

/*
 *   id
 * | int
 * | float
 * | string
 * | array
 * | hash
 * | paren_expr
 */

static luna_node_t *
primary_expr(luna_parser_t *self) {
  debug("primary_expr");
  switch (peek->type) {
    case LUNA_TOKEN_ID:
      return (luna_node_t *) luna_id_node_new(next->value.as_string);
    case LUNA_TOKEN_INT:
      return (luna_node_t *) luna_int_node_new(next->value.as_int);
    case LUNA_TOKEN_FLOAT:
      return (luna_node_t *) luna_float_node_new(next->value.as_float);
    case LUNA_TOKEN_STRING:
      return (luna_node_t *) luna_string_node_new(next->value.as_string);
    case LUNA_TOKEN_LBRACK:
      return array_expr(self);
    case LUNA_TOKEN_LBRACE:
      return hash_expr(self);
  }
  return paren_expr(self);
}

/*
 *   call_expr
 * | call_expr '**' call_expr
 */

static luna_node_t *
pow_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("pow_expr");
  if (!(node = call_expr(self))) return NULL;
  if (accept(OP_POW)) {
    context("** operation");
    if (right = call_expr(self)) {
      return (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_POW, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 *   pow_expr
 * | pow_expr '++'
 * | pow_expr '--'
 */

static luna_node_t *
postfix_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("postfix_expr");
  if (!(node = pow_expr(self))) return NULL;
  if (accept(OP_INCR) || accept(OP_DECR)) {
    return (luna_node_t *) luna_unary_op_node_new(prev->type, node, 1);
  }
  return node;
}

/*
 *   '++' unary_expr
 * | '--' unary_expr
 * | '~' unary_expr
 * | '+' unary_expr
 * | '-' unary_expr
 * | '!' unary_expr
 * | primary_expr
 */

static luna_node_t *
unary_expr(luna_parser_t *self) {
  debug("unary_expr");
  if (accept(OP_INCR)
    || accept(OP_DECR)
    || accept(OP_BIT_NOT)
    || accept(OP_PLUS)
    || accept(OP_MINUS)
    || accept(OP_NOT)) {
    return (luna_node_t *) luna_unary_op_node_new(prev->type, unary_expr(self), 0);
  }
  return postfix_expr(self);
}

/*
 * unary_expr (('* | '/' | '%') unary_expr)*
 */

static luna_node_t *
multiplicative_expr(luna_parser_t *self) {
  luna_token op;
  luna_node_t *node, *right;
  debug("multiplicative_expr");
  if (!(node = unary_expr(self))) return NULL;
  while (accept(OP_MUL) || accept(OP_DIV) || accept(OP_MOD)) {
    op = prev->type;
    context("multiplicative operation");
    if (right = unary_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(op, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * multiplicative_expr (('+ | '-') multiplicative_expr)*
 */

static luna_node_t *
additive_expr(luna_parser_t *self) {
  luna_token op;
  luna_node_t *node, *right;
  debug("additive_expr");
  if (!(node = multiplicative_expr(self))) return NULL;
  while (accept(OP_PLUS) || accept(OP_MINUS)) {
    op = prev->type;
    context("additive operation");
    if (right = multiplicative_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(op, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * additive_expr (('<<' | '>>') additive_expr)*
 */

static luna_node_t *
shift_expr(luna_parser_t *self) {
  luna_token op;
  luna_node_t *node, *right;
  debug("shift_expr");
  if (!(node = additive_expr(self))) return NULL;
  while (accept(OP_BIT_SHL) || accept(OP_BIT_SHR)) {
    op = prev->type;
    context("shift operation");
    if (right = additive_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(op, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * shift_expr (('<' | '<=' | '>' | '>=') shift_expr)*
 */

static luna_node_t *
relational_expr(luna_parser_t *self) {
  luna_token op;
  luna_node_t *node, *right;
  debug("relational_expr");
  if (!(node = shift_expr(self))) return NULL;
  while (accept(OP_LT) || accept(OP_LTE) || accept(OP_GT) || accept(OP_GTE)) {
    op = prev->type;
    context("relational operation");
    if (right = shift_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(op, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * relational_expr (('==' | '!=') relational_expr)*
 */

static luna_node_t *
equality_expr(luna_parser_t *self) {
  luna_token op;
  luna_node_t *node, *right;
  debug("equality_expr");
  if (!(node = relational_expr(self))) return NULL;
  while (accept(OP_EQ) || accept(OP_NEQ)) {
    op = prev->type;
    context("equality operation");
    if (right = relational_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(op, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * equality_expr ('&' equality_expr)*
 */

static luna_node_t *
bitwise_and_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("bitwise_and_expr");
  if (!(node = equality_expr(self))) return NULL;
  while (accept(OP_BIT_AND)) {
    context("& operation");
    if (right = equality_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_BIT_AND, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * bitwise_and_expr ('^' bitwise_and_expr)*
 */

static luna_node_t *
bitwise_xor_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("bitwise_xor_expr");
  if (!(node = bitwise_and_expr(self))) return NULL;
  while (accept(OP_BIT_XOR)) {
    context("^ operation");
    if (right = bitwise_and_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_BIT_XOR, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * bitwise_xor_expr ('|' bitwise_xor_expr)*
 */

static luna_node_t *
bitwise_or_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("bitwise_or_expr");
  if (!(node = bitwise_xor_expr(self))) return NULL;
  while (accept(OP_BIT_OR)) {
    context("| operation");
    if (right = bitwise_xor_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_BIT_OR, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * bitwise_or_expr ('&&' bitwise_or_expr)*
 */

static luna_node_t *
logical_and_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("logical_and_expr");
  if (!(node = bitwise_or_expr(self))) return NULL;
  while (accept(OP_AND)) {
    context("&& operation");
    if (right = bitwise_or_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_AND, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * logical_and_expr ('||' logical_and_expr)*
 */

static luna_node_t *
logical_or_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("logical_or_expr");
  if (!(node = logical_and_expr(self))) return NULL;
  while (accept(OP_OR)) {
    context("|| operation");
    if (right = logical_and_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_OR, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * (id ('=' expr)? (',' id ('=' expr)?)*)
 */

static luna_vec_t *
function_params(luna_parser_t *self) {
  luna_vec_t *params = luna_vec_new();
  debug("params");
  context("function params");
  if (!is(ID)) return params;

  do {
    // id
    if (!accept(ID)) return error("missing identifier");

    // ('=' expr)?
    luna_object_t *param;
    if (accept(OP_ASSIGN)) {
      luna_node_t *val = expr(self);
      if (!val) return NULL;
      param = luna_node((luna_node_t *) luna_param_node_new(prev->value.as_string, val));
    } else {
      param = luna_node((luna_node_t *) luna_param_node_new(prev->value.as_string, NULL));
    }
    luna_vec_push(params, param);

  } while (accept(COMMA));

  return params;
}

/*
 *   ':' params? block
 * | ':' params? '|' expr
 */

static luna_node_t *
function_expr(luna_parser_t *self) {
  luna_block_node_t *body;
  luna_vec_t *params;
  debug("function_expr");

  // ':'
  if (accept(COLON)) {
    // params?
    if (!(params = function_params(self))) return NULL;
    context("function");

    // '|' expr
    if (accept(OP_BIT_OR)) {
      luna_node_t *node;
      if (!(node = expr(self))) return NULL;
      return (luna_node_t *) luna_function_node_new_from_expr(node, params);
    }

    // block
    if (body = block(self)) {
      return (luna_node_t *) luna_function_node_new(body, params);
    }
  }

  return NULL;
}

/*
 * (primary_expr | function_expr) call_expr*
 */

static luna_node_t *
slot_access_expr(luna_parser_t * self) {
  luna_node_t *node;
  debug("slot_access_expr");

  // (primary_expr | function_expr)
  node = primary_expr(self);
  if (!node) node = function_expr(self);
  if (!node) return NULL;

  // id*
  while (is(ID)) {
    luna_node_t *right = call_expr(self);
    if (!right) return NULL;
    node = (luna_node_t *) luna_slot_node_new(node, right);
  }

  return node;
}

/*
 * (expr (',' expr)*)
 */

static luna_vec_t *
call_args(luna_parser_t *self) {
  luna_node_t *node;
  luna_vec_t *args = luna_vec_new();
  debug("args");
  context("function arguments");
  do {
    if (node = expr(self)) {
      luna_vec_push(args, luna_node(node));
    } else {
      return NULL;
    }
  } while (accept(COMMA));
  return args;
}

/*
 *   slot_access_expr '(' args? ')' function_expr?
 * | slot_access_expr function_expr?
 */

static luna_node_t *
call_expr(luna_parser_t *self) {
  luna_node_t *node;
  luna_call_node_t *call = NULL;
  debug("call_expr");

  // slot_access_expr
  if (!(node = slot_access_expr(self))) return NULL;

  // '('
  if (accept(LPAREN)) {
    context("function call");
    call = luna_call_node_new(node);

    // args? ')'
    if (!accept(RPAREN)) {
      call->args = call_args(self);
      if (!accept(RPAREN)) return error("missing closing ')'");
    }

    node = (luna_node_t *) call;
  }

  // function_expr?
  if (is(COLON)) {
    if (!call) call = luna_call_node_new(node);
    if (!call->args) call->args = luna_vec_new();
    luna_node_t *fn = function_expr(self);
    if (!fn) return NULL;
    luna_vec_push(call->args, luna_node(fn));
    node = (luna_node_t *) call;
  }

  return node;
}

/*
 *   logical_or_expr
 * | call_expr '=' not_expr
 * | call_expr '+=' not_expr
 * | call_expr '-=' not_expr
 * | call_expr '/=' not_expr
 * | call_expr '*=' not_expr
 * | call_expr '||=' not_expr
 */

static luna_node_t *
assignment_expr(luna_parser_t *self) {
  luna_token op;
  luna_node_t *node, *right;
  debug("assignment_expr");
  if (!(node = logical_or_expr(self))) return NULL;

  if ( accept(OP_ASSIGN)
    || accept(OP_PLUS_ASSIGN)
    || accept(OP_MINUS_ASSIGN)
    || accept(OP_DIV_ASSIGN)
    || accept(OP_MUL_ASSIGN)
    || accept(OP_OR_ASSIGN)
    || accept(OP_AND_ASSIGN)) {
    op = prev->type;
    context("assignment");
    if (!(right = not_expr(self))) return NULL;
    return (luna_node_t *) luna_binary_op_node_new(op, node, right);
  }

  return node;
}

/*
 *   'not' not_expr
 * | assignment_expr
 */

static luna_node_t *
not_expr(luna_parser_t *self) {
  debug("not_expr");
  if (accept(OP_LNOT)) {
    luna_node_t *expr;
    if (!(expr = not_expr(self))) return NULL;
    return (luna_node_t *) luna_unary_op_node_new(prev->type, expr, 0);
  }
  return assignment_expr(self);
}

/*
 *   not_expr 'if' expr
 * | not_expr 'unless' expr
 * | not_expr 'while' expr
 * | not_expr 'until' expr
 * | not_expr
 */

static luna_node_t *
if_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("if_expr");
  if (!(node = not_expr(self))) return NULL;

  // ('if' | 'unless') expr
  if (accept(IF) || accept(UNLESS)) {
    int negate = LUNA_TOKEN_UNLESS == prev->type;
    context(negate ? "unless operation" : "if operation");
    luna_node_t *cond;
    if (!(cond = expr(self))) return NULL;
    luna_block_node_t *block = luna_block_node_new();
    luna_vec_push(block->stmts, luna_node(node));
    node = (luna_node_t *) luna_if_node_new(negate, cond, block);
  }

  // ('while' | 'until') expr
  if (accept(WHILE) || accept(UNTIL)) {
    int negate = LUNA_TOKEN_UNTIL == prev->type;
     context(negate ? "until operation" : "while operation");
    luna_node_t *cond;
    if (!(cond = expr(self))) return NULL;
    luna_block_node_t *block = luna_block_node_new();
    luna_vec_push(block->stmts, luna_node(node));
    node = (luna_node_t *) luna_while_node_new(negate, cond, block);
  }

  return node;
}

/*
 * if_expr
 */

static luna_node_t *
expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("expr");
  if (!(node = if_expr(self))) return NULL;
  return node;
}

/*
 * expr (newline)
 */

static luna_node_t *
expr_stmt(luna_parser_t *self) {
  luna_node_t *node;
  debug("expr_stmt");
  
  if (!(node = expr(self))) return NULL;

  if (!(accept(NEWLINE) || is(OUTDENT) || is(EOS))) {
    return error("missing newline");
  }

  return node;
}

/*
 *  ('if' | 'unless') expr block
 *  ('else' 'if' block)*
 *  ('else' block)?
 */

static luna_node_t *
if_stmt(luna_parser_t *self) {
  luna_node_t *cond;
  luna_block_node_t *body;
  debug("if_stmt");

  // ('if' | 'unless')
  if (!(accept(IF) || accept(UNLESS))) return NULL;
  int negate = LUNA_TOKEN_UNLESS == prev->type;

  // expr
  context("if statement condition");
  if (!(cond = expr(self))) return NULL;

  // block
  context("if statement");
  if (!(body = block(self))) return NULL;

  luna_if_node_t *node = luna_if_node_new(negate, cond, body);

  // 'else'
  loop:
  if (accept(ELSE)) {
    luna_block_node_t *body;
    
    // ('else' 'if' block)*
    if (accept(IF)) {
      context("else if statement condition");
      if (!(cond = expr(self))) return NULL;
      context("else if statement");
      if (!(body = block(self))) return NULL;
      luna_vec_push(node->else_ifs, luna_node((luna_node_t *) luna_if_node_new(0, cond, body)));
      goto loop;
    // 'else'
    } else {
      context("else statement");
      if (!(body = block(self))) return NULL;
      node->else_block = body;
    }

  }

  return (luna_node_t *) node;
}

/*
 * ('while' | 'until') expr block
 */

static luna_node_t *
while_stmt(luna_parser_t *self) {
  luna_node_t *cond;
  luna_block_node_t *body;
  debug("while_stmt");

  // ('until' | 'while')
  if (!(accept(UNTIL) || accept(WHILE))) return NULL;
  int negate = LUNA_TOKEN_UNTIL == prev->type;
  context("while statement condition");

  // expr
  if (!(cond = expr(self))) return NULL;
  context("while statement");

  // block
  if (!(body = block(self))) return NULL;

  return (luna_node_t *) luna_while_node_new(negate, cond, body);
}

/*
 *   'return' expr
 * | 'return'
 */

static luna_node_t *
return_stmt(luna_parser_t *self) {
  debug("return");
  context("return statement");

  // 'return'
  if (!accept(RETURN)) return NULL;
  if (is(NEWLINE)) {
    return (luna_node_t *) luna_return_node_new(NULL);
  }

  // 'return' expr
  luna_node_t *node;
  if (!(node = expr(self))) return NULL;
  return (luna_node_t *) luna_return_node_new(node);
}

/*
 *   if_stmt
 * | while_stmt
 * | return_stmt
 * | expr_stmt
 */

static luna_node_t *
stmt(luna_parser_t *self) {
  debug("stmt");
  context("statement");
  if (is(IF) || is(UNLESS)) return if_stmt(self);
  if (is(WHILE) || is(UNTIL)) return while_stmt(self);
  if (is(RETURN)) return return_stmt(self);
  return expr_stmt(self);
}

/*
 * INDENT ws (stmt ws)+ OUTDENT
 */

static luna_block_node_t *
block(luna_parser_t *self) {
  debug("block");
  luna_node_t *node;
  luna_block_node_t *block = luna_block_node_new();
  if (!accept(INDENT)) return error("block expected");
  whitespace(self);
  do {
    if (node = stmt(self)) {
      luna_vec_push(block->stmts, luna_node(node));
    } else {
      return NULL;
    }
    whitespace(self);
  } while (!accept(OUTDENT));
  return block;
}

/*
 * ws (stmt ws)*
 */

static luna_block_node_t *
program(luna_parser_t *self) {
  debug("program");
  luna_node_t *node;
  whitespace(self);
  luna_block_node_t *block = luna_block_node_new();
  while (!accept(EOS)) {
    if (node = stmt(self)) {
      luna_vec_push(block->stmts, luna_node(node));
    } else {
      return NULL;
    }
    whitespace(self);
  }
  return block;
}

/*
 * Parse input.
 */

luna_block_node_t *
luna_parse(luna_parser_t *self) {
  return program(self);
}

