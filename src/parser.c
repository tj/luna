
//
// parser.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "prettyprint.h"
#include "parser.h"
#include "array.h"

// -DEBUG_PARSER

#ifdef EBUG_PARSER
#define debug(name) \
  fprintf(stderr, "\n\033[90m%s\033[0m\n", name); \
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
    ? (fprintf(stderr, "\033[90maccepted \033[33m%s\033[0m\n", #t), next) \
    : 0)
#else
#define accept(t) \
  (peek->type == LUNA_TOKEN_##t \
    ? next \
    : 0)
#endif

/*
 * Return the next token.
 */

#define next (luna_scan(self->lex), &self->lex->tok)

/*
 * Single token look-ahead.
 */

#define peek (self->la ? self->la : (self->la = next))

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

static luna_block_node_t * block(luna_parser_t *self);
static luna_node_t * expr(luna_parser_t *self);
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

// static int
// paren_expr(luna_parser_t *self) {
//   debug("paren_expr");
//   if (!accept(LPAREN)) return 0;
//   if (!expr(self)) return 0;
//   if (!accept(RPAREN)) return error("expression missing closing ')'");
//   return 1;
// }

/*
 *   id
 * | string
 * | int
 * | float
 * | paren_expr
 */

static luna_node_t *
primary_expr(luna_parser_t *self) {
  luna_token_t *tok;
  debug("primary_expr");

  // int
  if (tok = accept(INT)) {
    return (luna_node_t *) luna_int_node_new(tok->value.as_int);
  }

  // string
  if (tok = accept(STRING)) {
    return (luna_node_t *) luna_string_node_new(tok->value.as_string);
  }

  return NULL;
  // return accept(ID)
  //   || accept(STRING)
  //   || accept(FLOAT)
  //   || paren_expr(self);
  //   ;
}

/*
 *   call_expr
 * | call_expr '**' call_expr
 */

static luna_node_t *
pow_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("pow_expr");
  if (!(node = call_expr(self))) return NULL;
  // if (accept(OP_POW)) {
  //   context("** operation");
  //   if (!call_expr(self)) return error("missing right-hand expression");
  // }
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
  // if (accept(OP_INCR) || accept(OP_DECR)) ;
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
  // debug("unary_expr");
  // if (accept(OP_INCR)
  //   || accept(OP_DECR)
  //   || accept(OP_BIT_NOT)
  //   || accept(OP_PLUS)
  //   || accept(OP_MINUS)
  //   || accept(OP_NOT)) {
  //   return unary_expr(self);
  // }
  return postfix_expr(self);
}

/*
 * unary_expr (('* | '/' | '%') unary_expr)*
 */

static luna_node_t *
multiplicative_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("multiplicative_expr");
  if (!(node = unary_expr(self))) return NULL;
  // while (accept(OP_MULT) || accept(OP_DIV) || accept(OP_MOD)) {
  //   context("multiplicative operation");
  //   if (!unary_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * multiplicative_expr (('+ | '-') multiplicative_expr)*
 */

static luna_node_t *
concat_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("concat_expr");
  if (!(node = multiplicative_expr(self))) return NULL;
  // while (accept(OP_DOT)) {
  //   context("concatenation operation");
  //   if (!multiplicative_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * concat_expr (('+ | '-') concat_expr)*
 */

static luna_node_t *
additive_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("additive_expr");
  if (!(node = concat_expr(self))) return NULL;
  // while (accept(OP_PLUS) || accept(OP_MINUS)) {
  //   context("additive operation");
  //   if (!concat_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * additive_expr (('<<' | '>>') additive_expr)*
 */

static luna_node_t *
shift_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("shift_expr");
  if (!(node = additive_expr(self))) return NULL;
  // while (accept(OP_BIT_SHL) || accept(OP_BIT_SHR)) {
  //   context("shift operation");
  //   if (!additive_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * shift_expr (('<' | '<=' | '>' | '>=') shift_expr)*
 */

