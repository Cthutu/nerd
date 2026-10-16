//------------------------------------------------------------------------------
// LLVM IR emission from HIR
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/hir/hir.h>

//------------------------------------------------------------------------------

bool llvm_function_names_self_test(void);
bool llvm_render_sema_self_test(void);
bool llvm_lowering_failure_self_test(void);
// Physical storage units in layout order; entries are logical field indices.
// The caller owns the returned array. Shared with C emission.
Array(u32) llvm_record_storage_order(const Sema* sema, u32 type_index);
void llvm_index_function_names(const ProgramInfo* program, Map* counts);

string llvm_render_hir(const Hir*   hir,
                       const Lexer* lexer,
                       const Sema*  sema,
                       Arena*       arena,
                       bool         emit_debug,
                       bool         export_c_symbols);
bool   llvm_save_hir(const Hir*   hir,
                     const Lexer* lexer,
                     const Sema*  sema,
                     cstr         path);

//------------------------------------------------------------------------------
