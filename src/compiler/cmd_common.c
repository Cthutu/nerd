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
        .ir_path        = "a.ir",
        .c_path         = "a.c",
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

    FrontEndState front_results = {0};
    if (!front_end(
            effective_source, &front_end_options, timing, &front_results)) {
        front_end_results_done(&front_results);
        if (mapped_source) {
            filemap_unload(&source_map);
        }
        return false;
    }

    BackEndState back_results = {0};
    if (!back_end(&front_results,
                  artifacts,
                  dump_compiler_state,
                  timing,
                  &back_results)) {
        back_end_results_done(&back_results);
        front_end_results_done(&front_results);
        if (mapped_source) {
            filemap_unload(&source_map);
        }
        return false;
    }

    back_end_results_done(&back_results);
    front_end_results_done(&front_results);
    if (mapped_source) {
        filemap_unload(&source_map);
    }
    return true;
}

//------------------------------------------------------------------------------
