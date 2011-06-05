
//
// parser.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "parser.h"

// TODO: emit for codegen
// TODO: -DEBUG_PARSER output for accept() etc
// TODO: contextual errors

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
    : str), 0)

// forward declarations

static int block(luna_parser_t *self);
static int expr(luna_parser_t *self);
static int call_expr(luna_parser_t *self);
static int not_expr(luna_parser_t *self);

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

static int
paren_expr(luna_parser_t *self) {
  debug("paren_expr");
  if (!accept(LPAREN)) return 0;
  if (!expr(self)) return 0;
  if (!accept(RPAREN)) return error("expression missing closing ')'");
  return 1;
}

/*
 *   id
 * | string
 * | int
 * | float
 * | paren_expr
 */

static int
primary_expr(luna_parser_t *self) {
  debug("primary_expr");
  return accept(ID)
    || accept(STRING)
    || accept(INT)
    || accept(FLOAT)
    || paren_expr(self);
    ;
}

/*
 *   call_expr
 * | call_expr '**' call_expr
 */

static int
pow_expr(luna_parser_t *self) {
  debug("pow_expr");
  if (!call_expr(self)) return 0;
  if (accept(OP_POW)) {
    context("** operation");
    if (!call_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 *   pow_expr
 * | pow_expr '++'
 * | pow_expr '--'
 */

static int
postfix_expr(luna_parser_t *self) {
  debug("postfix_expr");
  if (!pow_expr(self)) return 0;
  if (accept(OP_INCR) || accept(OP_DECR)) ;
  return 1;
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

static int
unary_expr(luna_parser_t *self) {
  debug("unary_expr");
  if (accept(OP_INCR)
    || accept(OP_DECR)
    || accept(OP_BIT_NOT)
    || accept(OP_PLUS)
    || accept(OP_MINUS)
    || accept(OP_NOT)) {
    return unary_expr(self);
  }
  return postfix_expr(self);
}

/*
 * unary_expr (('* | '/' | '%') unary_expr)*
 */

static int
multiplicative_expr(luna_parser_t *self) {
  debug("multiplicative_expr");
  if (!unary_expr(self)) return 0;
  while (accept(OP_MULT) || accept(OP_DIV) || accept(OP_MOD)) {
    context("multiplicative operation");
    if (!unary_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * multiplicative_expr (('+ | '-') multiplicative_expr)*
 */

static int
concat_expr(luna_parser_t *self) {
  debug("concat_expr");
  if (!multiplicative_expr(self)) return 0;
  while (accept(OP_DOT)) {
    context("concatenation operation");
    if (!multiplicative_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * concat_expr (('+ | '-') concat_expr)*
 */

static int
additive_expr(luna_parser_t *self) {
  debug("additive_expr");
  if (!concat_expr(self)) return 0;
  while (accept(OP_PLUS) || accept(OP_MINUS)) {
    context("additive operation");
    if (!concat_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * additive_expr (('<<' | '>>') additive_expr)*
 */

static int
shift_expr(luna_parser_t *self) {
  debug("shift_expr");
  if (!additive_expr(self)) return 0;
  while (accept(OP_BIT_SHL) || accept(OP_BIT_SHR)) {
    context("shift operation");
    if (!additive_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * shift_expr (('<' | '<=' | '>' | '>=') shift_expr)*
 */

static int
relational_expr(luna_parser_t *self) {
  debug("relational_expr");
  if (!shift_expr(self)) return 0;
  while (accept(OP_LT) || accept(OP_LTE) || accept(OP_GT) || accept(OP_GTE)) {
    context("relational operation");
    if (!shift_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * relational_expr (('==' | '!=') relational_expr)*
 */

static int
equality_expr(luna_parser_t *self) {
  debug("equality_expr");
  if (!relational_expr(self)) return 0;
  while (accept(OP_EQ) || accept(OP_NEQ)) {
    context("equality operation");
    if (!relational_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * equality_expr ('&' equality_expr)*
 */

static int
bitwise_and_expr(luna_parser_t *self) {
  debug("bitwise_and_expr");
  if (!equality_expr(self)) return 0;
  while (accept(OP_BIT_AND)) {
    context("& operation");
    if (!equality_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * bitwise_and_expr ('^' bitwise_and_expr)*
 */

static int
bitwise_xor_expr(luna_parser_t *self) {
  debug("bitwise_xor_expr");
  if (!bitwise_and_expr(self)) return 0;
  while (accept(OP_BIT_XOR)) {
    context("^ operation");
    if (!bitwise_and_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * bitwise_xor_expr ('|' bitwise_xor_expr)*
 */

static int
bitwise_or_expr(luna_parser_t *self) {
  debug("bitwise_or_expr");
  if (!bitwise_xor_expr(self)) return 0;
  while (accept(OP_BIT_OR)) {
    context("| operation");
    if (!bitwise_xor_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * bitwise_or_expr ('&&' bitwise_or_expr)*
 */

static int
logical_and_expr(luna_parser_t *self) {
  debug("logical_and_expr");
  if (!bitwise_or_expr(self)) return 0;
  while (accept(OP_AND)) {
    context("&& operation");
    if (!bitwise_or_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * logical_and_expr ('||' logical_and_expr)*
 */

static int
logical_or_expr(luna_parser_t *self) {
  debug("logical_or_expr");
  if (!logical_and_expr(self)) return 0;
  while (accept(OP_OR)) {
    context("|| operation");
    if (!logical_and_expr(self)) return error("missing right-hand expression");
  }
  return 1;
}

/*
 * (id (',' id)*)?
 */

static void
params(luna_parser_t *self) {
  debug("params");
  do accept(ID); while (accept(COMMA));
}

/*
 * ':' params block
 */

static int
function_expr(luna_parser_t *self) {
  debug("function_expr");
  if (accept(COLON)) {
    params(self);
    if (block(self)) {
      return 1;
    }
  };
  return 0;
}

/*
 * (primary_expr | function_expr) id*
 */

static int
slot_access_expr(luna_parser_t * self) {
  debug("slot_access_expr");
  if (!(primary_expr(self) || function_expr(self))) return 0;
  while (accept(ID)) ;
  return 1;
}

/*
 * (expr (',' expr)*)?
 */

static void
args(luna_parser_t *self) {
  debug("args");
  do expr(self); while (accept(COMMA));
}

/*
 *   slot_access_expr '(' args ')'
 * | slot_access_expr
 */

static int
call_expr(luna_parser_t *self) {
  debug("call_expr");
  if (!slot_access_expr(self)) return 0;
  if (accept(LPAREN)) {
    args(self);
    if (!accept(RPAREN)) return error("call missing closing ')'");
  }
  return 1;
}

/*
 *   logical_or_expr
 * | call_expr '=' not_expr
 * | call_expr ':=' not_expr
 */

static int
assignment_expr(luna_parser_t *self) {
  debug("assignment_expr");
  if (!logical_or_expr(self)) return 0;
  if (accept(OP_ASSIGN) || accept(OP_SLOT_ASSIGN)) {
    if (!not_expr(self)) return error("assignment missing right-hand expression");
  }
  return 1;
}

/*
 *   'not' not_expr
 * | assignment_expr
 */

static int
not_expr(luna_parser_t *self) {
  debug("not_expr");
  if (accept(OP_LNOT)) return not_expr(self);
  return assignment_expr(self);
}

/*
 * not_expr (',' not_expr)*
 */

static int
expr(luna_parser_t *self) {
  debug("expr");
  context("expr");
  if (!not_expr(self)) return 0;
  while (accept(COMMA)) {
    if (!not_expr(self)) return 0;
  }
  return 1;
}

/*
 * expr (';' | newline)
 */

static int
expr_stmt(luna_parser_t *self) {
  debug("expr_stmt");
  if (!expr(self)) return 0;
  if (!(accept(SEMICOLON) || accept(NEWLINE) || is(OUTDENT))) {
    return error("missing newline or semi-colon");
  }
  return 1;
}

/*
 *  ('if' | 'unless') expr block
 *  ('else' 'if' block)*
 *  ('else' block)?
 */

static int
if_stmt(luna_parser_t *self) {
  debug("if_stmt");
  context("if statement");

  accept(UNLESS) || accept(IF);

  if (!expr(self)) return error("missing condition");
  if (!block(self)) return 0;

  // else
loop:
  if (accept(ELSE)) {
    context("else statement");

    // else if
    if (accept(IF)) {
      context("else if statement");
      if (!expr(self)) return error("missing condition");
      if (!block(self)) return 0;
      goto loop;
    } else if (!block(self)) {
      return 0;
    }
  }

  return 1;
}

/*
 * ('while' | 'until')
 */

static int
while_stmt(luna_parser_t *self) {
  debug("while_stmt");
  context("while statement");
  accept(UNTIL) || accept(WHILE);
  if (!expr(self)) return error("missing condition");
  if (!block(self)) return 0;
  return 1;
}

/*
 *   if_stmt
 * | while_stmt
 * | expr_stmt
 */

static int
stmt(luna_parser_t *self) {
  debug("stmt");
  context("statement");
  if (is(IF) || is(UNLESS)) return if_stmt(self);
  if (is(WHILE) || is(UNTIL)) return while_stmt(self);
  return expr_stmt(self);
}

/*
 * INDENT ws (stmt ws)+ OUTDENT
 */

static int
block(luna_parser_t *self) {
  debug("block");
  if (!accept(INDENT)) return error("block expected");
  whitespace(self);
  do {
    if (!stmt(self)) return 0;
    whitespace(self);
  } while (!accept(OUTDENT));
  return 1;
}

/*
 * ws (stmt ws)*
 */

static int
program(luna_parser_t *self) {
  whitespace(self);
  debug("program");
  context("program");
  while (!accept(EOS)) {
    if (!stmt(self)) return 0;
    whitespace(self);
  }
  return 1;
}

/*
 * Parse input.
 */

int
luna_parse(luna_parser_t *self) {
  luna_lexer_t *lex = self->lex;
  return program(self);
}

