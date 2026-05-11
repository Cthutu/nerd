//------------------------------------------------------------------------------
// LLVM runtime glue for the executable backend
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/back/llvm_runtime.h>
#include <compiler/error/error.h>
#include <stdio.h>

//------------------------------------------------------------------------------

static const unsigned char g_nrt_object[] = {
#embed "../../../../_obj/runtime/nrt.o"
};

bool back_end_llvm_runtime_write_object(cstr path)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(g_nrt_object, 1, sizeof(g_nrt_object), file);
    bool  close_failed = fclose(file) != 0;
    if (written != sizeof(g_nrt_object) || close_failed) {
        return error_runtime("Failed to write file: %s", path);
    }
    return true;
}

bool back_end_llvm_runtime_hir_has_globals(const Hir* hir)
{
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        if (hir->values[i].kind == HIR_VALUE_Global) {
            return true;
        }
    }
    return false;
}

bool back_end_llvm_runtime_root_main_returns_void(const FrontEndState* root)
{
    const Hir*   hir   = &root->hir;
    const Lexer* lexer = &root->lexer;
    const Sema*  sema  = &root->sema;
    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        const HirBinding* binding = &hir->bindings[i];
        if (binding->kind != HIR_BINDING_Function ||
            binding->target_index >= array_count(hir->functions) ||
            !string_eq_cstr(lex_symbol(lexer, binding->symbol_handle),
                            "main")) {
            continue;
        }

        const HirFunction* function = &hir->functions[binding->target_index];
        if (function->type_index >= array_count(sema->types) ||
            sema->types[function->type_index].kind != STK_Function) {
            return false;
        }
        u32 return_type = sema->types[function->type_index].return_type;
        return return_type < array_count(sema->types) &&
               sema->types[return_type].kind == STK_Void;
    }
    return false;
}

string back_end_llvm_runtime_epilogue(bool root_main_returns_void)
{
    return root_main_returns_void ? s("declare void @init()\n"
                                      "declare void @$main()\n"
                                      "\n"
                                      "define i32 @main() {\n"
                                      "  call void @init()\n"
                                      "  call void @$main()\n"
                                      "  ret i32 0\n"
                                      "}\n")
                                  : s("declare void @init()\n"
                                      "declare i32 @$main()\n"
                                      "\n"
                                      "define i32 @main() {\n"
                                      "  call void @init()\n"
                                      "  %result = call i32 @$main()\n"
                                      "  ret i32 %result\n"
                                      "}\n");
}

string back_end_llvm_runtime_render_init(Arena* arena,
                                         Array(u32) init_module_indices)
{
    StringBuilder init_ll_builder = {0};
    sb_init(&init_ll_builder, arena);
    for (u32 i = 0; i < array_count(init_module_indices); ++i) {
        sb_format(&init_ll_builder,
                  "declare void @m%u.init()\n",
                  init_module_indices[i]);
    }
    if (array_count(init_module_indices) > 0) {
        sb_append_char(&init_ll_builder, '\n');
    }
    sb_append_cstr(&init_ll_builder, "define void @init() {\n");
    for (u32 i = 0; i < array_count(init_module_indices); ++i) {
        sb_format(&init_ll_builder,
                  "  call void @m%u.init()\n",
                  init_module_indices[i]);
    }
    sb_append_cstr(&init_ll_builder, "  ret void\n}\n");
    return sb_to_string(&init_ll_builder);
}

//------------------------------------------------------------------------------
