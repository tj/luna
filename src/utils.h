
//
// utils.h
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_UTIL_H
#define LUNA_UTIL_H

#include <sys/stat.h>

size_t
file_size(FILE *handle);

char *
file_read(const char *filename);

char *
read_until_eof(FILE *stream);

#endif