//------------------------------------------------------------------------------
// Format command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>
#include <compiler/error/error.h>
#include <compiler/format/format.h>

#include <stdio.h>

//------------------------------------------------------------------------------
// Format a source file to stdout, in place, or to an explicit output path.

int compiler_cmd_format(const NerdFormatConfig* config)
{
    Arena arena = {0};
    arena_init(&arena);

    ASSERT(config->input_path.count > 0, "Expected format input path");

    cstr input_path = compiler_cmd_copy_path(&arena, config->input_path);
    if (config->write_stdout) {
        if (config->output_path.count > 0) {
            error_runtime("Cannot combine format --stdout with an output path");
            arena_done(&arena);
            return 1;
        }

        string rendered = {0};
        bool   ok       = format_file_to_string(input_path, &arena, &rendered);
        if (ok) {
            fwrite(rendered.data, 1, rendered.count, stdout);
        }
        arena_done(&arena);
        return ok ? 0 : 1;
    }

    cstr output_path = NULL;
    if (config->output_path.count > 0) {
        output_path = compiler_cmd_copy_path(&arena, config->output_path);
    } else {
        output_path = input_path;
    }
    bool ok = format_file(input_path, output_path);

    arena_done(&arena);
    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
