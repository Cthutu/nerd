//------------------------------------------------------------------------------
// LLVM IR emission from HIR
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>
#include <stdio.h>

//------------------------------------------------------------------------------

internal bool llvm_type_is_void(const Sema* sema, u32 type_index)
{
    return sema != NULL && type_index < array_count(sema->types) &&
           sema->types[type_index].kind == STK_Void;
}

internal u32 llvm_function_return_type(const Sema* sema, u32 type_index)
{
    if (sema == NULL || type_index >= array_count(sema->types)) {
        return sema_no_type();
    }

    const SemaType* type = &sema->types[type_index];
    return type->kind == STK_Function ? type->return_type : sema_no_type();
}

internal u32 llvm_function_param_count(const Sema* sema, u32 type_index)
{
    if (sema == NULL || type_index >= array_count(sema->types)) {
        return 0;
    }

    const SemaType* type = &sema->types[type_index];
    return type->kind == STK_Function ? type->param_count : 0;
}

internal u32 llvm_function_param_type(const Sema* sema,
                                      u32         type_index,
                                      u32         param_index)
{
    if (sema == NULL || type_index >= array_count(sema->types)) {
        return sema_no_type();
    }

    const SemaType* type = &sema->types[type_index];
    if (type->kind != STK_Function || param_index >= type->param_count) {
        return sema_no_type();
    }
    return sema->type_param_types[type->first_param_type + param_index];
}

internal void llvm_append_type(StringBuilder* sb,
                               const Sema*    sema,
                               u32            type_index)
{
    if (sema == NULL || type_index == sema_no_type() ||
        type_index >= array_count(sema->types)) {
        sb_append_cstr(sb, "ptr");
        return;
    }

    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Void:
        sb_append_cstr(sb, "void");
        break;
    case STK_Bool:
        sb_append_cstr(sb, "i1");
        break;
    case STK_I8:
    case STK_U8:
        sb_append_cstr(sb, "i8");
        break;
    case STK_I16:
    case STK_U16:
        sb_append_cstr(sb, "i16");
        break;
    case STK_I32:
    case STK_U32:
    case STK_UntypedInteger:
        sb_append_cstr(sb, "i32");
        break;
    case STK_I64:
    case STK_U64:
        sb_append_cstr(sb, "i64");
        break;
    case STK_Isize:
    case STK_Usize:
        sb_append_cstr(sb, "i64");
        break;
    case STK_F32:
        sb_append_cstr(sb, "float");
        break;
    case STK_F64:
    case STK_UntypedFloat:
        sb_append_cstr(sb, "double");
        break;
    case STK_Function:
    case STK_Pointer:
    case STK_String:
    case STK_Array:
    case STK_Slice:
    case STK_DynamicArray:
    case STK_Plex:
    case STK_Union:
    case STK_Enum:
    case STK_Tuple:
    case STK_Module:
    default:
        sb_append_cstr(sb, "ptr");
        break;
    }
}

internal void llvm_append_symbol_name(StringBuilder* sb, string name)
{
    sb_append_char(sb, '@');
    sb_append_char(sb, '$');
    for (usize i = 0; i < name.count; ++i) {
        u8 ch = name.data[i];
        bool simple = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                      (ch >= '0' && ch <= '9') || ch == '_' || ch == '$' ||
                      ch == '.';
        sb_append_char(sb, simple ? (char)ch : '$');
    }
}

internal void llvm_append_generated_function_name(StringBuilder* sb,
                                                 u32            function_index)
{
    sb_format(sb, "@fn.%u", function_index);
}

internal u32 llvm_find_function_binding(const Hir* hir, u32 function_index)
{
    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        const HirBinding* binding = &hir->bindings[i];
        if (binding->kind == HIR_BINDING_Function &&
            binding->target_index == function_index) {
            return i;
        }
    }
    return U32_MAX;
}

internal void llvm_append_function_name(StringBuilder* sb,
                                        const Hir*     hir,
                                        const Lexer*   lexer,
                                        u32            function_index)
{
    u32 binding_index = llvm_find_function_binding(hir, function_index);
    if (binding_index == U32_MAX) {
        llvm_append_generated_function_name(sb, function_index);
        return;
    }

    const HirBinding* binding = &hir->bindings[binding_index];
    llvm_append_symbol_name(sb, lex_symbol(lexer, binding->symbol_handle));
}

internal void llvm_append_function_signature(StringBuilder*   sb,
                                             const Hir*        hir,
                                             const Lexer*      lexer,
                                             const Sema*       sema,
                                             const HirFunction* function,
                                             u32               function_index)
{
    u32 return_type = llvm_function_return_type(sema, function->type_index);
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    llvm_append_function_name(sb, hir, lexer, function_index);
    sb_append_char(sb, '(');
    for (u32 i = 0; i < function->param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        const HirParam* param = &hir->params[function->first_param + i];
        llvm_append_type(sb, sema, param->type_index);
        if (param->symbol_handle != U32_MAX) {
            sb_append_cstr(sb, " %");
            sb_append_string(sb, lex_symbol(lexer, param->symbol_handle));
        }
    }
    sb_append_char(sb, ')');
}

