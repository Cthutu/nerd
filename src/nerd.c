//
// Hello test
//
//> use: core intern compiler timing table cli

#include <cli/cli.h>
#include <compiler/compiler.h>
#include <table/table.h>
#include <timing/timing.h>

//------------------------------------------------------------------------------

typedef struct {
    bool benchmark;
    bool million;
} NerdConfig;

enum {
    LEX_WARMUP_ITERATIONS = 1000,
    LEX_TIMED_ITERATIONS  = 10000,
    MILLION_LINES_COUNT   = 1000000,
};

internal string make_million_lines_source(Arena* arena)
{
    usize total_bytes = MILLION_LINES_COUNT * 3;
    u8*   data        = (u8*)arena_alloc(arena, total_bytes);
    for (usize i = 0; i < MILLION_LINES_COUNT; i++) {
        usize offset     = i * 3;
        data[offset + 0] = '4';
        data[offset + 1] = '2';
        data[offset + 2] = '\n';
    }
    return string_from(data, total_bytes);
}

internal void print_source_overview(string source_code)
{
    if (source_code.count <= 128) {
        prn("Source code: " STRINGP, STRINGV(source_code));
    } else {
        prn("Source code size: %zu bytes", source_code.count);
    }
}

internal cstr run_mode_name(NerdConfig config)
{
    if (config.benchmark && config.million) {
        return "million+benchmark";
    }
    if (config.benchmark) {
        return "benchmark";
    }
    if (config.million) {
        return "million";
    }
    return "normal";
}

internal NerdConfig parse_config(int argc, char** argv)
{
    NerdConfig config = {0};
    CliParser  parser = {0};
    cli_init(&parser, "nerd", "Nerd compiler playground");

    cli_add_flag(&parser,
                 'b',
                 "benchmark",
                 "Run 10000 lex iterations and report average/total timing",
                 &config.benchmark);
    cli_add_flag(&parser,
                 'm',
                 "million",
                 "Generate 1,000,000 lines and skip token dump",
                 &config.million);

    CliParseResult parse_result = cli_parse(&parser, argc, argv);
    if (parse_result.help_requested) {
        cli_print_help(&parser);
        cli_done(&parser);
        exit(0);
    }

    cli_done(&parser);
    return config;
}

//------------------------------------------------------------------------------
// Entry point for the test.

int run(int argc, char** argv)
{
    NerdConfig config = parse_config(argc, argv);

    //
    // Dump system information and run mode.
    //

    dump_info();
    prn("Run mode: %s", run_mode_name(config));

    //
    // Set up build parameters, including source code
    //

    cstr source_code = "42";
    SLICE_SET(libs, cstr, "kernel32.lib");
    cstr output_base       = "test";
    u32  run_count         = config.benchmark ? LEX_TIMED_ITERATIONS : (u32)1;

    //
    // Generate a million lines of source if we're in this mode
    //

    Arena  benchmark_arena = {0};
    string source          = string_from_cstr(source_code);
    if (config.million) {
        arena_init(&benchmark_arena);
        source = make_million_lines_source(&benchmark_arena);
        prn("Generated benchmark input: %u lines (%zu bytes)",
            MILLION_LINES_COUNT,
            source.count);
    }

    print_source_overview(source);

    TimingAccumulateSession session = {0};
    timing_accumulate_session_init(&session);
    if (config.benchmark) {

        //
        // Benchmarked run (accurate per-phase averages)
        //

        Timing benchmark_timing = {0};
        front_end_benchmark(
            source, LEX_WARMUP_ITERATIONS, run_count, &benchmark_timing);
        timing_accumulate_session_add(&session, &benchmark_timing);
        timing_done(&benchmark_timing);

        //
        // One extra untimed run to inspect compiler state while benchmarking.
        //

        FrontEndResults dump_front = front_end(source, NULL);
        BackEndResults  dump_back  = back_end(&dump_front, NULL);
        if (!config.million) {
            compiler_dump(&dump_front, &dump_back);
        }
        back_end_results_done(&dump_back);
        front_end_results_done(&dump_front);
    } else {

        //
        // Single run
        //

        Timing iteration_timing = {0};
        timing_init(&iteration_timing);
        FrontEndResults results = front_end(source, &iteration_timing);
        BackEndResults  back    = back_end(&results, &iteration_timing);
        if (!config.million) {
            compiler_dump(&results, &back);
        }
        back_end_results_done(&back);
        front_end_results_done(&results);

        timing_accumulate_session_add(&session, &iteration_timing);
        timing_done(&iteration_timing);
    }

    //
    // Timings processing and output
    //

    Timing report = {0};
    timing_accumulate_session_build_report(&session, &report);
    timing_dump(&report);
    timing_done(&report);
    timing_accumulate_session_done(&session);

    //
    // Clean up
    //

    UNUSED(libs);
    UNUSED(output_base);
    if (config.million) {
        arena_done(&benchmark_arena);
    }

    return 0;
}
