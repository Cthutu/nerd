//------------------------------------------------------------------------------
// LLVM runtime glue for the executable backend
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/build/front/front.h>
#include <compiler/internal.h>

//------------------------------------------------------------------------------

typedef struct {
    bool returns_void;
    bool takes_args;
} BackEndRootMainInfo;

typedef struct {
    u32 module_index;
    u32 init_function_index;
    u32 done_function_index;
} BackEndCoreLifecycleInfo;

bool back_end_llvm_runtime_write_object(cstr path);
bool back_end_llvm_runtime_write_pic_object(cstr path);
BackEndRootMainInfo
back_end_llvm_runtime_root_main_info(const FrontEndState* root);
BackEndCoreLifecycleInfo
back_end_llvm_runtime_core_lifecycle_info(const ProgramInfo* program);
string back_end_llvm_runtime_epilogue(Arena*                   arena,
                                      BackEndRootMainInfo      main_info,
                                      BackEndCoreLifecycleInfo core_lifecycle,
                                      bool                     windowed);
string back_end_llvm_runtime_render_init(Arena* arena,
                                         Array(u32) init_module_indices);
