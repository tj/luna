
//
// utils.h
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_UTIL__
#define __LUNA_UTIL__

#include <sys/stat.h>

off_t
file_size(const char *filename);

char *
file_read(const char *filename);

char *
read_until_eof(FILE *stream);

#endif