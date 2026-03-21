//------------------------------------------------------------------------------
// Million-lines command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

internal string compiler_cmd_make_million_lines_source(Arena* arena)
{
    usize total_bytes = NERD_MILLION_LINES_COUNT * 3;
    u8*   data        = (u8*)arena_alloc(arena, total_bytes);

    for (usize i = 0; i < NERD_MILLION_LINES_COUNT; i++) {
        usize offset     = i * 3;
        data[offset + 0] = '4';
        data[offset + 1] = '2';
        data[offset + 2] = '\n';
    }

    return string_from(data, total_bytes);
}

int compiler_cmd_million(const NerdMillionConfig* config)
{
    UNUSED(config);

    TimePoint total_start = time_now();

    Arena source_arena    = {0};
    arena_init(&source_arena);

    string source = compiler_cmd_make_million_lines_source(&source_arena);
    prn("Generated benchmark input: %u lines (%zu bytes)",
        NERD_MILLION_LINES_COUNT,
        source.count);

    Timing timing = {0};
    timing_init(&timing);
    NerdArtifactConfig artifacts = compiler_cmd_default_artifacts();
    bool               ok        = compiler_cmd_run_pipeline_once(
        (NerdSource){
                                 .source      = source,
                                 .source_path = s(""),
        },
        &artifacts,
        false,
        &timing);
    timing_dump(&timing);
    timing_done(&timing);

    TimeDuration total_elapsed = time_elapsed(total_start, time_now());
    prn("million total (generate + build): %.3f ms", time_msecs(total_elapsed));

    arena_done(&source_arena);
    return ok ? 0 : 1;
}

//------------------------------------------------------------------------------