static luna_node_t *
relational_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("relational_expr");
  if (!(node = shift_expr(self))) return NULL;
  // while (accept(OP_LT) || accept(OP_LTE) || accept(OP_GT) || accept(OP_GTE)) {
  //   context("relational operation");
  //   if (!shift_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * relational_expr (('==' | '!=') relational_expr)*
 */

static luna_node_t *
equality_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("equality_expr");
  if (!(node = relational_expr(self))) return NULL;
  // while (accept(OP_EQ) || accept(OP_NEQ)) {
  //   context("equality operation");
  //   if (!relational_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * equality_expr ('&' equality_expr)*
 */

static luna_node_t *
bitwise_and_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("bitwise_and_expr");
  if (!(node = equality_expr(self))) return NULL;
  // while (accept(OP_BIT_AND)) {
  //   context("& operation");
  //   if (!equality_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * bitwise_and_expr ('^' bitwise_and_expr)*
 */

static luna_node_t *
bitwise_xor_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("bitwise_xor_expr");
  if (!(node = bitwise_and_expr(self))) return NULL;
  // while (accept(OP_BIT_XOR)) {
  //   context("^ operation");
  //   if (!bitwise_and_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * bitwise_xor_expr ('|' bitwise_xor_expr)*
 */

static luna_node_t *
bitwise_or_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("bitwise_or_expr");
  if (!(node = bitwise_xor_expr(self))) return NULL;
  // while (accept(OP_BIT_OR)) {
  //   context("| operation");
  //   if (!bitwise_xor_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * bitwise_or_expr ('&&' bitwise_or_expr)*
 */

static luna_node_t *
logical_and_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("logical_and_expr");
  if (!(node = bitwise_or_expr(self))) return NULL;
  // while (accept(OP_AND)) {
  //   context("&& operation");
  //   if (!bitwise_or_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * logical_and_expr ('||' logical_and_expr)*
 */

static luna_node_t *
logical_or_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("logical_or_expr");
  if (!(node = logical_and_expr(self))) return NULL;
  // while (accept(OP_OR)) {
  //   context("|| operation");
  //   if (!logical_and_expr(self)) return error("missing right-hand expression");
  // }
  return node;
}

/*
 * (id (',' id)*)
 */

// static int
// params(luna_parser_t *self) {
//   debug("params");
//   context("function params");
//   do {
//     if (!accept(ID)) {
//       return error("missing identifier");
//     };
//   } while (accept(COMMA));
//   return 1;
// }

/*
 * ':' params? block
 */

// static int
// function_expr(luna_parser_t *self) {
//   debug("function_expr");
//   if (accept(COLON)) {
//     if (is(ID)) if (!params(self)) return 0;
//     context("function literal");
//     if (block(self)) return 1;
//   };
//   return 0;
// }

/*
 * (primary_expr | function_expr) id* function_expr?
 */

static luna_node_t *
slot_access_expr(luna_parser_t * self) {
  luna_node_t *node;
  debug("slot_access_expr");
  if (!(node = primary_expr(self))) return NULL;
  // if (!(primary_expr(self) || function_expr(self))) return 0;
  // while (accept(ID)) ;
  // if (is(COLON)) {
  //   if (!function_expr(self)) return 0;
  // }
  return node;
}

/*
 * (expr (',' expr)*)
 */

// static int
// args(luna_parser_t *self) {
//   debug("args");
//   context("function arguments");
//   do {
//     if (!expr(self)) return 0;
//   } while (accept(COMMA));
//   return 1;
// }

/*
 *   slot_access_expr '(' args? ')' function_expr?
 * | slot_access_expr
 */

static luna_node_t *
call_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("call_expr");
  if (!(node = slot_access_expr(self))) return NULL;
  // if (accept(LPAREN)) {
  //   if (accept(RPAREN)) goto tail;
  //   if (!args(self)) return 0;
  //   context("function call");
  //   if (!accept(RPAREN)) return error("missing closing ')'");
  //   tail:
  //   if (is(COLON)) {
  //     if (!function_expr(self)) return 0;
  //   }
  // }
  return node;
}

