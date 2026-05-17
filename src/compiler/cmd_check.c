//------------------------------------------------------------------------------
// Check command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/cmd_internal.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

internal bool compiler_cmd_check_load_source(NerdSource* source,
                                             FileMap*    out_map,
                                             bool*       out_mapped)
{
    *out_mapped = false;
    if (source->source.count != 0 || source->source_path.count == 0) {
        return true;
    }

    char* source_path = calloc(source->source_path.count + 1, sizeof(char));
    if (source_path == NULL) {
        return error_runtime("Failed to allocate source path buffer");
    }

    memcpy(source_path, source->source_path.data, source->source_path.count);
    source_path[source->source_path.count] = '\0';

    if (path_is_directory(source_path)) {
        string bad_path = source->source_path;
        free(source_path);
        return error_runtime(
            "Expected a source file but found directory: " STRINGP,
            STRINGV(bad_path));
    }

    source->source = filemap_load(source_path, out_map);
    free(source_path);
    if (source->source.data == NULL) {
        return error_runtime("Failed to load source file: " STRINGP,
                             STRINGV(source->source_path));
    }

    *out_mapped = true;
    return true;
}

int compiler_cmd_check(const NerdCheckConfig* config)
{
    NerdSource source        = config->source;
    FileMap    source_map    = {0};
    bool       mapped_source = false;

    if (!compiler_cmd_check_load_source(&source, &source_map, &mapped_source)) {
        return 1;
    }

    Timing timing = {0};
    timing_init(&timing);

    FrontEndOptions options = {
        .verbose             = config->verbose,
        .release             = config->release,
        .require_entry_point = true,
        .skip_hir_generation = true,
        .keywords            = config->keywords,
    };

    ProgramInfo program = {0};
    bool        ok = front_end_program(source, &options, &timing, &program);
    program_info_done(&program);

    if (config->timing) {
        timing_dump(&timing);
    }
    timing_done(&timing);

    if (mapped_source) {
        filemap_unload(&source_map);
    }

    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
