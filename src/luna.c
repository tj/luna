
//
// luna.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "linenoise.h"
#include "luna.h"
#include "lexer.h"
#include "parser.h"
#include "errors.h"
#include "utils.h"
#include "prettyprint.h"
#include "codegen.h"
#include "vm.h"

// --ast

static int ast = 0;

// --tokens

static int tokens = 0;

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
    "\n    -A, --ast       output ast to stdout"
    "\n    -T, --tokens    output tokens to stdout"
    "\n    -h, --help      output help information"
    "\n    -V, --version   output luna version"
    "\n"
    "\n  Examples:"
    "\n"
    "\n    $ luna < some.luna"
    "\n    $ luna some.luna"
    "\n    $ luna some"
    "\n    $ luna"
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
 * Line-noise REPL.
 */

void
repl() {
  luna_set_prettyprint_func(printf);
  char *line;
  while((line = linenoise("luna> "))) {
    if ('\0' != line[0]) {
      // parse the input
      luna_lexer_t lex;
      luna_lexer_init(&lex, line, "stdin");
      luna_parser_t parser;
      luna_parser_init(&parser, &lex);
      luna_block_node_t *root;

      // oh noes!
      if (!(root = luna_parse(&parser))) {
        luna_report_error(&parser);
        exit(1);
      }

      // print
      luna_prettyprint((luna_node_t *) root);
      linenoiseHistoryAdd(line);
    }
    free(line);
  }
  exit(0);
}

/*
 * Parse arguments.
 */

const char **
parse_args(int *argc, const char **argv) {
  const char *arg, **args = argv;

  for (int i = 0, len = *argc; i < len; ++i) {
    arg = args[i];
    if (!strcmp("-h", arg) || !strcmp("--help", arg))
      usage();
    else if (!strcmp("-V", arg) || !strcmp("--version", arg))
      version();
    else if (!strcmp("-A", arg) || !strcmp("--ast", arg)) {
      ast = 1;
      --*argc; ++argv;
    } else if (!strcmp("-T", arg) || !strcmp("--tokens", arg)) {
      tokens = 1;
      --*argc; ++argv;
    } else if ('-' == arg[0]) {
      fprintf(stderr, "unknown flag %s\n", arg);
      exit(1);
    }
  }

  return argv;
}

/*
 * Evaluate `source` with the given
 * `path` name and return status.
 */

int
eval(char *source, const char *path) {
  // parse the input
  luna_lexer_t lex;
  luna_lexer_init(&lex, source, path);
  luna_parser_t parser;
  luna_parser_init(&parser, &lex);
  luna_block_node_t *root;

  // --tokens
  if (tokens) {
    while (luna_scan(&lex)) {
      printf("  \e[90m%d : \e[m", lex.lineno);
      luna_token_inspect(&lex.tok);
    }
    return 0;
  }

  // oh noes!
  if (!(root = luna_parse(&parser))) {
    luna_report_error(&parser);
    return 1;
  }

  // --ast
  luna_set_prettyprint_func(printf);
  luna_prettyprint((luna_node_t *) root);

  // evaluate
  luna_vm_t *vm = luna_gen((luna_node_t *) root);
  luna_object_t *obj = luna_eval(vm);
  luna_object_inspect(obj);
  free(obj);
  luna_vm_free(vm);
  
  return 0;
}

/*
 * Parse arguments and scan from stdin (for now).
 */

int
main(int argc, const char **argv){
  int tried_ext = 0;
  const char *path, *orig;
  char *source;

  // parse arguments
  argv = parse_args(&argc, argv);

  // eval stdin
  if (1 == argc && !isatty(0)) {
    source = read_until_eof(stdin);
    return eval(source, "stdin");
  }

  // REPL
  if (1 == argc) repl();

  // eval file
  orig = path = argv[1];
  read:
  if (!(source = file_read(path))) {
    // try with .luna extension
    if (!tried_ext) {
      tried_ext = 1;
      char buf[256];
      snprintf(buf, 256, "%s.luna", path);
      path = buf;
      goto read;
    }
    fprintf(stderr, "error reading %s:\n\n  %s\n\n", orig, strerror(errno));
    exit(1);
  }

  return eval(source, path);
}
