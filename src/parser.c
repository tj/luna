
//
// parser.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include <stdbool.h>
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
 * Advance if the current token is `t`
 */

#ifdef EBUG_PARSER
#define accept(t) \
  (is(t)) \
    ? (fprintf(stderr, "\e[90maccepted \e[33m%s\e[0m\n", #t), next, 1) \
    : 0)
#else
#define accept(t) (is(t) && next)
#endif

/*
 * Consume a token from the lexer
 */

#define next (self->tok = (luna_scan(self->lex), &self->lex->tok))

/*
 * Check if the current token is `t`.
 */

#define is(t) (self->tok->type == LUNA_TOKEN_##t)

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
static luna_node_t *call_expr(luna_parser_t *self, luna_node_t *left);
static luna_node_t *not_expr(luna_parser_t *self);

/*
 * Initialize with the given lexer.
 */

void
luna_parser_init(luna_parser_t *self, luna_lexer_t *lex) {
  self->lex = lex;
  self->tok = NULL;
  self->ctx = NULL;
  self->err = NULL;
  self->in_args = 0;
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
  if (delim == self->tok->type) return 1;

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
  if (delim == self->tok->type) return 1;

  luna_hash_pair_node_t *pair = luna_hash_pair_node_new();
  if (!(pair->key = expr(self))) return 0;

  // :
  if (!accept(COLON)) return error("hash pair ':' missing"), 0;

  // expr
  if (!(pair->val = expr(self))) return 0;

  luna_vec_push(hash->pairs, luna_node((luna_node_t *)pair));

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
 * id
 */

static luna_node_t *
type_expr(luna_parser_t *self) {
  debug("type_expr");
  if (!is(ID)) return NULL;

  luna_node_t *ret = (luna_node_t *) luna_id_node_new(self->tok->value.as_string);
  next;

  return ret;
}

/*
 * id (',' id)* ':' type_expr
 */

static luna_node_t *
decl_expr(luna_parser_t *self, bool need_type) {
  debug("decl_expr");
  context("declaration");

  if (!is(ID)) return NULL;

  luna_vec_t *vec = luna_vec_new();
  while (is(ID)) {
    // id
    luna_node_t *id = (luna_node_t *) luna_id_node_new(self->tok->value.as_string);
    luna_vec_push(vec, luna_node(id));
    next;

    // ','
    if (!accept(COMMA)) {
      break;
    }
  }

  // ':'
  if (!accept(COLON)) {
    if (need_type) {
      return error("expecting type");
    } else {
      return (luna_node_t *) luna_decl_node_new(vec, NULL);
    }
  }

  luna_node_t *type = type_expr(self);
  if (!type) {
    return error("expecting type");
  }

  return (luna_node_t *) luna_decl_node_new(vec, type);
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
  luna_node_t *ret = NULL;
  luna_token_t *tok = self->tok;
  switch (tok->type) {
    case LUNA_TOKEN_ID:
      ret = (luna_node_t *) luna_id_node_new(tok->value.as_string);
      break;
    case LUNA_TOKEN_INT:
      ret = (luna_node_t *) luna_int_node_new(tok->value.as_int);
      break;
    case LUNA_TOKEN_FLOAT:
      ret = (luna_node_t *) luna_float_node_new(tok->value.as_float);
      break;
    case LUNA_TOKEN_STRING:
      ret = (luna_node_t *) luna_string_node_new(tok->value.as_string);
      break;
    case LUNA_TOKEN_LBRACK:
      return array_expr(self);
    case LUNA_TOKEN_LBRACE:
      return hash_expr(self);
  }
  if (ret) {
    next;
    return ret;
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
  if (!(node = call_expr(self, NULL))) return NULL;
  if (accept(OP_POW)) {
    context("** operation");
    if (right = call_expr(self, NULL)) {
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
  if (is(OP_INCR) || is(OP_DECR)) {
    node = (luna_node_t *) luna_unary_op_node_new(self->tok->type, node, 1);
    next;
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
  if (is(OP_INCR)
    || is(OP_DECR)
    || is(OP_BIT_NOT)
    || is(OP_PLUS)
    || is(OP_MINUS)
    || is(OP_NOT)) {
    int op = self->tok->type;
    next;
    return (luna_node_t *) luna_unary_op_node_new(op, unary_expr(self), 0);
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
  while (is(OP_MUL) || is(OP_DIV) || is(OP_MOD)) {
    op = self->tok->type;
    next;
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
  while (is(OP_PLUS) || is(OP_MINUS)) {
    op = self->tok->type;
    next;
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
  while (is(OP_BIT_SHL) || is(OP_BIT_SHR)) {
    op = self->tok->type;
    next;
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
  while (is(OP_LT) || is(OP_LTE) || is(OP_GT) || is(OP_GTE)) {
    op = self->tok->type;
    next;
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
  while (is(OP_EQ) || is(OP_NEQ)) {
    op = self->tok->type;
    next;
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
 * equality_expr ('and' equality_expr)*
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
bitswise_or_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("bitswise_or_expr");
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
 * bitswise_or_expr ('&&' bitswise_or_expr)*
 */

static luna_node_t *
logical_and_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("logical_and_expr");
  if (!(node = bitswise_or_expr(self))) return NULL;
  while (accept(OP_AND)) {
    context("&& operation");
    if (right = bitswise_or_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_AND, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }
  return node;
}

/*
 * logical_and_expr ('||' logical_and_expr)* '&'?
 */

static luna_node_t *
logical_or_expr(luna_parser_t *self) {
  luna_node_t *node, *right;
  debug("logical_or_expr");
  if (!(node = logical_and_expr(self))) return NULL;

  // '||'
  while (accept(OP_OR)) {
    context("|| operation");
    if (right = logical_and_expr(self)) {
      node = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_OR, node, right);
    } else {
      return error("missing right-hand expression");
    }
  }

  // '&'
  if (accept(OP_FORK)) {
    luna_id_node_t *id = luna_id_node_new("fork");
    luna_call_node_t *call = luna_call_node_new((luna_node_t *) id);
    luna_vec_push(call->args->vec, luna_node(node));
    node = (luna_node_t *) call;
  }

  return node;
}

/*
 * (decl_expr ('=' expr)? (',' decl_expr ('=' expr)?)*)
 */

static luna_vec_t *
function_params(luna_parser_t *self) {
  luna_vec_t *params = luna_vec_new();
  debug("params");
  context("function params");

  if (!is(ID)) return params;

  do {
    luna_node_t *decl = decl_expr(self, false);
    if (!decl) return NULL;

    context("function param");

    // ('=' expr)?
    luna_object_t *param;
    if (accept(OP_ASSIGN)) {
      luna_node_t *val = expr(self);
      if (!val) return NULL;
      param = luna_node((luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_ASSIGN, decl, val));
    } else {
      // if there isn't a value we need a type
      if (!decl->type) {
        return error("expecting type");
      }
      param = luna_node((luna_node_t *) decl);
    }

    luna_vec_push(params, param);
  } while (accept(COMMA));

  return params;
}

/*
 * ':' params? block
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

    // block
    if (body = block(self)) {
      //return (luna_node_t *) luna_function_node_new(body, params);
    }
  }

  return NULL;
}

/*
 *   primary_expr
 * | primary_expr '[' expr ']'
 * | primary_expr '.' id
 * | primary_expr '.' call_expr
 */

static luna_node_t *
slot_access_expr(luna_parser_t *self, luna_node_t *left) { 
  debug("slot_access_expr");

  // primary_expr
  if (!left) {
    if (!(left = primary_expr(self))) return NULL;
  }

  // subscript
  if (accept(LBRACK)) {
    luna_node_t *right;

    if (!(right = expr(self))) {
      return error("missing index in subscript");
    }
    context("subscript");
    if (!accept(RBRACK)) return error("missing closing ']'");
    left = (luna_node_t *) luna_subscript_node_new(left, right);
    return call_expr(self, left);
  }

  // slot
  while (accept(OP_DOT)) {
    context("slot access");
    if (!is(ID)) return error("expecting identifier");
    luna_node_t *id = (luna_node_t *)luna_id_node_new(self->tok->value.as_string);
    next;

    if (is(LPAREN)) {
      luna_call_node_t *call;
      luna_vec_t *args_vec;
      luna_object_t *prev = NULL;

      call = (luna_call_node_t *) call_expr(self, id);
      if (luna_vec_length(call->args->vec) > 0) {

        // re-organize call arguments (issue #47)
        args_vec = luna_vec_new();
        luna_vec_each(call->args->vec, {
          if (i == 0) {
            luna_vec_push(args_vec, luna_node(left));
          } else {
            luna_vec_push(args_vec, prev);
          }

          prev = val;
        });
      } else {
        prev = luna_node(left);
        args_vec = call->args->vec;
      }

      // add last argument
      luna_vec_push(args_vec, prev);

      // TODO: free the old arguments vector
      call->args->vec = args_vec;
      left = (luna_node_t *)call;

    } else {
      left = (luna_node_t *) luna_slot_node_new(left, id);
    }

    left = call_expr(self, left);
  }

  return left;
}

/*
 * (expr (',' expr)*)
 */

luna_args_node_t *
call_args(luna_parser_t *self) {
  luna_node_t *node;
  luna_args_node_t *args = luna_args_node_new();

  self->in_args++;

  debug("args");
  do {
    if (node = expr(self)) {
      // TODO: assert string or id
      if (accept(COLON)) {
        luna_node_t *val = expr(self);
        const char *str = ((luna_id_node_t *) node)->val;
        luna_hash_set(args->hash, (char *) str, luna_node(val));
      }
      luna_vec_push(args->vec, luna_node(node));
    } else {
      return NULL;
    }
  } while (accept(COMMA));

  self->in_args--;

  return args;
}

/*
 *   slot_access_expr '(' args? ')'
 * | slot_access_expr
 */

static luna_node_t *
call_expr(luna_parser_t *self, luna_node_t *left) {
  luna_node_t *right;
  luna_node_t *prev = left;
  luna_call_node_t *call = NULL;
  debug("call_expr");

  // slot_access_expr
  if (!left) {
    if (!(left = slot_access_expr(self, NULL))) return NULL;
  }

  // '('
  if (accept(LPAREN)) {
    context("function call");
    call = luna_call_node_new(left);

    // args? ')'
    if (!is(RPAREN)) {
      call->args = call_args(self);
      if (!is(RPAREN)) return error("missing closing ')'");
    }
    next;
    left = (luna_node_t *) call;
  }

  if (is(OP_DOT) && prev) {
    // stop here if the there was a previous left-hand expression
    // and the current token is '.' because we're
    // probably inside the loop in slot_access_expr
    return left;
  } else if (is(LPAREN)) {
    return call_expr(self, left);
  } else {
    return slot_access_expr(self, left);
  }
}

/*
 * 'let' decl_expr ('=' expr)? (',' decl_expr ('=' expr)?)*
 */
 
static luna_node_t *
let_expr(luna_parser_t *self) {
  // let already consumed
  luna_vec_t *vec = luna_vec_new();

  do {
    luna_node_t *decl = decl_expr(self, false);
    luna_node_t *val = NULL;

    context("let expression");
    if (!decl) {
      return error("expecting declaration");
    }

    // '='
    if (accept(OP_ASSIGN)) {
      val = expr(self);
      if (!val) return error("expecting declaration initializer");
    }

    luna_node_t *bin = (luna_node_t *) luna_binary_op_node_new(LUNA_TOKEN_OP_ASSIGN, decl, val);
    luna_vec_push(vec, luna_node(bin));
  } while (accept(COMMA));

  return (luna_node_t *) luna_let_node_new(vec);
}

/*
 *   logical_or_expr
 * | let_expr
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

  // let?
  if (accept(LET)) return let_expr(self);

  debug("assignment_expr");
  if (!(node = logical_or_expr(self))) return NULL;

  // =
  if (is(OP_ASSIGN)) {
    op = self->tok->type;
    next;
    context("assignment");
    if (!(right = not_expr(self))) return NULL;
    luna_binary_op_node_t *ret = luna_binary_op_node_new(op, node, right);
    return (luna_node_t *) ret;
  }

  // compound
  if ( is(OP_PLUS_ASSIGN)
    || is(OP_MINUS_ASSIGN)
    || is(OP_DIV_ASSIGN)
    || is(OP_MUL_ASSIGN)
    || is(OP_OR_ASSIGN)
    || is(OP_AND_ASSIGN)) {
    op = self->tok->type;
    next;
    context("compound assignment");
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
    return (luna_node_t *) luna_unary_op_node_new(LUNA_TOKEN_OP_LNOT, expr, 0);
  }
  return assignment_expr(self);
}

/*
 *  not_expr
 */

static luna_node_t *
expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("expr");
  if (!(node = not_expr(self))) return NULL;
  return node;
}

/*
 * 'type' id decl_expr* end
 */

static luna_node_t *
type_stmt(luna_parser_t *self) {
  debug("type_stmt");
  context("type statement");
  luna_type_node_t *type;

  // 'type'
  if (!accept(TYPE)) return NULL;

  // id
  if (!is(ID)) return error("missing type name");
  const char *name = self->tok->value.as_string;
  type = luna_type_node_new(name);
  next;

  // semicolon might have been inserted here
  accept(SEMICOLON);

  // type fields
  do {
    luna_node_t *decl = decl_expr(self, true);
    if (!decl) return error("expecting field");

    // semicolon might have been inserted here
    accept(SEMICOLON);

    luna_vec_push(type->fields, luna_node(decl));
  } while (!accept(END));

  return (luna_node_t *) type;
}

/*
 * 'def' id '(' args? ')' (':' type_expr)? block
 */

static luna_node_t *
function_stmt(luna_parser_t *self) {
  luna_block_node_t *body;
  luna_vec_t *params;
  luna_node_t *type = NULL;
  debug("function_stmt");
  context("function statement");

  // 'def'
  if (!accept(DEF)) return NULL;

  // id
  if (!is(ID)) return error("missing function name");
  const char *name = self->tok->value.as_string;
  next;

  // '('
  if (accept(LPAREN)) {
    // params?
    if (!(params = function_params(self))) return NULL;

    // ')'
    context("function");
    if (!accept(RPAREN)) return error("missing closing ')'");
  } else {
    params = luna_vec_new();
  }

  context("function");

  // (':' type_expr)?
  if (accept(COLON)) {
    type = type_expr(self);
    if (!type) return error("missing type after ':'");

    // semicolon might have been inserted here
    accept(SEMICOLON);
  }

  // block
  if (body = block(self)) {
    return (luna_node_t *) luna_function_node_new(name, type, body, params);
  }

  return NULL;
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
  if (!(is(IF) || is(UNLESS))) return NULL;
  int negate = LUNA_TOKEN_UNLESS == self->tok->type;
  next;

  // expr
  context("if statement condition");
  if (!(cond = expr(self))) {
    return NULL;
  }

  // semicolon might have been inserted here
  accept(SEMICOLON);

  // block
  context("if statement");
  if (!(body = block(self))) {
    return NULL;
  }

  luna_if_node_t *node = luna_if_node_new(negate, cond, body);

  // 'else'
  loop:
  if (accept(ELSE)) {
    luna_block_node_t *body;

    // ('else' 'if' block)*
    if (accept(IF)) {
      context("else if statement condition");
      if (!(cond = expr(self))) return NULL;

      // semicolon might have been inserted here
      accept(SEMICOLON);

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
  if (!(is(UNTIL) || is(WHILE))) return NULL;
  int negate = LUNA_TOKEN_UNTIL == self->tok->type;
  context("while statement condition");
  next;

  // expr
  if (!(cond = expr(self))) return NULL;
  context("while statement");

  // semicolon might have been inserted here
  accept(SEMICOLON);

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

  // 'return' expr
  luna_node_t *node = NULL;

  if (!accept(SEMICOLON)) {
    if (!(node = expr(self))) return NULL;
  }
  return (luna_node_t *) luna_return_node_new(node);
}

/*
 *   if_stmt
 * | while_stmt
 * | return_stmt
 * | function_stmt
 * | type_stmt
 * | expr
 */

static luna_node_t *
stmt(luna_parser_t *self) {
  debug("stmt");
  context("statement");
  if (is(IF) || is(UNLESS)) return if_stmt(self);
  if (is(WHILE) || is(UNTIL)) return while_stmt(self);
  if (is(RETURN)) return return_stmt(self);
  if (is(DEF)) return function_stmt(self);
  if (is(TYPE)) return type_stmt(self);
  return expr(self);
}

/*
 * stmt* 'end'
 */

static luna_block_node_t *
block(luna_parser_t *self) {
  debug("block");
  luna_node_t *node;
  luna_block_node_t *block = luna_block_node_new();

  if (accept(END)) return block;

  do {
    if (!(node = stmt(self))) return NULL;
    accept(SEMICOLON);

    luna_vec_push(block->stmts, luna_node(node));
  } while (!accept(END) && !is(ELSE));

  return block;
}

/*
 * stmt*
 */

static luna_block_node_t *
program(luna_parser_t *self) {
  debug("program");
  luna_node_t *node;
  luna_block_node_t *block = luna_block_node_new();

  next;
  while (!is(EOS)) {
    if (node = stmt(self)) {
      accept(SEMICOLON);
      luna_vec_push(block->stmts, luna_node(node));
    } else {
      return NULL;
    }
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

