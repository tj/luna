
//
// errors.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include "errors.h"

/*
 * Report syntax or parse error.
 */

void
luna_report_error(luna_parser_t *parser) {
  char *err, *type = "parse";
  luna_lexer_t *lex = parser->lex;

  // error message
  if (parser->err) {
    err = parser->err;
  // lexer
  } else if (lex->error) {
    err = lex->error;
    type = "syntax";
  // generate
  } else {
    char buf[64];
    snprintf(buf, 64, "unexpected token '%s'", luna_token_type_string(lex->tok.type));
    err = buf;
  } 

  fprintf(stderr
    , "luna(%s:%d). %s error in %s, %s.\n"
    , lex->filename
    , lex->lineno
    , type
    , parser->ctx
    , err);
}
