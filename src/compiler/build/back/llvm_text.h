//------------------------------------------------------------------------------
// LLVM text utilities for the executable backend
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/internal.h>

//------------------------------------------------------------------------------

string back_end_llvm_text_build_combined(Arena* arena,
                                         string runtime_prelude,
                                         Array(string) module_llvms,
                                         string runtime_epilogue,
                                         string init_ll);

bool back_end_llvm_text_self_test(void);
