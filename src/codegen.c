
//
// codegen.c
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include "ast.h"
#include "codegen.h"
#include "internal.h"
#include "visitor.h"
#include "opcodes.h"

// TODO: MSB
#define CONST(val) \
  (vm->main->constants[vm->main->nconstants] = val, 32 + vm->main->nconstants++)

/*
 * Emit an instruction.
 */

#define emit(op, a, b, c) \
  *vm->main->code++ = ABC(op, a, b, c);

/*
 * Emit binary operation.
 */

static void
emit_op(luna_vm_t *vm, luna_binary_op_node_t *node, int l, int r) {
  switch (node->op) {
    case LUNA_TOKEN_OP_PLUS:
      emit(ADD, 0, l, r);
      break;
    case LUNA_TOKEN_OP_MINUS:
      emit(SUB, 0, l, r);
      break;
    case LUNA_TOKEN_OP_DIV:
      emit(DIV, 0, l, r);
      break;
    case LUNA_TOKEN_OP_MUL:
      emit(MUL, 0, l, r);
      break;
    case LUNA_TOKEN_OP_MOD:
      emit(MOD, 0, l, r);
      break;
    case LUNA_TOKEN_OP_LT:
      emit(LT, 0, l, r);
      emit(JMP, 0, 1, 0);
      emit(LOADB, 0, CONST(1), 1);
      emit(LOADB, 0, CONST(0), 0);
      break;
    case LUNA_TOKEN_OP_LTE:
      emit(LTE, 0, l, r);
      emit(JMP, 0, 1, 0);
      emit(LOADB, 0, CONST(1), 1);
      emit(LOADB, 0, CONST(0), 0);
      break;
  }
}


/*

0: lt 5 2;
 : false1:
 : ...
 : ...
 : true1:
 : ...
 : ...
 : endif1:
 : ...
 : ...
 : ...
*/


/*
 * Visit block `node`.
 */

static void
visit_block(luna_visitor_t *self, luna_block_node_t *node) {
  luna_vec_each(node->stmts, {
    visit((luna_node_t *) val->value.as_pointer);
  });
}

/*
 * Visit int `node`.
 */

static void
visit_int(luna_visitor_t *self, luna_int_node_t *node) {
  // printf("(int %d)", node->val);
}

/*
 * Visit float `node`.
 */

static void
visit_float(luna_visitor_t *self, luna_float_node_t *node) {
  luna_vm_t *vm = (luna_vm_t *) self->data;
}

/*
 * Visit id `node`.
 */

static void
visit_id(luna_visitor_t *self, luna_id_node_t *node) {
  // printf("(id %s)", node->val);
}

/*
 * Visit decl `node`.
 */

static void
visit_decl(luna_visitor_t *self, luna_decl_node_t *node) {
  // printf("(decl %s:%s", node->name, node->type ? node->type : "");
  // if (node->val) visit(node->val);
  // printf(")");
}

/*
 * Visit string `node`.
 */

static void
visit_string(luna_visitor_t *self, luna_string_node_t *node) {
  // printf("(string '%s')", inspect(node->val));
}

/*
 * Visit unary op `node`.
 */

static void
visit_unary_op(luna_visitor_t *self, luna_unary_op_node_t *node) {
  luna_vm_t *vm = (luna_vm_t *) self->data;
  visit(node->expr);
  emit(NEGATE, 0, 0, 0);
}

/*
 * Visit binary op `node`.
 */

static void
visit_binary_op(luna_visitor_t *self, luna_binary_op_node_t *node) {
  luna_vm_t *vm = (luna_vm_t *) self->data;
  if (LUNA_NODE_BINARY_OP == node->left->type) {
    visit(node->left);
    int r = CONST(((luna_float_node_t *) node->right)->val);
    emit_op(vm, node, 0, r);
  } else {
    int l = CONST(((luna_float_node_t *) node->left)->val);
    int r = CONST(((luna_float_node_t *) node->right)->val);
    emit_op(vm, node, l, r);
  }
}

/*
 * Visit array `node`.
 */

static void
visit_array(luna_visitor_t *self, luna_array_node_t *node) {
  // printf("(array\n");
  // ++indents;
  // luna_vec_each(node->vals, {
  //   INDENT;
  //   visit((luna_node_t *) val->value.as_pointer);
  //   if (i != len - 1) printf("\n");
  // });
  // --indents;
  // printf(")");
}

/*
 * Visit hash `node`.
 */

static void
visit_hash(luna_visitor_t *self, luna_hash_node_t *node) {
  // printf("(hash\n");
  // ++indents;
  // luna_hash_each(node->vals, {
  //   INDENT;
  //   printf("%s: ", slot);
  //   visit((luna_node_t *) val->value.as_pointer);
  //   printf("\n");
  // });
  // --indents;
  // printf(")");
}

/*
 * Visit subscript `node`.
 */

static void
visit_subscript(luna_visitor_t *self, luna_subscript_node_t *node) {

}

/*
 * Visit slot `node`.
 */

