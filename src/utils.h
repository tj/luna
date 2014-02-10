#pragma once

//
// utils.h
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

#include <sys/stat.h>

off_t
file_size(const char *filename);

char *
file_read(const char *filename);

char *
read_until_eof(FILE *stream);
