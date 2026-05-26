//------------------------------------------------------------------------------
// Shared helpers for compiler commands
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
#include <compiler/cmd_internal.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

NerdArtifactConfig compiler_cmd_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .binary_path     = "a.out",
        .hir_path        = "_a.hir",
        .llvm_path       = "_a.ll",
        .emit_hir_file   = false,
        .emit_llvm_file  = false,
        .emit_executable = true,
        .release         = false,
        .keywords        = NULL,
    };
}

void nerd_side_file_registry_init(NerdSideFileRegistry* registry)
{
    *registry = (NerdSideFileRegistry){0};
    arena_init(&registry->arena);
}

void nerd_side_file_registry_done(NerdSideFileRegistry* registry)
{
    array_free(registry->cleanup_paths);
    arena_done(&registry->arena);
    *registry = (NerdSideFileRegistry){0};
}

void nerd_side_file_register_cleanup(NerdSideFileRegistry* registry, cstr path)
{
    if (registry == NULL || path == NULL || path[0] == '\0') {
        return;
    }

    usize len  = strlen(path);
    char* copy = (char*)arena_alloc(&registry->arena, len + 1);
    memcpy(copy, path, len + 1);
    array_push(registry->cleanup_paths, copy);
}

void nerd_side_file_cleanup_registered(NerdSideFileRegistry* registry)
{
    if (registry == NULL) {
        return;
    }

    for (u32 i = 0; i < array_count(registry->cleanup_paths); ++i) {
        path_remove(registry->cleanup_paths[i]);
    }
}

cstr compiler_cmd_copy_path(Arena* arena, string path)
{
    char* copy = (char*)arena_alloc(arena, path.count + 1);
    memcpy(copy, path.data, path.count);
    copy[path.count] = '\0';
    return copy;
}

cstr compiler_cmd_output_root(Arena*     arena,
                              string     output_path,
                              NerdSource source)
{
    if (output_path.count > 0) {
        return compiler_cmd_copy_path(arena, output_path);
    }

    if (source.source_path.count > 0) {
        cstr source_path = compiler_cmd_copy_path(arena, source.source_path);
        return path_replace_extension(arena, source_path, "");
    }

    return compiler_cmd_default_artifacts().binary_path;
}

cstr compiler_cmd_build_binary_path(Arena* arena, cstr output_root)
{
#if OS_WINDOWS
    string path = s(output_root);
    if (path.count >= 4 && (path.data[path.count - 4] == '.') &&
        (path.data[path.count - 3] == 'e' ||
         path.data[path.count - 3] == 'E') &&
        (path.data[path.count - 2] == 'x' ||
         path.data[path.count - 2] == 'X') &&
        (path.data[path.count - 1] == 'e' ||
         path.data[path.count - 1] == 'E')) {
        return output_root;
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, output_root);
    sb_append_cstr(&sb, ".exe");
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
#else
    UNUSED(arena);
    return output_root;
#endif
}

cstr compiler_cmd_sidecar_path(Arena* arena, cstr output_root, cstr extension)
{
    cstr          dir_path = path_dirname(arena, output_root);
    string        stem     = path_stem(s(output_root));
    StringBuilder sb       = {0};
    sb_init(&sb, arena);
    sb_append_char(&sb, '_');
    sb_append_string(&sb, stem);
    sb_append_cstr(&sb, extension);
    sb_append_null(&sb);
    return path_join(arena, dir_path, (cstr)sb_to_string(&sb).data);
}

cstr compiler_cmd_temp_binary_path(Arena* arena, cstr output_root)
{
    cstr          dir_path = path_dirname(arena, output_root);
    string        stem     = path_stem(s(output_root));
    StringBuilder sb       = {0};
    sb_init(&sb, arena);
    sb_append_char(&sb, '_');
    sb_append_string(&sb, stem);
    sb_append_cstr(&sb, ".out");
    sb_append_null(&sb);
    return path_join(arena, dir_path, (cstr)sb_to_string(&sb).data);
}

bool compile(NerdSource                source,
             const NerdArtifactConfig* artifacts,
             bool                      dump_compiler_state,
             Timing*                   timing)
{
    NerdArtifactConfig default_artifacts = compiler_cmd_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    NerdSideFileRegistry side_files = {0};
    nerd_side_file_registry_init(&side_files);

    NerdArtifactConfig effective_artifacts = *artifacts;
    effective_artifacts.side_files         = &side_files;
    artifacts                              = &effective_artifacts;

    if (!artifacts->emit_hir_file) {
        nerd_side_file_register_cleanup(&side_files, artifacts->hir_path);
    }
    if (!artifacts->emit_llvm_file) {
        nerd_side_file_register_cleanup(&side_files, artifacts->llvm_path);
    }

    NerdSource effective_source = source;
    FileMap    source_map       = {0};
    bool       mapped_source    = false;

    if (effective_source.source.count == 0 &&
        effective_source.source_path.count > 0) {
        char* source_path =
            calloc(effective_source.source_path.count + 1, sizeof(char));
        if (source_path == NULL) {
            nerd_side_file_registry_done(&side_files);
            return error_runtime("Failed to allocate source path buffer");
        }

        memcpy(source_path,
               effective_source.source_path.data,
               effective_source.source_path.count);
        source_path[effective_source.source_path.count] = '\0';

        if (path_is_directory(source_path)) {
            string bad_path = effective_source.source_path;
            free(source_path);
            nerd_side_file_registry_done(&side_files);
            return error_runtime(
                "Expected a source file but found directory: " STRINGP,
                STRINGV(bad_path));
        }

        effective_source.source = filemap_load(source_path, &source_map);
        free(source_path);
        if (effective_source.source.data == NULL) {
            nerd_side_file_registry_done(&side_files);
            return error_runtime("Failed to load source file: " STRINGP,
                                 STRINGV(effective_source.source_path));
        }
        mapped_source = true;
    }

    FrontEndOptions front_end_options = {
        .verbose             = dump_compiler_state,
        .release             = artifacts->release,
        .require_entry_point = true,
        .keywords            = artifacts->keywords,
    };

    ProgramInfo program = {0};
    if (!front_end_program(
            effective_source, &front_end_options, timing, &program)) {
        program_info_done(&program);
        if (mapped_source) {
            filemap_unload(&source_map);
        }
        nerd_side_file_cleanup_registered(&side_files);
        nerd_side_file_registry_done(&side_files);
        return false;
    }

    if (!back_end_program(&program, artifacts, dump_compiler_state, timing)) {
        program_info_done(&program);
        if (mapped_source) {
            filemap_unload(&source_map);
        }
        nerd_side_file_registry_done(&side_files);
        return false;
    }

    program_info_done(&program);
    if (mapped_source) {
        filemap_unload(&source_map);
    }
    nerd_side_file_cleanup_registered(&side_files);
    nerd_side_file_registry_done(&side_files);
    return true;
}

//------------------------------------------------------------------------------
