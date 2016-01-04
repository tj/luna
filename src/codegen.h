
//
// codegen.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LUNA_CODE_H
#define LUNA_CODE_H

#include "ast.h"
#include "vm.h"

// protos

luna_vm_t *
luna_gen(luna_node_t *node);

#endif /* LUNA_CODE_H */
