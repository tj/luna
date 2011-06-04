
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

#define is(t) (peek->type == (LUNA_TOKEN_##t))

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
 * Expect a token, advancing the lexer,
 * or issuing an error.
 */

#define expect(t) \
  (peek->type == LUNA_TOKEN_##t \
    ? next \
    : (self->expected = LUNA_TOKEN_##t, 0))

/*
 * Accept a token, advancing the lexer.
 */

#define accept(t) \
  (peek->type == LUNA_TOKEN_##t \
    ? next \
    : 0)

// protos

static int block(luna_parser_t *self);

/*
 * Initialize with the given lexer.
 */

void
luna_parser_init(luna_parser_t *self, luna_lexer_t *lex) {
  self->lex = lex;
  self->la = NULL;
  self->ctx = NULL;
  self->err = NULL;
  self->expected = -1;
}

/*
 * newline*
 */

static void
whitespace(luna_parser_t *self) {
  while (accept(NEWLINE)) ;
}

/*
 *   id
 * | string
 * | int
 * | float
 */

static int
primitive_expr(luna_parser_t *self) {
  debug("primitive_expr");
  return accept(ID)
    || accept(STRING)
    || accept(INT)
    || accept(FLOAT)
    ;
}

/*
 * assignment_expr
 */

static int
expr(luna_parser_t *self) {
  debug("expr");
  return primitive_expr(self);
}

/*
 * expr
 */

static int
expr_stmt(luna_parser_t *self) {
  debug("expr_stmt");
  if (!expr(self)) return 0;
  if (!(accept(SEMICOLON) || accept(NEWLINE))) {
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

  accept(UNLESS) || expect(IF);

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
  accept(UNTIL) || expect(WHILE);
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
  expect(INDENT);
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

