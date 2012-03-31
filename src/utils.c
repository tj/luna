
//
// utils.h
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

/*
 * Return the filesize of `filename` or -1.
 */

off_t
file_size(const char *filename) {
  struct stat s;
  if (stat(filename, &s) < 0) return -1;
  return s.st_size;
}

/*
 * Read the contents of `filename` or return NULL.
 */

char *
file_read(const char *filename) {
  off_t len = file_size(filename);
  if (len < 0) return NULL;

  char *buf = malloc(len + 1);
  if (!buf) return NULL;

  int fd = open(filename, O_RDONLY);
  if (fd < 0) return NULL;

  ssize_t size = read(fd, buf, len);
  if (size != len) return NULL;

  return buf;
}