static void
visit_slot(luna_visitor_t *self, luna_slot_node_t *node) {
  // printf("(slot\n");
  // ++indents;
  // INDENT;
  // visit(node->left);
  // printf("\n");
  // INDENT;
  // visit(node->right);
  // --indents;
  // printf(")");
}

/*
 * Visit call `node`.
 */

static void
visit_call(luna_visitor_t *self, luna_call_node_t * node) {
  // printf("(call\n");
  // ++indents;
  // INDENT;
  // visit((luna_node_t *) node->expr);
  // if (luna_vec_length(node->args->vec)) {
  //   printf("\n");
  //   INDENT;
  //   luna_vec_each(node->args->vec, {
  //     visit((luna_node_t *) val->value.as_pointer);
  //     if (i != len - 1) printf(" ");
  //   });

  //   luna_hash_each(node->args->hash, {
  //     printf(" %s: ", slot);
  //     visit((luna_node_t *) val->value.as_pointer);
  //   });
  // }
  // --indents;
  // printf(")");
}

/*
 * Visit function `node`.
 */

static void
visit_function(luna_visitor_t *self, luna_function_node_t * node) {
  // printf("(function %s -> %s", node->name, node->type ? node->type : "");
  // ++indents;
  // luna_vec_each(node->params, {
  //   printf("\n");
  //   INDENT;
  //   visit((luna_node_t *) val->value.as_pointer);
  // });
  // --indents;
  // printf("\n");
  // ++indents;
  // visit((luna_node_t *) node->block);
  // --indents;
  // printf(")");
}

/*
 * Visit `while` node.
 */

static void
visit_while(luna_visitor_t *self, luna_while_node_t *node) {
  // // while | until
  // printf("(%s ", node->negate ? "until" : "while");
  // visit((luna_node_t *) node->expr);
  // ++indents;
  // printf("\n");
  // visit((luna_node_t *) node->block);
  // --indents;
  // printf(")\n");
}

/*
 * Visit `return` node.
 */

static void
visit_return(luna_visitor_t *self, luna_return_node_t *node) {
  // printf("(return");
  // if (node->expr) {
  //   ++indents;
  //   printf("\n");
  //   INDENT;
  //   visit((luna_node_t *) node->expr);
  //   --indents;
  // }
  // printf(")");
}

/*
 * Visit if `node`.
 */

static void
visit_if(luna_visitor_t *self, luna_if_node_t *node) {
  // // if
  // printf("(%s ", node->negate ? "unless" : "if");
  // visit((luna_node_t *) node->expr);
  // ++indents;
  // printf("\n");
  // visit((luna_node_t *) node->block);
  // --indents;
  // printf(")");

  // // else ifs
  // luna_vec_each(node->else_ifs, {
  //   luna_if_node_t *else_if = (luna_if_node_t *) val->value.as_pointer;
  //   printf("\n");
  //   INDENT;
  //   printf("(else if ");
  //   visit((luna_node_t *) else_if->expr);
  //   ++indents;
  //   printf("\n");
  //   visit((luna_node_t *) else_if->block);
  //   --indents;
  //   printf(")");
  // });

  // // else
  // if (node->else_block) {
  //   printf("\n");
  //   INDENT;
  //   printf("(else\n");
  //   ++indents;
  //   visit((luna_node_t *) node->else_block);
  //   --indents;
  //   printf(")");
  // }
}

/*
 * Visit `type` node.
 */

static void
visit_type(luna_visitor_t *self, luna_type_node_t *node) {

}

/*
 * Visit use `node`.
 */

static void
visit_use(luna_visitor_t *self, luna_use_node_t *node) {

}

/*
 * Generate code for the given `node`.
 */

luna_vm_t *
luna_gen(luna_node_t *node) {
  luna_vm_t *vm = malloc(sizeof(luna_vm_t));
  if (!vm) return NULL;
  vm->main = malloc(sizeof(luna_activation_t));
  vm->main->nconstants = 0;
  vm->main->constants = malloc(1024 * sizeof(int)); // TODO: vec / objects
  vm->main->ip = vm->main->code = malloc(64 * 1024);

  luna_visitor_t visitor = {
    .data = (void *) vm,
    .visit_if = visit_if,
    .visit_id = visit_id,
    .visit_int = visit_int,
    .visit_slot = visit_slot,
    .visit_call = visit_call,
    .visit_hash = visit_hash,
    .visit_array = visit_array,
    .visit_while = visit_while,
    .visit_block = visit_block,
    .visit_decl = visit_decl,
    .visit_float = visit_float,
    .visit_string = visit_string,
    .visit_return = visit_return,
    .visit_function = visit_function,
    .visit_unary_op = visit_unary_op,
    .visit_binary_op = visit_binary_op,
    .visit_subscript = visit_subscript,
    .visit_type = visit_type,
    .visit_use = visit_use
  };

  luna_visit(&visitor, node);
  emit(HALT, 0, 0, 0);
  
  // Reset code so we can free it later
  vm->main->code = vm->main->ip;
  return vm;
}
