//
// lexer.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"

/*
 * Next char in the array.
 */

#ifdef EBUG_LEXER
#define next \
  (self->stash = self->source[self->offset++]\
    , fprintf(stderr, "'%c'\n", self->stash)\
    , self->stash)
#else
#define next (self->stash = self->source[self->offset++])
#endif

/*
 * Undo the previous char.
 */

#define undo (self->source[--self->offset] = self->stash)

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
 * Initialize lexer with the given `source` and `filename`.
 */

void
luna_lexer_init(luna_lexer_t *self, char *source, const char *filename) {
  self->error = NULL;
  self->source = source;
  self->filename = filename;
  self->lineno = 1;
  self->offset = 0;
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
  switch (len-1) {
    case 2:
      if (0 == strcmp("if", buf)) return token(IF);
      break;
    case 3:
      if (0 == strcmp("for", buf)) return token(FOR);
      if (0 == strcmp("def", buf)) return token(DEF);
      if (0 == strcmp("end", buf)) return token(END);
      if (0 == strcmp("let", buf)) return token(LET);
      if (0 == strcmp("and", buf)) return token(OP_BIT_AND);
      if (0 == strcmp("not", buf)) return token(OP_LNOT);
      break;
    case 4:
      if (0 == strcmp("else", buf)) return token(ELSE);
      if (0 == strcmp("type", buf)) return token(TYPE);
      break;
    case 5:
      if (0 == strcmp("while", buf)) return token(WHILE);
      if (0 == strcmp("until", buf)) return token(UNTIL);
      break;
    default:
      if (0 == strcmp("return", buf)) return token(RETURN);
      if (0 == strcmp("unless", buf)) return token(UNLESS);
  }

  self->tok.value.as_string = strdup(buf);
  return 1;
}

/*
 * Scan string hex literal, returning -1 on invalid digits.
 */

static int
hex_literal(luna_lexer_t *self) {
  int a = hex(next);
  int b = hex(next);
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
  int n = 0, type = 0, expo = 0, e;
  int expo_type = 1;
  /* expo_type:
   * 1 -> '+'(default)
   * 0 -> '-'
   */
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
    else if ('e' == c || 'E' == c) goto scan_expo;
    n = n * 10 + c - '0';
  } while (isdigit(c = next) || '_' == c || '.' == c || 'e' == c || 'E' == c);
  undo;
  self->tok.value.as_int = n;
  return 1;

  // [0-9_]+

  scan_float: {
    e = 1;
    type = 1;
    token(FLOAT);
    while (isdigit(c = next) || '_' == c || 'e' == c || 'E' == c) {
      if ('_' == c) continue;
      else if ('e' == c || 'E' == c) goto scan_expo;
      n = n * 10 + c - '0';
      e *= 10;
    }
    undo;
    self->tok.value.as_float = (float) n / e;
    return 1;
  }

  // [\+\-]?[0-9]+

  scan_expo: {
    while (isdigit(c = next) || '+' == c || '-' == c) {
      if ('-' == c) {
        expo_type = 0;
        continue;
      }
      expo = expo * 10 + c - '0';
    }

    undo;
    if (expo_type == 0) expo *= -1;
    if (type == 0)
      self->tok.value.as_int = n * pow(10, expo);
    else
      self->tok.value.as_float = ((float) n / e) * pow(10, expo);
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
      switch (next) {
        case '=': return token(OP_MUL_ASSIGN);
        case '*': return token(OP_POW);
        default: return undo, token(OP_MUL);
      }
    case '/':
      return '=' == next
        ? token(OP_DIV_ASSIGN)
        : (undo, token(OP_DIV));
    case '!':
      return '=' == next
        ? token(OP_NEQ)
        : (undo, token(OP_NOT));
    case '=':
      return '=' == next
        ? token(OP_EQ)
        : (undo, token(OP_ASSIGN));
    case '&':
      switch (next) {
        case '&':
          return '=' == next
            ? token(OP_AND_ASSIGN)
            : (undo, token(OP_AND));
        default:
          return undo, token(OP_FORK);
      }
    case '|':
      switch (next) {
        case '|':
          return '=' == next
            ? token(OP_OR_ASSIGN)
            : (undo, token(OP_OR));
        default:
          return undo, token(OP_BIT_OR);
      }
    case '<':
      switch (next) {
        case '=': return token(OP_LTE);
        case '<': return token(OP_BIT_SHL);
        default: return undo, token(OP_LT);
      }
    case '>':
      switch (next) {
        case '=': return token(OP_GTE);
        case '>': return token(OP_BIT_SHR);
        default: return undo, token(OP_GT);
      }
    case '#':
      while ((c = next) != '\n' && c) ; undo;
      goto scan;
    case '\n':
    case '\r':
      ++self->lineno;
      goto scan;
    case '"':
    case '\'':
      return scan_string(self, c);
    case 0:
      token(EOS);
      return 0;
    default:
      if (isalpha(c) || '_' == c) return scan_ident(self, c);
      if (isdigit(c) || '.' == c) return scan_number(self, c);
      error("illegal character");
      return 0;
  }
}
