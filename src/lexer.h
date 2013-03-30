
//
// lexer.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_LEXER__
#define __LUNA_LEXER__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "token.h"

#ifndef LUNA_BUF_SIZE
#define LUNA_BUF_SIZE 1024
#endif

/*
 * Lexer struct.
 */

typedef struct {
  char *error;
  int stash;
  int lineno;
  off_t offset;
  char *source;
  const char *filename;
  luna_token_t tok;
  char buf[LUNA_BUF_SIZE];
} luna_lexer_t;

// protos

int
luna_scan(luna_lexer_t *self);

void
luna_lexer_init(luna_lexer_t *self, char *source, const char *filename);

#endif /* __LUNA_LEXER__ */