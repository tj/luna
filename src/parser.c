
//
// parser.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "parser.h"

/*
 * Initialize the parser with the given lexer.
 */

void
luna_parser_init(luna_parser_t *self, luna_lexer_t *lex) {
  self->lex = lex;
}

/*
 * Parser the input.
 */

void
luna_parse(luna_parser_t *self) {
  luna_lexer_t *lex = self->lex;

  while (luna_scan(lex)) {
#ifdef EBUG_LEXER
    luna_token_inspect(&lex->tok);
#endif
  }

  // lexical error
  if (lex->tok.type != LUNA_TOKEN_EOS) {
    fprintf(stderr
      , "luna(%s:%d): SyntaxError %s\n"
      , lex->filename
      , lex->lineno
      , lex->error);
  }
}