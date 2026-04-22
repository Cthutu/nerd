//------------------------------------------------------------------------------
// Format command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/format/format.h>

//------------------------------------------------------------------------------
// Copy one Nerd string into a null-terminated path buffer.

internal cstr compiler_cmd_format_copy_path(Arena* arena, string path)
{
    char* copy = (char*)arena_alloc(arena, path.count + 1);
    memcpy(copy, path.data, path.count);
    copy[path.count] = '\0';
    return copy;
}

//------------------------------------------------------------------------------
// Format a source file in place or to an explicit output path.

int compiler_cmd_format(const NerdFormatConfig* config)
{
    Arena arena = {0};
    arena_init(&arena);

    ASSERT(config->input_path.count > 0, "Expected format input path");

    cstr input_path = compiler_cmd_format_copy_path(&arena, config->input_path);
    cstr output_path = NULL;
    if (config->output_path.count > 0) {
        output_path =
            compiler_cmd_format_copy_path(&arena, config->output_path);
    } else {
        output_path = input_path;
    }
    bool ok = format_file(input_path, output_path);

    arena_done(&arena);
    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
