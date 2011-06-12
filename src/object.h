
//
// object.h
//
// Copyright (c) 2011 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_OBJECT_H__
#define __LUNA_OBJECT_H__

/*
 * Check if `obj` is the given type.
 */

#define luna_object_is(obj, t) ((obj).type == LUNA_TYPE_##t)

/*
 * Specific type macros.
 */

#define luna_is_list(obj) luna_object_is(obj, LIST)
#define luna_is_array(obj) luna_object_is(obj, ARRAY)
#define luna_is_object(obj) luna_object_is(obj, OBJECT)
#define luna_is_string(obj) luna_object_is(obj, STRING)
#define luna_is_float(obj) luna_object_is(obj, FLOAT)
#define luna_is_int(obj) luna_object_is(obj, INT)
#define luna_is_bool(obj) luna_object_is(obj, BOOL)
#define luna_is_null(obj) luna_object_is(obj, NULL)

/*
 * Luna object types.
 */

typedef enum {
    LUNA_TYPE_NULL
  , LUNA_TYPE_BOOL
  , LUNA_TYPE_INT
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
    void *pointer;
    int as_int;
    float as_float;
  } val;
} luna_object_t;

#endif /* __LUNA_OBJECT_H__ */