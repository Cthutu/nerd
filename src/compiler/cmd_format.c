//------------------------------------------------------------------------------
// Format command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/format/format.h>

//------------------------------------------------------------------------------
// Format a source file to a sibling .format file.

int compiler_cmd_format(const NerdFormatConfig* config)
{
    Arena arena = {0};
    arena_init(&arena);

    ASSERT(config->input_path.count > 0, "Expected format input path");

    cstr input_path = (cstr)string_format(&arena, STRINGP, STRINGV(config->input_path)).data;
    cstr output_path = path_replace_extension(&arena, input_path, ".format");
    bool ok = format_file(input_path, output_path);

    arena_done(&arena);
    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
