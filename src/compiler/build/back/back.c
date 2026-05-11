//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>
#if OS_POSIX
#    include <sys/stat.h>
#endif
#include <stdio.h>

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>

//------------------------------------------------------------------------------

static const char g_llvm_runtime_prelude[] = {
#embed "../../../../_obj/llvm/prelude.ll"
    , 0};

internal cstr back_end_cstr(Arena* arena, string text)
{
    char* copy = (char*)arena_alloc(arena, text.count + 1);
    memcpy(copy, text.data, text.count);
    copy[text.count] = '\0';
    return copy;
}

internal void back_end_append_hir_extern_link_flags(StringBuilder* link_flags,
                                                    const ProgramInfo* program)
{
    for (u32 module_index = 0; module_index < array_count(program->modules);
         ++module_index) {
        const Hir* hir = &program->modules[module_index].front_end.hir;
        for (u32 i = 0; i < array_count(hir->externs); ++i) {
            string library = hir->externs[i].library;
            if (string_eq(library, s("c"))) {
                continue;
            }
#if OS_WINDOWS
            if (string_eq(library, s("m"))) {
                continue;
            }
#endif

            bool already_added = false;
            for (u32 previous_module = 0; previous_module <= module_index;
                 ++previous_module) {
                const Hir* previous_hir =
                    &program->modules[previous_module].front_end.hir;
                u32 end = previous_module == module_index
                              ? i
                              : (u32)array_count(previous_hir->externs);
                for (u32 j = 0; j < end; ++j) {
                    if (string_eq(previous_hir->externs[j].library, library)) {
                        already_added = true;
                        break;
                    }
                }
                if (already_added) {
                    break;
                }
            }
            if (!already_added) {
                sb_format(link_flags, " -l" STRINGP, STRINGV(library));
            }
        }
    }
}

internal bool back_end_write_text_file(cstr path, string text)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(text.data, 1, text.count, file);
    bool  close_failed = fclose(file) != 0;
    if (written != text.count || close_failed) {
        return error_runtime("Failed to write file: %s", path);
    }
    return true;
}

internal bool back_end_string_starts_with_cstr(string text, cstr prefix)
{
    usize prefix_count = strlen(prefix);
    return text.count >= prefix_count &&
           memcmp(text.data, prefix, prefix_count) == 0;
}

internal bool back_end_string_contains_cstr(string text, cstr needle)
{
    usize needle_count = strlen(needle);
    if (needle_count == 0 || text.count < needle_count) {
        return false;
    }
    for (usize i = 0; i + needle_count <= text.count; ++i) {
        if (memcmp(text.data + i, needle, needle_count) == 0) {
            return true;
        }
    }
    return false;
}

// The executable backend combines independently-rendered textual LLVM modules
// into one clang input. Before concatenating, remove declarations already
// satisfied by a definition or alias in another module. This is deliberately a
// small top-level LLVM scanner, not a general LLVM parser.
internal bool back_end_llvm_line_symbol(string line, string* out)
{
    usize at = line.count;
    for (usize i = 0; i < line.count; ++i) {
        if (line.data[i] == '@') {
            at = i;
            break;
        }
    }
    if (at == line.count) {
        return false;
    }

    usize end = at + 1;
    if (end < line.count && line.data[end] == '"') {
        usize quoted_start = end + 1;
        usize quoted_end   = line.count;
        for (end = quoted_start; end < line.count; ++end) {
            if (line.data[end] == '"') {
                quoted_end = end;
                break;
            }
        }
        if (quoted_end == line.count || quoted_end == quoted_start) {
            return false;
        }
        *out = (string){
            .data  = line.data + quoted_start,
            .count = quoted_end - quoted_start,
        };
        return true;
    } else {
        while (end < line.count && line.data[end] != '(' &&
               line.data[end] != ' ' && line.data[end] != '\t' &&
               line.data[end] != ',') {
            end++;
        }
    }
    if (end <= at + 1) {
        return false;
    }
    usize symbol_start = at;
    if (at + 1 < end) {
        symbol_start = at + 1;
    }
    *out = (string){
        .data  = line.data + symbol_start,
        .count = end - symbol_start,
    };
    return true;
}

internal bool back_end_llvm_line_defines_symbol(string line, string* out)
{
    if (back_end_string_starts_with_cstr(line, "define ")) {
        return back_end_llvm_line_symbol(line, out);
    }
    // Covers top-level globals and aliases such as:
    //   @"$main" = alias void (), ptr @fn.0
    //   @.str.m0.0 = private unnamed_addr constant ...
    if (line.count > 0 && line.data[0] == '@' &&
        back_end_string_contains_cstr(line, " = ")) {
        return back_end_llvm_line_symbol(line, out);
    }
    return false;
}

internal bool back_end_llvm_line_declares_symbol(string line, string* out)
{
    if (!back_end_string_starts_with_cstr(line, "declare ")) {
        return false;
    }
    return back_end_llvm_line_symbol(line, out);
}

internal bool back_end_symbol_array_contains(Array(string) symbols,
                                             string symbol)
{
    for (u32 i = 0; i < array_count(symbols); ++i) {
        if (string_eq(symbols[i], symbol)) {
            return true;
        }
    }
    return false;
}

