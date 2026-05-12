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
#include <compiler/build/back/llvm_runtime.h>
#include <compiler/build/back/llvm_text.h>
#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>

//------------------------------------------------------------------------------

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
            if (string_eq(library, s("c")) || string_eq(library, s("nrt"))) {
                continue;
            }
#if OS_WINDOWS || OS_MACOS
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

internal NerdArtifactConfig compiler_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .binary_path     = "a.out",
        .hir_path        = "_a.hir",
        .llvm_path       = "_a.ll",
        .emit_hir_file   = false,
        .emit_llvm_file  = false,
        .emit_executable = true,
        .release         = false,
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

internal void back_end_cleanup_llvm_artifacts(Array(cstr) llvm_paths,
                                              bool remove_llvm_paths,
                                              cstr combined_llvm_path,
                                              cstr runtime_object_path)
{
    if (combined_llvm_path != NULL) {
        path_remove(combined_llvm_path);
    }
    if (runtime_object_path != NULL) {
        path_remove(runtime_object_path);
    }
    if (remove_llvm_paths) {
        for (u32 i = 0; i < array_count(llvm_paths); ++i) {
            path_remove(llvm_paths[i]);
        }
    }
}

typedef struct {
    Array(cstr) llvm_paths;
    Array(string) module_llvms;
    Array(u32) init_module_indices;
} BackEndLlvmModules;

internal void back_end_llvm_modules_done(BackEndLlvmModules* modules)
{
    array_free(modules->llvm_paths);
    array_free(modules->module_llvms);
    array_free(modules->init_module_indices);
    *modules = (BackEndLlvmModules){0};
}

internal bool back_end_render_llvm_modules(Arena*                    arena,
                                           const ProgramInfo*        program,
                                           const NerdArtifactConfig* artifacts,
                                           BackEndLlvmModules*       out)
{
    u32 module_count = (u32)array_count(program->modules);
    array_requires_capacity(out->module_llvms, module_count);
    if (artifacts->emit_llvm_file) {
        array_requires_capacity(out->llvm_paths, module_count);
    }

    for (u32 i = 0; i < module_count; ++i) {
        const FrontEndState* front_end   = &program->modules[i].front_end;
        string               module_llvm = llvm_render_hir(
            &front_end->hir, &front_end->lexer, &front_end->sema, arena);
        array_push(out->module_llvms, module_llvm);
        if (artifacts->emit_llvm_file) {
            cstr llvm_path = back_end_module_llvm_path(arena, artifacts, i);
            if (!back_end_write_text_file(llvm_path, module_llvm)) {
                return false;
            }
            array_push(out->llvm_paths, llvm_path);
        }
        if (back_end_llvm_runtime_hir_has_globals(&front_end->hir)) {
            array_push(out->init_module_indices, i);
        }
    }
    return true;
}

internal bool back_end_link_combined_llvm(Arena*                    arena,
                                          const ProgramInfo*        program,
                                          const NerdArtifactConfig* artifacts,
                                          cstr combined_llvm_path,
                                          cstr runtime_object_path)
{
    string        opt_flags  = artifacts->release ? s("-O2") : s("-g -O0");
    StringBuilder link_flags = {0};
    sb_init(&link_flags, arena);
    back_end_append_hir_extern_link_flags(&link_flags, program);
    StringBuilder command_builder = {0};
    sb_init(&command_builder, arena);
    sb_format(&command_builder,
              "clang -Wno-override-module " STRINGP " -o \"%s\" \"%s\" \"%s\"",
              STRINGV(opt_flags),
              artifacts->binary_path,
              combined_llvm_path,
              runtime_object_path);
    sb_append_string(&command_builder, sb_to_string(&link_flags));
    string command        = sb_to_string(&command_builder);
    int    compile_result = shell(back_end_cstr(arena, command));
    if (compile_result != 0) {
        return error_runtime(
            "Failed to compile generated LLVM file (exit code %d)",
            compile_result);
    }

#if OS_POSIX
    if (chmod(artifacts->binary_path, 0755) != 0) {
        return error_runtime("Failed to make %s executable",
                             artifacts->binary_path);
    }
#endif

    return true;
}

internal bool back_end_emit_llvm_artifacts(const ProgramInfo*        program,
                                           const NerdArtifactConfig* artifacts)
{
    if (!artifacts->emit_executable && !artifacts->emit_llvm_file) {
        return true;
    }
    if (program->root_module_index >= array_count(program->modules)) {
        return false;
    }

    Arena arena = {0};
    arena_init(&arena);

    BackEndLlvmModules modules = {0};
    MemoryStats        memory_before = compiler_memory_profile_begin();
    if (!back_end_render_llvm_modules(&arena, program, artifacts, &modules)) {
        compiler_memory_profile_end(
            COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_RENDER, memory_before);
        back_end_cleanup_llvm_artifacts(modules.llvm_paths, false, NULL, NULL);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_RENDER, memory_before);
    if (!artifacts->emit_executable) {
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return true;
    }

    cstr combined_llvm_path = back_end_cstr(
        &arena, string_format(&arena, "%s.link.ll", artifacts->binary_path));
    cstr runtime_object_path = back_end_cstr(
        &arena, string_format(&arena, "%s.nrt.o", artifacts->binary_path));

    const FrontEndState* root =
        &program->modules[program->root_module_index].front_end;
    bool root_main_returns_void =
        back_end_llvm_runtime_root_main_returns_void(root);
    string runtime_epilogue =
        back_end_llvm_runtime_epilogue(root_main_returns_void);
    string init_ll =
        back_end_llvm_runtime_render_init(&arena, modules.init_module_indices);
    memory_before = compiler_memory_profile_begin();
    string combined_llvm = back_end_llvm_text_build_combined(
        &arena, modules.module_llvms, runtime_epilogue, init_ll);
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_COMBINE, memory_before);
    if (!back_end_write_text_file(combined_llvm_path, combined_llvm)) {
        back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                        !artifacts->emit_llvm_file,
                                        combined_llvm_path,
                                        NULL);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    memory_before = compiler_memory_profile_begin();
    if (!back_end_llvm_runtime_write_object(runtime_object_path)) {
        compiler_memory_profile_end(COMPILER_STAGE_BACK_END,
                                    COMPILER_PHASE_RUNTIME_OBJECT,
                                    memory_before);
        back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                        !artifacts->emit_llvm_file,
                                        combined_llvm_path,
                                        runtime_object_path);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_RUNTIME_OBJECT, memory_before);

    memory_before = compiler_memory_profile_begin();
    if (!back_end_link_combined_llvm(&arena,
                                     program,
                                     artifacts,
                                     combined_llvm_path,
                                     runtime_object_path)) {
        compiler_memory_profile_end(
            COMPILER_STAGE_BACK_END, COMPILER_PHASE_LINK, memory_before);
        back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                        !artifacts->emit_llvm_file,
                                        combined_llvm_path,
                                        runtime_object_path);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LINK, memory_before);

    back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                    !artifacts->emit_llvm_file,
                                    combined_llvm_path,
                                    runtime_object_path);

    back_end_llvm_modules_done(&modules);
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
    return back_end_emit_llvm_artifacts(program, artifacts);
}

//------------------------------------------------------------------------------