internal void llvm_append_default_return(StringBuilder* sb,
                                         const Sema*    sema,
                                         u32            return_type)
{
    if (llvm_type_is_void(sema, return_type)) {
        sb_append_cstr(sb, "  ret void\n");
        return;
    }

    sb_append_cstr(sb, "  ret ");
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    if (sema == NULL || return_type >= array_count(sema->types)) {
        sb_append_cstr(sb, "null\n");
        return;
    }

    switch (sema->types[return_type].kind) {
    case STK_F32:
    case STK_F64:
    case STK_UntypedFloat:
        sb_append_cstr(sb, "0.000000e+00\n");
        break;
    case STK_Function:
    case STK_Pointer:
    case STK_String:
    case STK_Array:
    case STK_Slice:
    case STK_DynamicArray:
    case STK_Plex:
    case STK_Union:
    case STK_Enum:
    case STK_Tuple:
    case STK_Module:
        sb_append_cstr(sb, "null\n");
        break;
    default:
        sb_append_cstr(sb, "0\n");
        break;
    }
}

internal void llvm_render_import(StringBuilder* sb,
                                 const Lexer*   lexer,
                                 const Sema*    sema,
                                 const HirImport* import)
{
    if (sema == NULL || import->type_index >= array_count(sema->types) ||
        sema->types[import->type_index].kind != STK_Function) {
        return;
    }

    sb_append_cstr(sb, "declare ");
    u32 return_type = llvm_function_return_type(sema, import->type_index);
    llvm_append_type(sb, sema, return_type);
    sb_append_char(sb, ' ');
    llvm_append_symbol_name(sb, lex_symbol(lexer, import->symbol_handle));
    sb_append_char(sb, '(');
    u32 param_count = llvm_function_param_count(sema, import->type_index);
    for (u32 i = 0; i < param_count; ++i) {
        if (i > 0) {
            sb_append_cstr(sb, ", ");
        }
        llvm_append_type(
            sb, sema, llvm_function_param_type(sema, import->type_index, i));
    }
    sb_append_cstr(sb, ")\n");
}

internal void llvm_render_function(StringBuilder*    sb,
                                   const Hir*         hir,
                                   const Lexer*       lexer,
                                   const Sema*        sema,
                                   const HirFunction* function,
                                   u32                function_index)
{
    if (function->kind == HIR_FUNCTION_Ffi ||
        function->body_block_index == U32_MAX) {
        sb_append_cstr(sb, "declare ");
        llvm_append_function_signature(
            sb, hir, lexer, sema, function, function_index);
        sb_append_char(sb, '\n');
        return;
    }

    sb_append_cstr(sb, "define ");
    llvm_append_function_signature(
        sb, hir, lexer, sema, function, function_index);
    sb_append_cstr(sb, " {\n");
    u32 return_type = llvm_function_return_type(sema, function->type_index);
    llvm_append_default_return(sb, sema, return_type);
    sb_append_cstr(sb, "}\n");
}

string llvm_render_hir(const Hir* hir,
                       const Lexer* lexer,
                       const Sema* sema,
                       Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    sb_append_cstr(&sb, "; nerd llvm-ir 0\n");
    sb_append_cstr(&sb, "; generated from HIR\n\n");

    for (u32 i = 0; i < array_count(hir->imports); ++i) {
        llvm_render_import(&sb, lexer, sema, &hir->imports[i]);
    }
    if (array_count(hir->imports) > 0) {
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->functions); ++i) {
        llvm_render_function(&sb, hir, lexer, sema, &hir->functions[i], i);
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(hir->exports); ++i) {
        const HirExport* export = &hir->exports[i];
        if (export->binding_index >= array_count(hir->bindings)) {
            continue;
        }
        const HirBinding* binding = &hir->bindings[export->binding_index];
        sb_append_cstr(&sb, "; export ");
        sb_append_string(&sb, lex_symbol(lexer, binding->symbol_handle));
        sb_append_char(&sb, '\n');
    }

    return sb_to_string(&sb);
}

bool llvm_save_hir(const Hir* hir,
                   const Lexer* lexer,
                   const Sema* sema,
                   cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = llvm_render_hir(hir, lexer, sema, &arena);

    FILE* file = fopen(path, "wb");
    if (!file) {
        arena_done(&arena);
        return error_runtime("Failed to open LLVM IR file for writing: %s", path);
    }

    usize written = fwrite(rendered.data, 1, rendered.count, file);
    bool close_failed = fclose(file) != 0;
    arena_done(&arena);

    if (written != rendered.count || close_failed) {
        return error_runtime("Failed to write LLVM IR file: %s", path);
    }
    return true;
}

//------------------------------------------------------------------------------
