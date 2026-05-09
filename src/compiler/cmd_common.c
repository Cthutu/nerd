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
        .binary_path    = "a.out",
        .hir_path       = "_a.hir",
        .ir_path        = "_a.ir",
        .c_path         = "_a.gen.c",
        .emit_hir_file  = false,
        .emit_ir_file   = false,
        .emit_c_file    = false,
        .compile_binary = true,
        .release        = false,
        .keywords       = NULL,
    };
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

    NerdSource effective_source = source;
    FileMap    source_map       = {0};
    bool       mapped_source    = false;

    if (effective_source.source.count == 0 &&
        effective_source.source_path.count > 0) {
        char* source_path =
            calloc(effective_source.source_path.count + 1, sizeof(char));
        if (source_path == NULL) {
            return error_runtime("Failed to allocate source path buffer");
        }

        memcpy(source_path,
               effective_source.source_path.data,
               effective_source.source_path.count);
        source_path[effective_source.source_path.count] = '\0';

        if (path_is_directory(source_path)) {
            string bad_path = effective_source.source_path;
            free(source_path);
            return error_runtime(
                "Expected a source file but found directory: " STRINGP,
                STRINGV(bad_path));
        }

        effective_source.source = filemap_load(source_path, &source_map);
        free(source_path);
        if (effective_source.source.data == NULL) {
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
        return false;
    }

    if (!back_end_program(&program, artifacts, dump_compiler_state, timing)) {
        program_info_done(&program);
        if (mapped_source) {
            filemap_unload(&source_map);
        }
        return false;
    }

    program_info_done(&program);
    if (mapped_source) {
        filemap_unload(&source_map);
    }
    return true;
}

//------------------------------------------------------------------------------
