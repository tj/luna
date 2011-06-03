
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
      printf("integer %d\n", tok->value.as_int);
      break;
    case LUNA_TOKEN_FLOAT:
      printf("float %f\n", tok->value.as_float);
      break;
    case LUNA_TOKEN_STRING:
      printf("string '%s'\n", tok->value.as_string);
      break;
    case LUNA_TOKEN_ID:
      printf("identifier %s\n", tok->value.as_string);
      break;
    default:
      printf("%s\n", luna_token_type_string(tok->type));
      break;
  }
}
