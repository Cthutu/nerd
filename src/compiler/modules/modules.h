//------------------------------------------------------------------------------
// Module path resolution helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>

//------------------------------------------------------------------------------

typedef enum : u8 {
    MRS_NotFound,
    MRS_Found,
    MRS_InvalidRootSource,
} ModuleResolveStatus;

typedef struct {
    cstr resolved_path;
    cstr qualified_name;
} ModuleResolveResult;

bool                module_path_is(const Lexer*         lexer,
                                   const Ast*           ast,
                                   const AstModulePath* path,
                                   string               first,
                                   string               second);
cstr                module_path_to_relative(Arena*               arena,
                                            const Lexer*         lexer,
                                            const Ast*           ast,
                                            const AstModulePath* path,
                                            cstr                 extension);
cstr                module_path_to_qualified_name(Arena*               arena,
                                                  const Lexer*         lexer,
                                                  const Ast*           ast,
                                                  const AstModulePath* path);
ModuleResolveStatus module_resolve_path(Arena*               arena,
                                        NerdSource           root_source,
                                        const Lexer*         lexer,
                                        const Ast*           ast,
                                        const AstModulePath* path,
                                        ModuleResolveResult* out_result);

//------------------------------------------------------------------------------
