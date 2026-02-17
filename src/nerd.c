//
// Hello test
//
//> use: core intern compiler table

#include <compiler/lexer/lexer.h>
#include <table/table.h>

DEF_SLICE(cstr) strings;

//------------------------------------------------------------------------------
// Front End

typedef struct {
    Lexer lexer;

    TimeDuration lex_time;
} FrontEndResults;

typedef struct {
} BackEndResults;

FrontEndResults front_end(string source_code)
{
    FrontEndResults results = {0};

    prn("Source code: " STRINGP, STRINGV(source_code));

    //
    // Lexing
    //

    prn("\n-- Lexing --");
    TimePoint lex_start = time_now();
    results.lexer       = lex(source_code);
    results.lex_time    = time_elapsed(lex_start, time_now());
    lex_dump(&results.lexer);

    //
    // Done
    //

    lex_done(&results.lexer);
    return results;
}

//------------------------------------------------------------------------------
// Back End

//------------------------------------------------------------------------------
// Timings

void duration_text(StringBuilder* sb, TimeDuration duration)
{
    sb_format(sb, "%.03f μs", time_usecs(duration));
}

void table_add_timing_row(
    Table* table, Arena* arena, cstr stage, cstr phase, TimeDuration duration)
{
    Array(TableCell) cells = NULL;
    array_requires_capacity(cells, 3);
    array_push(cells, table_cell_text(s(stage)), table_cell_text(s(phase)));

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    duration_text(&sb, duration);
    array_push(cells, table_cell_text(sb_to_string(&sb)));

    table_add_row(table, cells);
    array_free(cells);
}

void table_add_timing_total_row(Table*       table,
                                Arena*       arena,
                                cstr         stage,
                                cstr         phase,
                                TimeDuration duration,
                                cstr         stage_colour,
                                cstr         phase_colour,
                                cstr         time_colour,
                                bool         divider_before)
{
    Array(TableCell) cells = NULL;
    array_requires_capacity(cells, 3);
    array_push(cells, table_cell_text(s(stage)), table_cell_text(s(phase)));

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    duration_text(&sb, duration);
    array_push(cells, table_cell_text(sb_to_string(&sb)));

    cstr colours[] = {stage_colour, phase_colour, time_colour};
    table_add_row(
        table, cells, .colours = colours, .divider_before = divider_before);
    array_free(cells);
}

TimeDuration front_end_total(FrontEndResults* results)
{
    return results->lex_time;
}

TimeDuration back_end_total(BackEndResults* results)
{
    UNUSED(results);
    return time_from_secs(0);
}

void time_dump(FrontEndResults* front,
               BackEndResults*  back,
               TimeDuration     run_time)
{
    cstr border_colour          = "\x1b[38;5;245m";
    cstr header_colour          = "\x1b[1;38;5;45m";
    cstr stage_colour           = "\x1b[38;5;214m";
    cstr phase_colour           = "\x1b[38;5;111m";
    cstr time_colour            = "\x1b[38;5;82m";
    cstr total_stage_colour     = "\x1b[1;38;5;220m";
    cstr total_phase_colour     = "\x1b[1;38;5;222m";
    cstr total_time_colour      = "\x1b[1;38;5;229m";
    cstr reset                  = ANSI_RESET;

    TimeDuration compiler_total = front_end_total(front) + back_end_total(back);

    Array(TableColumn) columns  = NULL;
    array_push(columns,
               (TableColumn){.title = "stage", .colour = stage_colour},
               (TableColumn){.title = "phase", .colour = phase_colour},
               (TableColumn){.title = "duration", .colour = time_colour});
    Table table;
    table_init(&table, columns);
    array_free(columns);
    table_reserve_rows(&table, 10);

    Arena arena = {0};
    arena_init(&arena);

    table_add_timing_row(
        &table, &arena, "front-end", "tokenise source text", front->lex_time);

    table_add_timing_total_row(&table,
                               &arena,
                               "front-end",
                               "front-end total",
                               front_end_total(front),
                               total_stage_colour,
                               total_phase_colour,
                               total_time_colour,
                               false);

    table_add_timing_total_row(&table,
                               &arena,
                               "back-end",
                               "back-end total",
                               back_end_total(back),
                               total_stage_colour,
                               total_phase_colour,
                               total_time_colour,
                               false);

    table_add_timing_total_row(&table,
                               &arena,
                               "compiler",
                               "compiler total",
                               compiler_total,
                               total_stage_colour,
                               total_phase_colour,
                               total_time_colour,
                               true);

    prn("\n--- Timings ---");
    table_print(&table, border_colour, header_colour, reset);

    StringBuilder sb = {0};
    sb_init(&sb, &arena);
    duration_text(&sb, run_time);
    string run_time_text = sb_to_string(&sb);

    prn("%sruntime%s: %sexecute output program%s -> %s%.*s%s",
        total_stage_colour,
        reset,
        total_phase_colour,
        reset,
        total_time_colour,
        STRINGV(run_time_text),
        reset);

    table_done(&table);
    arena_done(&arena);
}

//------------------------------------------------------------------------------
// Entry point for the test.

int run(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    cstr source_code = "42";
    SLICE_SET(libs, cstr, "kernel32.lib");
    cstr output_base = "test";

    FrontEndResults front_end_results =
        front_end(string_from_cstr(source_code));

    BackEndResults back_end_results = {};
    TimeDuration   run_time         = time_from_secs(0);

    time_dump(&front_end_results, &back_end_results, run_time);

    UNUSED(libs);
    UNUSED(output_base);
    UNUSED(front_end_results);

    return 0;
}
