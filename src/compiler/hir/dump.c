//------------------------------------------------------------------------------
// HIR dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/hir/hir.h>
#include <stdio.h>

//------------------------------------------------------------------------------

internal cstr hir_function_kind_name(HirFunctionKind kind)
{
    switch (kind) {
    case HIR_FUNCTION_Normal:
        return "fn";
    case HIR_FUNCTION_Ffi:
        return "ffi";
    case HIR_FUNCTION_GenericInstantiation:
        return "generic-inst";
    default:
        return "unknown";
    }
}

string hir_render(const Hir* hir, const Lexer* lexer, Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    sb_append_cstr(&sb, "hir\n");
    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        const HirFunction* function = &hir->functions[i];
        sb_format(&sb, "  %s ", hir_function_kind_name(function->kind));
        if (function->symbol_handle != U32_MAX) {
            sb_append_string(&sb, lex_symbol(lexer, function->symbol_handle));
        } else {
            sb_append_cstr(&sb, "<anonymous>");
        }
        sb_format(&sb,
                  " decl=%u node=%u scope=%u type=%u",
                  function->decl_index,
                  function->fn_node_index,
                  function->root_scope_index,
                  function->type_index);

        sb_append_cstr(&sb, " params=(");
        for (u32 p = 0; p < function->param_count; ++p) {
            if (p > 0) {
                sb_append_cstr(&sb, ", ");
            }
            const HirParam* param = &hir->params[function->first_param + p];
            if (param->symbol_handle != U32_MAX) {
                sb_append_string(&sb, lex_symbol(lexer, param->symbol_handle));
            } else {
                sb_append_cstr(&sb, "<anonymous>");
            }
            sb_format(&sb, ":type%u", param->type_index);
        }
        sb_append_cstr(&sb, ")\n");
    }

    return sb_to_string(&sb);
}

void hir_dump(const Hir* hir, const Lexer* lexer)
{
    Arena arena = {0};
    arena_init(&arena);
    epr(STRINGP, STRINGV(hir_render(hir, lexer, &arena)));
    arena_done(&arena);
}

bool hir_save(const Hir* hir, const Lexer* lexer, cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = hir_render(hir, lexer, &arena);

    FILE* file      = fopen(path, "wb");
    if (!file) {
        arena_done(&arena);
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(rendered.data, 1, rendered.count, file);
    bool  close_failed = fclose(file) != 0;
    arena_done(&arena);

    if (written != rendered.count || close_failed) {
        return error_runtime("Failed to write HIR file: %s", path);
    }
    return true;
}

//------------------------------------------------------------------------------
