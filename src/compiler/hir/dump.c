//------------------------------------------------------------------------------
// HIR dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/hir/hir.h>
#include <stdio.h>

//------------------------------------------------------------------------------

internal cstr hir_function_prefix(HirFunctionKind kind)
{
    switch (kind) {
    case HIR_FUNCTION_Normal:
        return "func";
    case HIR_FUNCTION_Ffi:
        return "extern func";
    case HIR_FUNCTION_GenericInstantiation:
        return "inst func";
    default:
        return "func";
    }
}

internal string hir_function_name(const HirFunction* function,
                                  const Lexer*       lexer)
{
    if (function->symbol_handle == U32_MAX) {
        return s("<anonymous>");
    }
    return lex_symbol(lexer, function->symbol_handle);
}

internal string hir_type_name(const Lexer* lexer,
                              const Sema*  sema,
                              Arena*       arena,
                              u32          type_index)
{
    if (!sema || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        return s("<unknown>");
    }
    return sema_type_name(lexer, sema, arena, type_index);
}

internal string hir_function_return_type_name(const HirFunction* function,
                                              const Lexer*       lexer,
                                              const Sema*        sema,
                                              Arena*             arena)
{
    if (!sema || function->type_index == sema_no_type() ||
        function->type_index >= array_count(sema->types) ||
        sema->types[function->type_index].kind != STK_Function) {
        return s("<unknown>");
    }
    return hir_type_name(
        lexer, sema, arena, sema->types[function->type_index].return_type);
}

string
hir_render(const Hir* hir, const Lexer* lexer, const Sema* sema, Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    sb_append_cstr(&sb, "hir 0\n");
    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        const HirFunction* function = &hir->functions[i];
        sb_format(&sb, "%s ", hir_function_prefix(function->kind));
        sb_append_string(&sb, hir_function_name(function, lexer));
        sb_append_char(&sb, '(');
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
            sb_append_cstr(&sb, ": ");
            sb_append_string(
                &sb, hir_type_name(lexer, sema, arena, param->type_index));
        }
        sb_append_cstr(&sb, ") -> ");
        sb_append_string(
            &sb, hir_function_return_type_name(function, lexer, sema, arena));
        sb_append_char(&sb, '\n');
    }

    return sb_to_string(&sb);
}

void hir_dump(const Hir* hir, const Lexer* lexer, const Sema* sema)
{
    Arena arena = {0};
    arena_init(&arena);
    epr(STRINGP, STRINGV(hir_render(hir, lexer, sema, &arena)));
    arena_done(&arena);
}

bool hir_save(const Hir* hir, const Lexer* lexer, const Sema* sema, cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = hir_render(hir, lexer, sema, &arena);

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
