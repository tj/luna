
//
// disasm.h
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef __LUNA_DISASM__
#define __LUNA_DISASM__

#include <stdio.h>
#include "opcodes.h"
#include "vm.h"

/*
 * Dump disassembled program to stdout.
 *
 * TODO: return a string
 */

void
luna_dump(luna_vm_t *vm) {
  luna_instruction_t *ip = vm->main->ip;
  luna_instruction_t i;
  int registers[32] = {0};

  for (;;) {
    i = *ip++;
    printf("%10s ", luna_op_strings[OP(i)]);
    switch (OP(i)) {
      // -
      case LUNA_OP_HALT:
        printf("\n");
        return;

      // op : sBx
      case LUNA_OP_JMP:
        printf("%d\n", B(i));
        break;

      // op : R(A) RK(B)
      case LUNA_OP_LOADK:
      case LUNA_OP_LOADB:
        printf("%d %d; %d\n", A(i), B(i), K(B(i)));
        break;

      // op : R(A) RK(B) RK(C)
      case LUNA_OP_ADD:
      case LUNA_OP_SUB:
      case LUNA_OP_DIV:
      case LUNA_OP_MUL:
      case LUNA_OP_MOD:
      case LUNA_OP_POW:
      case LUNA_OP_LT:
      case LUNA_OP_LTE:
        printf("%d %d %d; %d %d\n", A(i), B(i), C(i), RK(B(i)), RK(C(i)));
        break;
    }
  }
}

#endif
