
//
// parser.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "parser.h"

// TODO: emit for codegen

// -DEBUG_PARSER

#ifdef EBUG_PARSER
#define debug(name) \
  fprintf(stderr, "\n\033[90m%s\033[0m\n", name); \
  luna_token_inspect(&self->lex->tok);
#else
#define debug(name)
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

/*
 * Accept a token, advancing the lexer.
 */

#define accept(t) \
  (peek->type == LUNA_TOKEN_##t \
    ? next \
    : 0)

// forward declarations

static int block(luna_parser_t *self);
static int expr(luna_parser_t *self);

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
 * primary_expr (('<' | '<=' | '>' | '>=') primary_expr)*
 */

static int
relational_expr(luna_parser_t *self) {
  debug("relational_expr");
  if (!primary_expr(self)) return 0;
  while (accept(OP_LT) || accept(OP_LTE) || accept(OP_GT) || accept(OP_GTE)) {
    if (!primary_expr(self)) return 0;
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
    if (!relational_expr(self)) return 0;
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
    if (!equality_expr(self)) return 0;
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
    if (!bitwise_and_expr(self)) return 0;
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
    if (!bitwise_xor_expr(self)) return 0;
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
    if (!bitwise_or_expr(self)) return 0;
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
    if (!logical_and_expr(self)) return 0;
  }
  return 1;
}

/*
 *   primary_expr '=' assignment_expr
 * | primary_expr
 */

static int
assignment_expr(luna_parser_t *self) {
  debug("assignment_expr");
  int lval = is(ID);
  if (!logical_or_expr(self)) return 0;
  if (accept(OP_ASSIGN)) {
    if (!lval) return error("invalid left-hand side value in assignment");
    if (!assignment_expr(self)) return 0;
  }
  return 1;
}

/*
 * assignment_expr (',' assignment_expr)*
 */

static int
expr(luna_parser_t *self) {
  debug("expr");
  if (!assignment_expr(self)) return 0;
  while (accept(COMMA)) {
    if (!assignment_expr(self)) return 0;
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

