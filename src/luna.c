
//
// luna.c
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "luna.h"
#include "lexer.h"
#include "parser.h"

/*
 * Output usage information.
 */

void
usage() {
  fprintf(stderr, 
    "\n  Usage: luna [options] [file]"
    "\n"
    "\n  Options:"
    "\n"
    "\n    -h, --help      output help information"
    "\n    -V, --version   output luna version"
    "\n"
    "\n"
    );
  exit(1);
}

/*
 * Output luna version.
 */

void
version() {
  printf("%s\n", LUNA_VERSION);
  exit(0);
}

/*
 * Parse arguments.
 */

void
parse_args(int argc, const char **argv) {
  const char *arg;
  for (int i = 0; i < argc; ++i) {
    arg = argv[i];
    if (0 == strcmp(arg, "-h") || 0 == strcmp(arg, "--help"))
      usage();
    else if (0 == strcmp(arg, "-V") || 0 == strcmp(arg, "--version"))
      version();
    else if ('-' == arg[0]) {
      fprintf(stderr, "unknown flag %s\n", arg);
      exit(1);
    }
  }
}

/*
 * Parse arguments and scan from stdin (for now).
 */

int
main(int argc, const char **argv){
  FILE *stream;
  const char *path;

  // parse arguments
  parse_args(argc, argv);

  // file given
  if (argc > 1) {
    path = argv[1];
    stream = fopen(path, "r");
    if (!stream) {
      fprintf(stderr, "error reading %s:\n\n  %s\n\n", path, strerror(errno));
      exit(1);
    }
  } else {
    stream = stdin;
    path = "stdin";
  }

  // parser the input
  luna_lexer_t lex;
  luna_lexer_init(&lex, stream, path);
  luna_parser_t parser;
  luna_parser_init(&parser, &lex);

  // oh noes!
  if (!luna_parse(&parser)) {
    // deduce
    if (!parser.error && LUNA_TOKEN_EOS != lex.tok.type) {
      char buf[256];
      snprintf(buf, 256, "unexpected token '%s'", luna_token_type_string(lex.tok.type));
      parser.error = buf;
    }

    fprintf(stderr
      , "luna(%s:%d). parse error: %s\n"
      , lex.filename
      , lex.lineno
      , parser.error);

    exit(1);
  }

  return 0;
}