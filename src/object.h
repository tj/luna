
//
// object.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_OBJECT_H__
#define __LUNA_OBJECT_H__

/*
 * Luna object types.
 */

typedef enum {
    LUNA_TYPE_INT
  , LUNA_TYPE_FLOAT
  , LUNA_TYPE_STRING
  , LUNA_TYPE_OBJECT
  , LUNA_TYPE_ARRAY
  , LUNA_TYPE_LIST
} luna_object;

/*
 * Luna object.
 */

typedef struct {
  luna_object type;
  union {
    void *as_pointer;
    int as_int;
    float as_float;
  } val;
} luna_object_t;

#endif /* __LUNA_OBJECT_H__ */