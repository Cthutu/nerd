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

static const unsigned char g_nrt_pic_object[] = {
#embed "../../../../_obj/runtime/nrt.pic.o"
};

internal bool back_end_llvm_runtime_write_bytes(cstr                 path,
                                                const unsigned char* bytes,
                                                usize                count)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(bytes, 1, count, file);
    bool  close_failed = fclose(file) != 0;
    if (written != count || close_failed) {
        return error_runtime("Failed to write file: %s", path);
    }
    return true;
}

bool back_end_llvm_runtime_write_object(cstr path)
{
    return back_end_llvm_runtime_write_bytes(
        path, g_nrt_object, sizeof(g_nrt_object));
}

bool back_end_llvm_runtime_write_pic_object(cstr path)
{
    return back_end_llvm_runtime_write_bytes(
        path, g_nrt_pic_object, sizeof(g_nrt_pic_object));
}

BackEndRootMainInfo
back_end_llvm_runtime_root_main_info(const FrontEndState* root)
{
    BackEndRootMainInfo info  = {0};
    const Hir*          hir   = &root->hir;
    const Lexer*        lexer = &root->lexer;
    const Sema*         sema  = &root->sema;
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
            return info;
        }
        const SemaType* fn_type     = &sema->types[function->type_index];
        u32             return_type = fn_type->return_type;
        info.returns_void           = return_type < array_count(sema->types) &&
                                      sema->types[return_type].kind == STK_Void;
        info.takes_args             = fn_type->param_count == 1;
        return info;
    }
    return info;
}

internal void back_end_append_main_call(StringBuilder*      sb,
                                        BackEndRootMainInfo main_info)
{
    cstr return_type = main_info.returns_void ? "void" : "i32";
    if (!main_info.takes_args) {
        if (main_info.returns_void) {
            sb_append_cstr(sb, "  call void @$main()\n");
        } else {
            sb_format(sb, "  %%result = call %s @$main()\n", return_type);
        }
        return;
    }

    sb_append_cstr(sb,
                   "  %args = alloca { ptr, i64 }, i64 %argc64\n"
                   "  br label %args.loop\n"
                   "\n"
                   "args.loop:\n"
                   "  %i = phi i64 [ 0, %entry ], [ %next, %args.body ]\n"
                   "  %more = icmp ult i64 %i, %argc64\n"
                   "  br i1 %more, label %args.body, label %args.done\n"
                   "\n"
                   "args.body:\n"
                   "  %argv.slot = getelementptr ptr, ptr %argv, i64 %i\n"
                   "  %arg.data = load ptr, ptr %argv.slot\n"
                   "  %arg.count = call i64 @strlen(ptr %arg.data)\n"
                   "  %arg.slot = getelementptr { ptr, i64 }, ptr %args, i64 "
                   "%i\n"
                   "  %arg.data.slot = getelementptr { ptr, i64 }, ptr "
                   "%arg.slot, i32 0, i32 0\n"
                   "  store ptr %arg.data, ptr %arg.data.slot\n"
                   "  %arg.count.slot = getelementptr { ptr, i64 }, ptr "
                   "%arg.slot, i32 0, i32 1\n"
                   "  store i64 %arg.count, ptr %arg.count.slot\n"
                   "  %next = add i64 %i, 1\n"
                   "  br label %args.loop\n"
                   "\n"
                   "args.done:\n");
    sb_append_cstr(sb,
                   "  %arg.slice = insertvalue { ptr, i64 } zeroinitializer, "
                   "ptr %args, 0\n"
                   "  %arg.slice.1 = insertvalue { ptr, i64 } %arg.slice, "
                   "i64 %argc64, 1\n");
    if (main_info.returns_void) {
        sb_append_cstr(sb, "  call void @$main({ ptr, i64 } %arg.slice.1)\n");
    } else {
        sb_append_cstr(
            sb, "  %result = call i32 @$main({ ptr, i64 } %arg.slice.1)\n");
    }
}

internal string back_end_llvm_runtime_console_epilogue(
    Arena* arena, BackEndRootMainInfo main_info)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "declare void @init()\n");
    if (main_info.takes_args) {
        sb_append_cstr(&sb, "declare i64 @strlen(ptr)\n");
    }
    sb_append_cstr(&sb, "declare ");
    sb_append_cstr(&sb, main_info.returns_void ? "void" : "i32");
    sb_append_cstr(&sb, " @$main(");
    if (main_info.takes_args) {
        sb_append_cstr(&sb, "{ ptr, i64 }");
    }
    sb_append_cstr(&sb,
                   ")\n"
                   "\n"
                   "define i32 @main(i32 %argc, ptr %argv) {\n"
                   "entry:\n"
                   "  %argc64 = sext i32 %argc to i64\n"
                   "  call void @init()\n");
    back_end_append_main_call(&sb, main_info);
    if (main_info.returns_void) {
        sb_append_cstr(&sb, "  ret i32 0\n}\n");
    } else {
        sb_append_cstr(&sb, "  ret i32 %result\n}\n");
    }
    return sb_to_string(&sb);
}

internal string back_end_llvm_runtime_windowed_epilogue(
    Arena* arena, BackEndRootMainInfo main_info)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, "declare void @init()\n");
    if (main_info.takes_args) {
        sb_append_cstr(&sb, "declare i64 @strlen(ptr)\n");
    }
    sb_append_cstr(&sb, "declare ");
    sb_append_cstr(&sb, main_info.returns_void ? "void" : "i32");
    sb_append_cstr(&sb, " @$main(");
    if (main_info.takes_args) {
        sb_append_cstr(&sb, "{ ptr, i64 }");
    }
    sb_append_cstr(&sb,
                   ")\n"
                   "\n"
                   "define i32 @WinMain(ptr %hInstance, ptr %hPrevInstance, "
                   "ptr %lpCmdLine, i32 %nCmdShow) {\n"
                   "entry:\n"
                   "  %argc64 = add i64 0, 0\n"
                   "  %argv = inttoptr i64 0 to ptr\n"
                   "  call void @init()\n");
    if (main_info.takes_args) {
        back_end_append_main_call(&sb, main_info);
        if (main_info.returns_void) {
            sb_append_cstr(&sb, "  ret i32 0\n}\n");
        } else {
            sb_append_cstr(&sb, "  ret i32 %result\n}\n");
        }
        return sb_to_string(&sb);
    }
    if (main_info.returns_void) {
        sb_append_cstr(&sb,
                       "  call void @$main()\n"
                       "  ret i32 0\n}\n");
    } else {
        sb_append_cstr(&sb,
                       "  %result = call i32 @$main()\n"
                       "  ret i32 %result\n}\n");
    }
    return sb_to_string(&sb);
}

string back_end_llvm_runtime_epilogue(Arena*              arena,
                                      BackEndRootMainInfo main_info,
                                      bool                windowed)
{
    if (windowed) {
        return back_end_llvm_runtime_windowed_epilogue(arena, main_info);
    }
    return back_end_llvm_runtime_console_epilogue(arena, main_info);
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
