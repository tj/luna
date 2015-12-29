#pragma once

//
// parser.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include "lexer.h"
#include "ast.h"

/*
 * Parser struct.
 */

typedef struct {
  char *ctx;
  char *err;
  int in_args;
  luna_token_t *tmp;
  luna_token_t *la;
  luna_token_t lb;
  luna_lexer_t *lex;
} luna_parser_t;

// protos

void
luna_parser_init(luna_parser_t *self, luna_lexer_t *lex);

luna_block_node_t *
luna_parse(luna_parser_t *self);
