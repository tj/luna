
//
// vm.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include "vm.h"
#include "object.h"
#include "opcodes.h"
#include "internal.h"

/*
 * Register n.
 */

#define R(n) registers[n]

/*
 * Constant n.
 */

#define K(n) vm->main->constants[(n) - 32]

/*
 * Register or constant.
 */

// TODO: MSB
#define RK(n) \
   ((n) < 32 ? R(n) : K(n))

#include <stdio.h>

void
luna_dump(luna_vm_t *vm) {
  luna_instruction_t *ip = vm->main->ip;
  luna_instruction_t i;
  int registers[32] = {0};

  for (;;) {
    switch (OP(i = *ip++)) {
      // LOADK
      case LUNA_OP_LOADK:
        printf("loadk %d %d; %d\n", A(i), B(i), K(B(i)));
        break;

      // LOADB
      case LUNA_OP_LOADB:
        printf("loadb %d %d; %d\n", A(i), B(i), K(B(i)));
        break;

      // ADD
      case LUNA_OP_ADD:
        printf("add %d %d %d; %d %d\n", A(i), B(i), C(i), RK(B(i)), RK(C(i)));
        break;

      // SUB
      case LUNA_OP_SUB:
        printf("sub %d %d %d; %d %d\n", A(i), B(i), C(i), RK(B(i)), RK(C(i)));
        break;

      // DIV
      case LUNA_OP_DIV:
        printf("div %d %d %d; %d %d\n", A(i), B(i), C(i), RK(B(i)), RK(C(i)));
        break;

      // MUL
      case LUNA_OP_MUL:
        printf("mul %d %d %d; %d %d\n", A(i), B(i), C(i), RK(B(i)), RK(C(i)));
        break;

      // MOD
      case LUNA_OP_MOD:
        printf("mod %d %d %d; %d %d\n", A(i), B(i), C(i), RK(B(i)), RK(C(i)));
        break;

      // NEGATE
      case LUNA_OP_NEGATE:

        break;

      // LT
      case LUNA_OP_LT:
        printf("lt %d %d; %d %d\n", B(i), C(i), RK(B(i)), RK(C(i)));
        break;

      // LTE
      case LUNA_OP_LTE:
        printf("lte %d %d; %d %d\n", B(i), C(i), RK(B(i)), RK(C(i)));
        break;

      // JMP
      case LUNA_OP_JMP:
        printf("jmp %d\n", B(i));
        break;

      // HALT
      case LUNA_OP_HALT:
        printf("halt\n");
        return;
    }
  }
}

luna_object_t *
luna_eval(luna_vm_t *vm) {
  luna_dump(vm);
  printf("\n");
  luna_instruction_t *ip = vm->main->ip;
  luna_instruction_t i;
  int registers[32] = {0};

  for (;;) {
    switch (OP(i = *ip++)) {
      // LOADK
      case LUNA_OP_LOADK:
        printf("loadk %d\n", K(B(i)));
        R((A(i))) = K(B(i));
        break;

      // LOADB
      case LUNA_OP_LOADB:
        printf("loadb %d %d %d\n", A(i), K(B(i)), C(i));
        R(A(i)) = K(B(i));
        if (C(i)) ip++;
        break;

      // ADD
      case LUNA_OP_ADD:
        R(A(i)) = RK(B(i)) + RK(C(i));
        break;

      // SUB
      case LUNA_OP_SUB:
        R(A(i)) = RK(B(i)) - RK(C(i));
        break;

      // DIV
      case LUNA_OP_DIV:
        R(A(i)) = RK(B(i)) / RK(C(i));
        break;

      // MUL
      case LUNA_OP_MUL:
        R(A(i)) = RK(B(i)) * RK(C(i));
        break;

      // MOD
      case LUNA_OP_MOD:
        R(A(i)) = RK(B(i)) % RK(C(i));
        break;

      // NEGATE
      case LUNA_OP_NEGATE:
        R(A(i)) = -R(B(i));
        break;

      // LT
      case LUNA_OP_LT:
        printf("lt %d %d\n", RK(B(i)), RK(C(i)));
        if (RK(B(i)) < RK(C(i))) ip++;
        break;

      // LTE
      case LUNA_OP_LTE:
        printf("lte %d %d\n", RK(B(i)), RK(C(i)));
        if (RK(B(i)) <= RK(C(i))) ip++;
        break;

      // JMP
      case LUNA_OP_JMP:
        printf("jmp %d\n", B(i));
        ip += B(i);
        break;

      // HALT
      case LUNA_OP_HALT:
        goto end;
    }
  }

end:
  return luna_int_new(R(0));
}
