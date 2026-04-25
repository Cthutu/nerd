//------------------------------------------------------------------------------
// Module path resolution helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/modules/modules.h>
#include <stdlib.h>

//------------------------------------------------------------------------------

internal bool module_path_exists_in_root(Arena*               arena,
                                         const Lexer*         lexer,
                                         const Ast*           ast,
                                         const AstModulePath* path,
                                         cstr                 root,
                                         ModuleResolveResult* out_result)
{
    cstr module_file = path_join(
        arena, root, module_path_to_relative(arena, lexer, ast, path, ".n"));
    if (!path_exists(module_file) || path_is_directory(module_file)) {
        return false;
    }

    cstr canonical_path = path_canonical(arena, module_file);
    if (canonical_path == NULL) {
        return false;
    }

    if (out_result != NULL) {
        *out_result = (ModuleResolveResult){
            .resolved_path = canonical_path,
            .qualified_name =
                module_path_to_qualified_name(arena, lexer, ast, path),
        };
    }
    return true;
}

internal bool module_path_exists_in_env_roots(Arena*               arena,
                                              const Lexer*         lexer,
                                              const Ast*           ast,
                                              const AstModulePath* path,
                                              cstr                 env_value,
                                              ModuleResolveResult* out_result)
{
    if (env_value == NULL || *env_value == '\0') {
        return false;
    }

#if OS_WINDOWS
    char separator = ';';
#else
    char separator = ':';
#endif

    const char* cursor = env_value;
    while (*cursor != '\0') {
        const char* end = strchr(cursor, separator);
        usize       len = end != NULL ? (usize)(end - cursor) : strlen(cursor);
        if (len > 0) {
            char* root = (char*)arena_alloc(arena, len + 1);
            memcpy(root, cursor, len);
            root[len] = '\0';
            if (module_path_exists_in_root(
                    arena, lexer, ast, path, root, out_result)) {
                return true;
            }
        }

        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }

    return false;
}

bool module_path_is(const Lexer*         lexer,
                    const Ast*           ast,
                    const AstModulePath* path,
                    string               first,
                    string               second)
{
    if (path->symbol_count != 2) {
        return false;
    }

    return string_eq(
               lex_symbol(lexer, ast->module_path_symbols[path->first_symbol]),
               first) &&
           string_eq(
               lex_symbol(lexer,
                          ast->module_path_symbols[path->first_symbol + 1]),
               second);
}

cstr module_path_to_relative(Arena*               arena,
                             const Lexer*         lexer,
                             const Ast*           ast,
                             const AstModulePath* path,
                             cstr                 extension)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    for (u32 i = 0; i < path->symbol_count; ++i) {
        if (i > 0) {
#if OS_WINDOWS
            sb_append_char(&sb, '\\');
#else
            sb_append_char(&sb, '/');
#endif
        }
        sb_append_string(
            &sb,
            lex_symbol(lexer,
                       ast->module_path_symbols[path->first_symbol + i]));
    }

    sb_append_cstr(&sb, extension);
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

cstr module_path_to_qualified_name(Arena*               arena,
                                   const Lexer*         lexer,
                                   const Ast*           ast,
                                   const AstModulePath* path)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    for (u32 i = 0; i < path->symbol_count; ++i) {
        if (i > 0) {
            sb_append_char(&sb, '.');
        }
        sb_append_string(
            &sb,
            lex_symbol(lexer,
                       ast->module_path_symbols[path->first_symbol + i]));
    }

    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

ModuleResolveStatus module_resolve_path(Arena*               arena,
                                        NerdSource           root_source,
                                        const Lexer*         lexer,
                                        const Ast*           ast,
                                        const AstModulePath* path,
                                        ModuleResolveResult* out_result)
{
    if (root_source.source_path.count == 0) {
        return MRS_InvalidRootSource;
    }

    char* root_path =
        (char*)arena_alloc(arena, root_source.source_path.count + 1);
    memcpy(
        root_path, root_source.source_path.data, root_source.source_path.count);
    root_path[root_source.source_path.count] = '\0';

    cstr root_dir                            = path_dirname(arena, root_path);
    if (module_path_exists_in_root(
            arena, lexer, ast, path, root_dir, out_result)) {
        return MRS_Found;
    }

    cstr lib_path = getenv("NERD_LIB_PATH");
    if (module_path_exists_in_env_roots(
            arena, lexer, ast, path, lib_path, out_result)) {
        return MRS_Found;
    }

    cstr exe_dir  = path_executable_dir(arena);
    cstr mods_dir = path_join(arena, exe_dir, "mods");
    if (module_path_exists_in_root(
            arena, lexer, ast, path, mods_dir, out_result)) {
        return MRS_Found;
    }

    return MRS_NotFound;
}

//------------------------------------------------------------------------------
