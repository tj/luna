#pragma once

//
// codegen.h
//
// Copyright (c) 2013 TJ Holowaychuk <tj@vision-media.ca>
//

#include "ast.h"
#include "vm.h"

// protos

luna_vm_t *
luna_gen(luna_node_t *node);
