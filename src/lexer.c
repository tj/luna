
//
// lexer.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"

#define next (self->stash = fgetc(self->stream))
#define undo ungetc(self->stash, self->stream)
#define token(t) self->tok.type = LUNA_TOKEN_##t

/*
 * Initialize lexer with the given `stream` and `filename`.
 */

void
luna_lexer_init(luna_lexer_t *self, FILE *stream, const char *filename) {
  self->error = NULL;
  self->stream = stream;
  self->filename = filename;
  self->indents = 0;
  self->outdents = 0;
  self->lineno = 1;
}

/*
 * Set error `msg` and token to ILLEGAL.
 */

static void
error(luna_lexer_t *self, char *msg) {
  self->error = msg;
  token(ILLEGAL);
}

/*
 * Convert hex digit `c` to an integer,
 * returning -1 on failure. 
 */

static int
hex(const char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

/*
 * Accept char `c`, returning it, or 0 otherwise.
 */

static int
accept(luna_lexer_t *self, int c) {
  if (c == next) return c;
  undo;
  return 0;
}

/*
 * Perform an outdent.
 */

static int
outdent(luna_lexer_t *self) {
  --self->outdents;
  return token(OUTDENT);
}

/*
 * Scan newline, and determine if we have
 * an indentation.
 */

static int
scan_newline(luna_lexer_t *self) {
  int curr = 0;
  int prev = self->indent_stack[self->indents];

  ++self->lineno;

  while (accept(self, ' ')) ++curr;

  if (curr > prev) {
    token(INDENT);
  } else if (curr < prev) {
    while (self->indent_stack[self->indents] > curr) {
      ++self->outdents;
      --self->indents;
    }
    outdent(self);
  } else {
    token(NEWLINE);
  }

  self->indent_stack[++self->indents] = curr;
  return 1;
}

/*
 * Scan identifier.
 */

static int
scan_ident(luna_lexer_t *self, int c) {
  int len = 0;
  char buf[128]; // TODO: ditch these buffers
  token(ID);

  do {
    buf[len++] = c;
  } while (isalpha(c = next) || isdigit(c) || '_' == c);

  buf[len++] = 0;
  self->tok.value.as_string = strdup(buf); // TODO: remove
  undo;
  return 1;
}

/*
 * Scan string hex literal, returning -1 on invalid digits.
 */

static int
hex_literal(luna_lexer_t *self) {
  int a = hex(next)
    , b = hex(next);

  if (a > -1 && b > -1) return a << 4 | b;

  error(self, "string hex literal \\x contains invalid digits");
  return -1;
}

/*
 * Scan string.
 */

static int
scan_string(luna_lexer_t *self, int quote) {
  int c, len = 0;
  char buf[128];
  token(STRING);

  while (quote != (c = next)) {
    switch (c) {
      case '\n': ++self->lineno; break;
      case '\\':
        switch (c = next) {
          case 'a': c = '\a'; break;
          case 'b': c = '\b'; break;
          case 'e': c = '\e'; break;
          case 'f': c = '\f'; break;
          case 'n': c = '\n'; break;
          case 'r': c = '\r'; break;
          case 't': c = '\t'; break;
          case 'v': c = '\v'; break;
          case 'x':
            if (-1 == (c = hex_literal(self)))
              return 0;
        }
        break; 
    }
    buf[len++] = c;
  }

  buf[len++] = 0;
  self->tok.value.as_string = strdup(buf); // TODO: remove
  return 1;
}

/*
 * Scan number.
 */

static int
scan_number(luna_lexer_t *self, int c) {
  // TODO: exponential notation
  int n = 0;
  token(INT);

  switch (c) {
    case '0': goto scan_hex;
    case '.': goto scan_float;
    default: goto scan_int;
  }

scan_hex:
  switch (c = next) {
    case 'x':
      if (!isxdigit(c = next)) {
        error(self, "hex literal expects one or more digits");
        return 0;
      } else {
        do n = n << 4 | hex(c);
        while (isxdigit(c = next));
      }
      self->tok.value.as_int = n;
      undo;
      return 1;
    default:
      goto scan_int;
  }

// [0-9_]+

scan_int:
  do {
    if ('_' == c) continue;
    else if ('.' == c) goto scan_float;
    n = n * 10 + c - '0';
  } while (isdigit(c = next) || '_' == c || '.' == c);
  undo;
  self->tok.value.as_int = n;
  return 1;

// [0-9_]+

scan_float: {
  int e = 1;
  token(FLOAT);
  while (isdigit(c = next) || '_' == c) {
    if ('_' == c) continue;
    n = n * 10 + c - '0';
    e *= 10;
  }
  undo;
  self->tok.value.as_float = (float) n / e;
}

  return 1;
}

/*
 * Scan the next token in the stream, returns 0
 * on EOS, ILLEGAL token, or a syntax error.
 */

int
luna_lexer_next(luna_lexer_t *self) {
  int c;
  token(ILLEGAL);

  // deferred outdents
  if (self->outdents) return outdent(self);

  // scan
scan:
  switch (c = next) {
    case ' ':
    case '\t': goto scan;
    case '(': return token(LPAREN);
    case ')': return token(RPAREN);
    case '{': return token(LBRACE);
    case '}': return token(RBRACE);
    case '[': return token(LBRACK);
    case ']': return token(RBRACK);
    case ';': return token(SEMICOLON);
    case ',': return token(COMMA);
    case '?': return token(QMARK);
    case '/': return token(OP_DIV);
    case '%': return token(OP_MOD);
    case '^': return token(OP_BIT_XOR);
    case '~': return token(OP_BIT_NOT);
    case '+':
      return '+' == next
        ? token(OP_INCR)
        : (undo, token(OP_PLUS));
    case '-':
      return '-' == next
        ? token(OP_DECR)
        : (undo, token(OP_MINUS));
    case '*':
      return '*' == next
        ? token(OP_EX)
        : (undo, token(OP_MULT));
    case ':':
      return '=' == next
        ? token(OP_SLOT_ASSIGN)
        : (undo, token(COLON));
    case '!':
      return '=' == next
        ? token(OP_NEQ)
        : (undo, token(OP_NOT));
    case '=':
      return '=' == next
        ? token(OP_EQ)
        : (undo, token(OP_ASSIGN));
    case '&':
      return '&' == next
        ? token(OP_AND)
        : (undo, token(OP_BIT_AND));
    case '|':
      return '|' == next
        ? token(OP_OR)
        : (undo, token(OP_BIT_OR));
    case '<':
      if ('=' == next) return token(OP_LTE); undo;
      if ('<' == next) return token(OP_BIT_SHL); undo;
      return token(OP_LT);
    case '>':
      if ('=' == next) return token(OP_GTE); undo;
      if ('>' == next) return token(OP_BIT_SHR); undo;
      return token(OP_GT);
    case '\n':
      return scan_newline(self);
    case '"':
    case '\'':
      return scan_string(self, c);
    case EOF:
      token(EOS);
      return 0;
    default:
      if (isalpha(c) || '_' == c) return scan_ident(self, c);
      if (isdigit(c) || '.' == c) return scan_number(self, c);
      error(self, "illegal character");
      return 0;
  }
}