/*
 *   logical_or_expr
 * | call_expr '=' not_expr
 * | call_expr ':=' not_expr
 */

static luna_node_t *
assignment_expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("assignment_expr");
  if (!(node = logical_or_expr(self))) return NULL;
  // if (accept(OP_ASSIGN) || accept(OP_SLOT_ASSIGN)) {
  //   context("assignment");
  //   if (!not_expr(self)) return 0;
  // }
  return node;
}

/*
 *   'not' not_expr
 * | assignment_expr
 */

static luna_node_t *
not_expr(luna_parser_t *self) {
  debug("not_expr");
  //if (accept(OP_LNOT)) return not_expr(self);
  return assignment_expr(self);
}

/*
 * not_expr (',' not_expr)*
 */

static luna_node_t *
expr(luna_parser_t *self) {
  luna_node_t *node;
  debug("expr");
  if (!(node = not_expr(self))) return NULL;
  // while (accept(COMMA)) {
  //   if (!not_expr(self)) return NULL;
  // }
  return node;
}

/*
 * expr (';' | newline)
 */

static luna_node_t *
expr_stmt(luna_parser_t *self) {
  luna_node_t *node;
  debug("expr_stmt");
  
  if (!(node = expr(self))) return NULL;
  if (!(accept(SEMICOLON) || accept(NEWLINE) || is(OUTDENT))) {
    return error("missing newline or semi-colon");
  }

  return node;
}

// /*
//  *  ('if' | 'unless') expr block
//  *  ('else' 'if' block)*
//  *  ('else' block)?
//  */
// 
// static luna_node_t *
// if_stmt(luna_parser_t *self) {
//   debug("if_stmt");
// 
//   accept(UNLESS) || accept(IF);
// 
//   context("if statement condition");
//   if (!expr(self)) return 0;
//   context("if statement");
//   if (!block(self)) return 0;
// 
//   // else
// loop:
//   if (accept(ELSE)) {
//     context("else statement");
// 
//     // else if
//     if (accept(IF)) {
//       context("else if statement condition");
//       if (!expr(self)) return 0;
//       context("else if statement");
//       if (!block(self)) return 0;
//       goto loop;
//     } else if (!block(self)) {
//       return 0;
//     }
//   }
// 
//   return 1;
// }

// /*
//  * ('while' | 'until')
//  */
// 
// static luna_node_t *
// while_stmt(luna_parser_t *self) {
//   debug("while_stmt");
//   accept(UNTIL) || accept(WHILE);
//   context("while statement condition");
//   if (!expr(self)) return NULL;
//   context("while statement");
//   if (!block(self)) return NULL;
//   return 1;
// }

/*
 *   if_stmt
 * | while_stmt
 * | expr_stmt
 */

static luna_node_t *
stmt(luna_parser_t *self) {
  debug("stmt");
  context("statement");
  // if (is(IF) || is(UNLESS)) return if_stmt(self);
  // if (is(WHILE) || is(UNTIL)) return while_stmt(self);
  return expr_stmt(self);
}

/*
 * INDENT ws (stmt ws)+ OUTDENT
 */

// static int
// block(luna_parser_t *self) {
//   debug("block");
//   if (!accept(INDENT)) return error("block expected");
//   whitespace(self);
//   do {
//     if (!stmt(self)) return 0;
//     whitespace(self);
//   } while (!accept(OUTDENT));
//   return 1;
// }

/*
 * ws (stmt ws)*
 */

static luna_block_node_t *
program(luna_parser_t *self) {
  luna_node_t *node;
  whitespace(self);
  debug("program");
  luna_block_node_t *block = luna_block_node_new();
  while (!accept(EOS)) {
    if (node = stmt(self)) {
      luna_array_push(block->stmts, luna_node(node));
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
  luna_block_node_t *block = program(self);

// -DEBUG_AST
#ifdef EBUG_AST
  luna_prettyprint((luna_node_t *) block);
#endif

  return block;
}

