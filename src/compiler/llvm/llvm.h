//------------------------------------------------------------------------------
// LLVM IR emission from HIR
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/hir/hir.h>

//------------------------------------------------------------------------------

string llvm_render_hir(const Hir*   hir,
                       const Lexer* lexer,
                       const Sema*  sema,
                       Arena*       arena,
                       bool         emit_debug);
bool   llvm_save_hir(const Hir*   hir,
                     const Lexer* lexer,
                     const Sema*  sema,
                     cstr         path);

//------------------------------------------------------------------------------
