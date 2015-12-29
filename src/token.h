#pragma once

//
// token.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include <assert.h>

/*
 * Tokens.
 */

#define LUNA_TOKEN_LIST \
  t(ILLEGAL, "illegal") \
  t(EOS, "end-of-source") \
  t(ID, "id") \
  t(INT, "int") \
  t(FLOAT, "float") \
  t(STRING, "string") \
  t(DEF, "def") \
  t(TYPE, "type") \
  t(WHILE, "while") \
  t(UNTIL, "until") \
  t(IF, "if") \
  t(UNLESS, "unless") \
  t(ELSE, "else") \
  t(FOR, "for") \
  t(LET, "let") \
  t(END, "end") \
  t(RETURN, "return") \
  t(LBRACE, "{") \
  t(RBRACE, "}") \
  t(LPAREN, "(") \
  t(RPAREN, ")") \
  t(LBRACK, "[") \
  t(RBRACK, "]") \
  t(COLON, ":") \
  t(QMARK, "?") \
  t(SEMICOLON, ";") \
  t(COMMA, ",") \
  t(OP_DOT, ".") \
  t(OP_LNOT, "not") \
  t(OP_NOT, "!") \
  t(OP_FORK, "&") \
  t(OP_PLUS, "+") \
  t(OP_INCR, "++") \
  t(OP_MINUS, "-") \
  t(OP_DECR, "--") \
  t(OP_MUL, "*") \
  t(OP_DIV, "/") \
  t(OP_MOD, "%") \
  t(OP_POW, "**") \
  t(OP_GT, ">") \
  t(OP_LT, "<") \
  t(OP_GTE, ">=") \
  t(OP_LTE, "<=") \
  t(OP_EQ, "==") \
  t(OP_NEQ, "!=") \
  t(OP_AND, "&&") \
  t(OP_OR, "||") \
  t(OP_ASSIGN, "=") \
  t(OP_PLUS_ASSIGN, "+=") \
  t(OP_MINUS_ASSIGN, "-=") \
  t(OP_MUL_ASSIGN, "*=") \
  t(OP_DIV_ASSIGN, "/=") \
  t(OP_AND_ASSIGN, "&&=") \
  t(OP_OR_ASSIGN, "||=") \
  t(OP_BIT_AND, "and") \
  t(OP_BIT_OR, "|") \
  t(OP_BIT_XOR, "^") \
  t(OP_BIT_NOT, "~") \
  t(OP_BIT_SHL, "<<") \
  t(OP_BIT_SHR, ">>")

/*
 * Tokens enum.
 */

typedef enum {
#define t(tok, str) LUNA_TOKEN_##tok,
LUNA_TOKEN_LIST
#undef t
} luna_token;

/*
 * Token strings.
 */

static char *luna_token_strings[] = {
#define t(tok, str) str,
LUNA_TOKEN_LIST
#undef t
};

/*
 * Token struct.
 */

typedef struct {
  int len;
  luna_token type;
  struct {
    char *as_string;
    float as_float;
    int as_int;
  } value;
} luna_token_t;

/*
 * Return the string associated with the
 * given token `type`.
 */

static inline const char *
luna_token_type_string(luna_token type) {
  assert(type <= LUNA_TOKEN_OP_BIT_SHR);
  return luna_token_strings[type];
}

// protos

void
luna_token_inspect(luna_token_t *tok);
