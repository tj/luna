
//
// lexer.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_LEXER__
#define __LUNA_LEXER__

#include <stdio.h>
#include "token.h"

#ifndef LUNA_BUF_SIZE
#define LUNA_BUF_SIZE 1024
#endif

#ifndef LUNA_MAX_INDENTS
#define LUNA_MAX_INDENTS 32
#endif

typedef struct {
  char *error;
  int stash;
  int lineno;
  int indent_stack[LUNA_MAX_INDENTS];
  int indents;
  int outdents;
  FILE *stream;
  const char *filename;
  luna_token_t tok;
  char buf[LUNA_BUF_SIZE];
} luna_lexer_t;

int
luna_lexer_next(luna_lexer_t *self);

void
luna_lexer_init(luna_lexer_t *self, FILE *stream, const char *filename);

#endif /* __LUNA_LEXER__ */