internal void back_end_collect_llvm_defined_symbols(Array(string) * symbols,
                                                    string text)
{
    usize line_start = 0;
    for (usize i = 0; i <= text.count; ++i) {
        if (i < text.count && text.data[i] != '\n') {
            continue;
        }
        string line = {
            .data  = text.data + line_start,
            .count = i - line_start,
        };
        string symbol = {0};
        if (back_end_llvm_line_defines_symbol(line, &symbol) &&
            !back_end_symbol_array_contains(*symbols, symbol)) {
            array_push(*symbols, symbol);
        }
        line_start = i + 1;
    }
}

internal void back_end_append_llvm_without_satisfied_declarations(
    StringBuilder* sb,
    string         text,
    Array(string) defined_symbols,
    Array(string) * declared_symbols)
{
    // Keep the first declaration for unresolved external symbols such as libc
    // calls, but drop duplicate declarations and declarations for symbols this
    // combined module defines. Clang rejects `declare @x` plus `define @x` in
    // one textual module even though separate input files allowed that shape.
    usize line_start = 0;
    for (usize i = 0; i <= text.count; ++i) {
        if (i < text.count && text.data[i] != '\n') {
            continue;
        }
        string line = {
            .data  = text.data + line_start,
            .count = i - line_start,
        };
        string declared_symbol = {0};
        if (back_end_llvm_line_declares_symbol(line, &declared_symbol)) {
            if (back_end_symbol_array_contains(defined_symbols,
                                               declared_symbol) ||
                back_end_symbol_array_contains(*declared_symbols,
                                               declared_symbol)) {
                line_start = i + 1;
                continue;
            }
            array_push(*declared_symbols, declared_symbol);
        }
        sb_append_string(sb, line);
        if (i < text.count) {
            sb_append_char(sb, '\n');
        }
        line_start = i + 1;
    }
}

internal bool back_end_hir_has_globals(const Hir* hir)
{
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        if (hir->values[i].kind == HIR_VALUE_Global) {
            return true;
        }
    }
    return false;
}

internal NerdArtifactConfig compiler_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .binary_path    = "a.out",
        .hir_path       = "_a.hir",
        .llvm_path      = "_a.ll",
        .emit_hir_file  = false,
        .emit_llvm_file = false,
        .compile_binary = true,
        .release        = false,
    };
}

internal cstr back_end_module_llvm_path(Arena*                    arena,
                                        const NerdArtifactConfig* artifacts,
                                        u32                       module_index)
{
    if (module_index == 0) {
        return artifacts->llvm_path;
    }
    return back_end_cstr(
        arena,
        string_format(
            arena, "%s.m%u.ll", artifacts->binary_path, module_index));
}

internal bool back_end_root_main_returns_void(const FrontEndState* root)
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

internal void back_end_cleanup_llvm_compile_artifacts(Array(cstr) llvm_paths,
                                                      bool remove_llvm_paths,
                                                      cstr combined_llvm_path)
{
    if (combined_llvm_path != NULL) {
        path_remove(combined_llvm_path);
    }
    if (remove_llvm_paths) {
        for (u32 i = 0; i < array_count(llvm_paths); ++i) {
            path_remove(llvm_paths[i]);
        }
    }
}

