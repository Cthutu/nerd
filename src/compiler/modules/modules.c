//------------------------------------------------------------------------------
// Module path resolution helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/modules/modules.h>
#include <stdlib.h>

//------------------------------------------------------------------------------

internal int module_hex_value(u8 ch)
{
    if (ch >= '0' && ch <= '9') {
        return (int)(ch - '0');
    }
    if (ch >= 'a' && ch <= 'f') {
        return 10 + (int)(ch - 'a');
    }
    if (ch >= 'A' && ch <= 'F') {
        return 10 + (int)(ch - 'A');
    }
    return -1;
}

cstr module_source_file_path(Arena* arena, NerdSource source)
{
    if (source.source_path.count == 0) {
        return NULL;
    }

    string prefix = s("file://");
    bool   is_uri =
        source.source_path.count >= prefix.count &&
        memcmp(source.source_path.data, prefix.data, prefix.count) == 0;
    usize start = is_uri ? prefix.count : 0;

#if OS_WINDOWS
    if (is_uri && source.source_path.count > start &&
        source.source_path.data[start] == '/') {
        start += 1;
    }
#endif

    char* out       = arena_alloc(arena, source.source_path.count - start + 1);
    usize out_count = 0;
    for (usize i = start; i < source.source_path.count; ++i) {
        if (is_uri && source.source_path.data[i] == '%' &&
            i + 2 < source.source_path.count) {
            int hi = module_hex_value(source.source_path.data[i + 1]);
            int lo = module_hex_value(source.source_path.data[i + 2]);
            if (hi >= 0 && lo >= 0) {
                out[out_count++] = (char)((hi << 4) | lo);
                i += 2;
                continue;
            }
        }

#if OS_WINDOWS
        if (is_uri && source.source_path.data[i] == '/') {
            out[out_count++] = '\\';
            continue;
        }
#endif

        out[out_count++] = (char)source.source_path.data[i];
    }

    out[out_count] = '\0';
    return out;
}

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
        cstr module_dir = path_join(
            arena, root, module_path_to_relative(arena, lexer, ast, path, ""));
        module_file = path_join(arena, module_dir, "mod.n");
    }

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

internal cstr module_qualified_to_relative(Arena* arena,
                                           string qualified_name,
                                           cstr   extension)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    for (usize i = 0; i < qualified_name.count; ++i) {
        char ch = (char)qualified_name.data[i];
        if (ch == '.') {
#if OS_WINDOWS
            ch = '\\';
#else
            ch = '/';
#endif
        }
        sb_append_char(&sb, ch);
    }
    sb_append_cstr(&sb, extension);
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

internal bool module_qualified_exists_in_root(Arena* arena,
                                              string qualified_name,
                                              cstr   root,
                                              ModuleResolveResult* out_result)
{
    cstr module_file = path_join(
        arena, root, module_qualified_to_relative(arena, qualified_name, ".n"));
    if (!path_exists(module_file) || path_is_directory(module_file)) {
        cstr module_dir =
            path_join(arena,
                      root,
                      module_qualified_to_relative(arena, qualified_name, ""));
        module_file = path_join(arena, module_dir, "mod.n");
    }

    if (!path_exists(module_file) || path_is_directory(module_file)) {
        return false;
    }

    cstr canonical_path = path_canonical(arena, module_file);
    if (canonical_path == NULL) {
        return false;
    }

    if (out_result != NULL) {
        char* data = arena_alloc(arena, qualified_name.count + 1);
        memcpy(data, qualified_name.data, qualified_name.count);
        data[qualified_name.count] = '\0';
        *out_result                = (ModuleResolveResult){
            .resolved_path  = canonical_path,
            .qualified_name = data,
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

internal bool module_qualified_exists_in_env_roots(Arena* arena,
                                                   string qualified_name,
                                                   cstr   env_value,
                                                   ModuleResolveResult* out)
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
            if (module_qualified_exists_in_root(
                    arena, qualified_name, root, out)) {
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

    cstr root_path = module_source_file_path(arena, root_source);
    if (root_path == NULL) {
        return MRS_InvalidRootSource;
    }

    cstr current_path = module_source_file_path(arena, lexer->source);
    if (current_path != NULL) {
        cstr current_dir = path_dirname(arena, current_path);
        if (module_path_exists_in_root(
                arena, lexer, ast, path, current_dir, out_result)) {
            return MRS_Found;
        }
    }

    cstr root_dir = path_dirname(arena, root_path);
    if (module_path_exists_in_root(
            arena, lexer, ast, path, root_dir, out_result)) {
        return MRS_Found;
    }

    cstr cwd = path_canonical(arena, ".");
    if (cwd != NULL &&
        module_path_exists_in_root(arena, lexer, ast, path, cwd, out_result)) {
        return MRS_Found;
    }

    cstr lib_path = getenv("NERD_LIB_PATH");
    if (module_path_exists_in_env_roots(
            arena, lexer, ast, path, lib_path, out_result)) {
        return MRS_Found;
    }

    cstr exe_dir = path_executable_dir(arena);
    if (module_path_exists_in_root(
            arena, lexer, ast, path, exe_dir, out_result)) {
        return MRS_Found;
    }

    cstr mods_dir = path_join(arena, exe_dir, "mods");
    if (module_path_exists_in_root(
            arena, lexer, ast, path, mods_dir, out_result)) {
        return MRS_Found;
    }

    return MRS_NotFound;
}

ModuleResolveStatus module_resolve_qualified(Arena*     arena,
                                             NerdSource root_source,
                                             NerdSource current_source,
                                             string     qualified_name,
                                             ModuleResolveResult* out_result)
{
    if (root_source.source_path.count == 0) {
        return MRS_InvalidRootSource;
    }

    cstr root_path = module_source_file_path(arena, root_source);
    if (root_path == NULL) {
        return MRS_InvalidRootSource;
    }

    cstr current_path = module_source_file_path(arena, current_source);
    if (current_path != NULL) {
        cstr current_dir = path_dirname(arena, current_path);
        if (module_qualified_exists_in_root(
                arena, qualified_name, current_dir, out_result)) {
            return MRS_Found;
        }
    }

    cstr root_dir = path_dirname(arena, root_path);
    if (module_qualified_exists_in_root(
            arena, qualified_name, root_dir, out_result)) {
        return MRS_Found;
    }

    cstr cwd = path_canonical(arena, ".");
    if (cwd != NULL && module_qualified_exists_in_root(
                           arena, qualified_name, cwd, out_result)) {
        return MRS_Found;
    }

    cstr lib_path = getenv("NERD_LIB_PATH");
    if (module_qualified_exists_in_env_roots(
            arena, qualified_name, lib_path, out_result)) {
        return MRS_Found;
    }

    cstr exe_dir = path_executable_dir(arena);
    if (module_qualified_exists_in_root(
            arena, qualified_name, exe_dir, out_result)) {
        return MRS_Found;
    }

    cstr mods_dir = path_join(arena, exe_dir, "mods");
    if (module_qualified_exists_in_root(
            arena, qualified_name, mods_dir, out_result)) {
        return MRS_Found;
    }

    return MRS_NotFound;
}

//------------------------------------------------------------------------------
