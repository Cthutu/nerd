//------------------------------------------------------------------------------
// LLVM runtime glue for the executable backend
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/build/front/front.h>
#include <compiler/internal.h>

//------------------------------------------------------------------------------

string back_end_llvm_runtime_prelude(void);
bool   back_end_llvm_runtime_hir_has_globals(const Hir* hir);
bool   back_end_llvm_runtime_root_main_returns_void(const FrontEndState* root);
string back_end_llvm_runtime_epilogue(bool root_main_returns_void);
string back_end_llvm_runtime_render_init(Arena* arena,
                                         Array(u32) init_module_indices);
