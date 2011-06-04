
//
// parser.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "parser.h"

#define inspect luna_token_inspect(&self->lex->tok);
#define next (luna_scan(self->lex), &self->lex->tok)
#define peek (self->la ? self->la : (self->la = next))
#define accept(t) (peek->type == (LUNA_TOKEN_##t) ? next : 0)

/*
 * Initialize with the given lexer.
 */

void
luna_parser_init(luna_parser_t *self, luna_lexer_t *lex) {
  self->lex = lex;
  self->la = NULL;
  self->error = NULL;
}

/*
 * newline*
 */

static void
whitespace(luna_parser_t *self) {
  while (accept(NEWLINE)) ;
}

/*
 * ws
 */

static int
parse_stmt(luna_parser_t *self) {
  whitespace(self);
  return 1;
}

/*
 * stmt+
 */

static int
parse_program(luna_parser_t *self) {
  while (luna_scan(self->lex)) {
    if (!parse_stmt(self)) return 0;
  }
  return 1;
}

/*
 * Parse input.
 */

int
luna_parse(luna_parser_t *self) {
  luna_lexer_t *lex = self->lex;
  return parse_program(self);
}

