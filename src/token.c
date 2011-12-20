
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
  printf("\e[90m%s\e[0m", luna_token_type_string(tok->type));
  switch (tok->type) {
    case LUNA_TOKEN_INT:
      printf(" \e[36m%d\e[0m", tok->value.as_int);
      break;
    case LUNA_TOKEN_FLOAT:
      printf(" \e[36m%f\e[0m", tok->value.as_float);
      break;
    case LUNA_TOKEN_STRING:
      printf(" \e[32m'%s'\e[0m", tok->value.as_string);
      break;
    case LUNA_TOKEN_ID:
      printf(" %s", tok->value.as_string);
      break;
  }
  printf("\n");
}
