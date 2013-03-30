
//
// opcodes.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_OPCODES_H__
#define __LUNA_OPCODES_H__

/*
 * Opcodes.
 */

typedef enum {
  LUNA_OP_HALT,
  LUNA_OP_JMP,
  LUNA_OP_LOADK,
  LUNA_OP_LOADB,
  LUNA_OP_MOVE,
  LUNA_OP_EQ,
  LUNA_OP_LT,
  LUNA_OP_LTE,
  LUNA_OP_ADD,
  LUNA_OP_SUB,
  LUNA_OP_DIV,
  LUNA_OP_MUL,
  LUNA_OP_MOD,
  LUNA_OP_BIT_SHL,
  LUNA_OP_BIT_SHR,
  LUNA_OP_BIT_AND,
  LUNA_OP_BIT_OR,
  LUNA_OP_BIT_XOR,
  LUNA_OP_NEGATE
} luna_op_t;

#endif /* __LUNA_OPCODES_H__ */
