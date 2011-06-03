
//
// token.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include "token.h"

/*
 * Inspect the given `tok`, outputting debugging
 * information to stdout.
 */

void
luna_token_inspect(luna_token_t *tok) {
  switch (tok->type) {
    case LUNA_TOKEN_INT:
      printf("\033[90minteger\033[0m \033[36m%d\033[0m\n", tok->value.as_int);
      break;
    case LUNA_TOKEN_FLOAT:
      printf("\033[90mfloat\033[0m \033[36m%f\033[0m\n", tok->value.as_float);
      break;
    case LUNA_TOKEN_STRING:
      printf("\033[90mstring\033[0m \033[32m'%s'\033[0m\n", tok->value.as_string);
      break;
    case LUNA_TOKEN_ID:
      printf("\033[90midentifier\033[0m %s\n", tok->value.as_string);
      break;
    default:
      printf("\033[90m%s\033[0m\n", luna_token_type_string(tok->type));
      break;
  }
}
