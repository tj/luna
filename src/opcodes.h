
//
// opcodes.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_OPCODES_H_H
#define LUNA_OPCODES_H_H

/*
 * Opcodes.
 */

#define LUNA_OP_LIST \
  o(HALT, "halt") \
  o(JMP, "jmp") \
  o(LOADK, "loadk") \
  o(LOADB, "loadb") \
  o(MOVE, "move") \
  o(EQ, "eq") \
  o(LT, "lt") \
  o(LTE, "lte") \
  o(ADD, "add") \
  o(SUB, "sub") \
  o(DIV, "div") \
  o(MUL, "mul") \
  o(MOD, "mod") \
  o(POW, "pow") \
  o(NEGATE, "negate") \
  o(BIT_SHL, "bshl") \
  o(BIT_SHR, "bshr") \
  o(BIT_AND, "band") \
  o(BIT_OR, "bor") \
  o(BIT_XOR, "bxor")

/*
 * Opcodes enum.
 */

typedef enum {
#define o(op, str) LUNA_OP_##op,
LUNA_OP_LIST
#undef o
} luna_op_t;

/*
 * Opcode strings.
 */

static char *luna_op_strings[] = {
#define o(op, str) str,
LUNA_OP_LIST
#undef o
};

#endif /* LUNA_OPCODES_H_H */
