
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

/*
 * Next char in the stream.
 */

#ifdef EBUG_LEXER
#define next \
  (self->stash = fgetc(self->stream)\
    , fprintf(stderr, "'%c'\n", self->stash)\
    , self->stash)
#else
#define next (self->stash = fgetc(self->stream))
#endif

/*
 * Undo the previous char.
 */

#define undo ungetc(self->stash, self->stream)

/*
 * Assign token `t`.
 */

#define token(t) (self->tok.type = LUNA_TOKEN_##t)

/*
 * Accept char `c` or undo and return 0.
 */

#define accept(c) (c == next ? c : (undo, 0))

/*
 * Set error `msg` and assign ILLEGAL token.
 */

#define error(msg) (self->error = msg, token(ILLEGAL))

/*
 * Initialize lexer with the given `stream` and `filename`.
 */

void
luna_lexer_init(luna_lexer_t *self, FILE *stream, const char *filename) {
  self->error = NULL;
  self->stream = stream;
  self->filename = filename;
  self->indent_stack[0] = 0;
  self->indents = 0;
  self->outdents = 0;
  self->lineno = 1;
}

/*
 * Convert hex digit `c` to a base 10 int,
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

  while (accept(' ')) ++curr;

  if (curr > prev) {
    token(INDENT);
    self->indent_stack[++self->indents] = curr;
  } else if (curr < prev) {
    while (self->indent_stack[self->indents] > curr) {
      ++self->outdents;
      --self->indents;
    }
    outdent(self);
  } else {
    token(NEWLINE);
  }

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
  undo;

  buf[len++] = 0;
  // TODO: refactor this lameness with length checks etc
  if (0 == strcmp("if", buf)) return token(IF); 
  else if (0 == strcmp("else", buf)) return token(ELSE); 
  else if (0 == strcmp("unless", buf)) return token(UNLESS); 
  else if (0 == strcmp("while", buf)) return token(WHILE); 
  else if (0 == strcmp("until", buf)) return token(UNTIL); 
  else if (0 == strcmp("for", buf)) return token(FOR); 
  else if (0 == strcmp("return", buf)) return token(RETURN); 
  else if (0 == strcmp("not", buf)) return token(OP_LNOT); 
  self->tok.value.as_string = strdup(buf); // TODO: remove
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
  error("string hex literal \\x contains invalid digits");
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
  int n = 0, type = 0;
  token(INT);

  switch (c) {
    case '0': goto scan_hex;
    default: goto scan_int;
  }

  scan_hex:
  switch (c = next) {
    case 'x':
      if (!isxdigit(c = next)) {
        error("hex literal expects one or more digits");
        return 0;
      } else {
        do n = n << 4 | hex(c);
        while (isxdigit(c = next));
      }
      self->tok.value.as_int = n;
      undo;
      return 1;
    default:
      undo;
      c = '0';
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

  // [0-9_]+

  scan_float: {
    int e = 1;
    type = 1;
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
luna_scan(luna_lexer_t *self) {
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
    case ',': return token(COMMA);
    case '.': return token(OP_DOT);
    case '%': return token(OP_MOD);
    case '^': return token(OP_BIT_XOR);
    case '~': return token(OP_BIT_NOT);
    case '?': return token(QMARK);
    case ':': return token(COLON);
    case '#':
      self->tok.value.as_int = next;
      return token(INT);
    case '+':
      switch (next) {
        case '+': return token(OP_INCR);
        case '=': return token(OP_PLUS_ASSIGN);
        default: return undo, token(OP_PLUS);
      }
    case '-':
      switch (next) {
        case '-': return token(OP_DECR);
        case '=': return token(OP_MINUS_ASSIGN);
        default: return undo, token(OP_MINUS);
      }
    case '*':
      return '*' == next
        ? token(OP_POW)
        : (undo, token(OP_MULT));
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
    case '/':
      if ('/' == next) {
        while ((c = next) != '\n' && EOF != c) ; undo;
        goto scan;
      }
      return token(OP_DIV);
    case '\n':
      return scan_newline(self);
    case '"':
    case '\'':
      return scan_string(self, c);
    case EOF:
      if (self->indents) {
        --self->indents;
        return token(OUTDENT);
      }
      token(EOS);
      return 0;
    default:
      if (isalpha(c) || '_' == c) return scan_ident(self, c);
      if (isdigit(c) || '.' == c) return scan_number(self, c);
      error("illegal character");
      return 0;
  }
}