internal bool back_end_compile_llvm_program(const ProgramInfo*        program,
                                            const NerdArtifactConfig* artifacts)
{
    if (!artifacts->compile_binary && !artifacts->emit_llvm_file) {
        return true;
    }
    if (program->root_module_index >= array_count(program->modules)) {
        return false;
    }

    Arena arena = {0};
    arena_init(&arena);

    Array(cstr) llvm_paths         = NULL;
    Array(string) module_llvms     = NULL;
    Array(u32) init_module_indices = NULL;
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        const FrontEndState* front_end   = &program->modules[i].front_end;
        string               module_llvm = llvm_render_hir(
            &front_end->hir, &front_end->lexer, &front_end->sema, &arena);
        array_push(module_llvms, module_llvm);
        if (artifacts->emit_llvm_file) {
            cstr llvm_path = back_end_module_llvm_path(&arena, artifacts, i);
            if (!back_end_write_text_file(llvm_path, module_llvm)) {
                back_end_cleanup_llvm_compile_artifacts(
                    llvm_paths, false, NULL);
                array_free(llvm_paths);
                array_free(module_llvms);
                array_free(init_module_indices);
                arena_done(&arena);
                return false;
            }
            array_push(llvm_paths, llvm_path);
        }
        if (back_end_hir_has_globals(&front_end->hir)) {
            array_push(init_module_indices, i);
        }
    }
    if (!artifacts->compile_binary) {
        array_free(llvm_paths);
        array_free(module_llvms);
        array_free(init_module_indices);
        arena_done(&arena);
        return true;
    }

    cstr combined_llvm_path = back_end_cstr(
        &arena, string_format(&arena, "%s.link.ll", artifacts->binary_path));

    const FrontEndState* root =
        &program->modules[program->root_module_index].front_end;
    bool   root_main_returns_void = back_end_root_main_returns_void(root);
    string runtime_epilogue       = root_main_returns_void
                                        ? s("declare void @init()\n"
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
    StringBuilder init_ll_builder = {0};
    sb_init(&init_ll_builder, &arena);
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
    string init_ll                = sb_to_string(&init_ll_builder);

    Array(string) defined_symbols = NULL;
    back_end_collect_llvm_defined_symbols(&defined_symbols,
                                          s(g_llvm_runtime_prelude));
    for (u32 i = 0; i < array_count(module_llvms); ++i) {
        back_end_collect_llvm_defined_symbols(&defined_symbols,
                                              module_llvms[i]);
    }
    back_end_collect_llvm_defined_symbols(&defined_symbols, runtime_epilogue);
    back_end_collect_llvm_defined_symbols(&defined_symbols, init_ll);

    StringBuilder combined_llvm_builder = {0};
    Array(string) declared_symbols      = NULL;
    sb_init(&combined_llvm_builder, &arena);
    back_end_append_llvm_without_satisfied_declarations(
        &combined_llvm_builder,
        s(g_llvm_runtime_prelude),
        defined_symbols,
        &declared_symbols);
    sb_append_cstr(&combined_llvm_builder, "\n\n");
    for (u32 i = 0; i < array_count(module_llvms); ++i) {
        back_end_append_llvm_without_satisfied_declarations(
            &combined_llvm_builder,
            module_llvms[i],
            defined_symbols,
            &declared_symbols);
        sb_append_cstr(&combined_llvm_builder, "\n");
    }
    back_end_append_llvm_without_satisfied_declarations(&combined_llvm_builder,
                                                        runtime_epilogue,
                                                        defined_symbols,
                                                        &declared_symbols);
    sb_append_cstr(&combined_llvm_builder, "\n");
    back_end_append_llvm_without_satisfied_declarations(
        &combined_llvm_builder, init_ll, defined_symbols, &declared_symbols);
    string combined_llvm = sb_to_string(&combined_llvm_builder);
    if (!back_end_write_text_file(combined_llvm_path, combined_llvm)) {
        back_end_cleanup_llvm_compile_artifacts(
            llvm_paths, !artifacts->emit_llvm_file, combined_llvm_path);
        array_free(llvm_paths);
        array_free(module_llvms);
        array_free(defined_symbols);
        array_free(declared_symbols);
        array_free(init_module_indices);
        arena_done(&arena);
        return false;
    }

    string        opt_flags  = artifacts->release ? s("-O2") : s("-g -O0");
    StringBuilder link_flags = {0};
    sb_init(&link_flags, &arena);
    back_end_append_hir_extern_link_flags(&link_flags, program);
    StringBuilder command_builder = {0};
    sb_init(&command_builder, &arena);
    sb_format(&command_builder,
              "clang -Wno-override-module " STRINGP " -o \"%s\" \"%s\"",
              STRINGV(opt_flags),
              artifacts->binary_path,
              combined_llvm_path);
    sb_append_string(&command_builder, sb_to_string(&link_flags));
    string command        = sb_to_string(&command_builder);
    int    compile_result = shell(back_end_cstr(&arena, command));
    if (compile_result != 0) {
        back_end_cleanup_llvm_compile_artifacts(
            llvm_paths, !artifacts->emit_llvm_file, combined_llvm_path);
        array_free(llvm_paths);
        array_free(module_llvms);
        array_free(defined_symbols);
        array_free(declared_symbols);
        array_free(init_module_indices);
        arena_done(&arena);
        return error_runtime(
            "Failed to compile generated LLVM file (exit code %d)",
            compile_result);
    }

#if OS_POSIX
    if (chmod(artifacts->binary_path, 0755) != 0) {
        back_end_cleanup_llvm_compile_artifacts(
            llvm_paths, !artifacts->emit_llvm_file, combined_llvm_path);
        array_free(llvm_paths);
        array_free(module_llvms);
        array_free(defined_symbols);
        array_free(declared_symbols);
        array_free(init_module_indices);
        arena_done(&arena);
        return error_runtime("Failed to make %s executable",
                             artifacts->binary_path);
    }
#endif

    back_end_cleanup_llvm_compile_artifacts(
        llvm_paths, !artifacts->emit_llvm_file, combined_llvm_path);

    array_free(llvm_paths);
    array_free(module_llvms);
    array_free(defined_symbols);
    array_free(declared_symbols);
    array_free(init_module_indices);
    arena_done(&arena);
    return true;
}

bool back_end_program(const ProgramInfo*        program,
                      const NerdArtifactConfig* artifacts,
                      bool                      verbose,
                      Timing*                   timing)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    if (artifacts->emit_hir_file &&
        program->root_module_index < array_count(program->modules)) {
        const FrontEndState* root =
            &program->modules[program->root_module_index].front_end;
        if (!hir_save(
                &root->hir, &root->lexer, &root->sema, artifacts->hir_path)) {
            return false;
        }
    }

    (void)verbose;
    (void)timing;
    return back_end_compile_llvm_program(program, artifacts);
}

//------------------------------------------------------------------------------
