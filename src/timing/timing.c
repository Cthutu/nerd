//------------------------------------------------------------------------------
// Timing API implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <timing/timing.h>

#include <table/table.h>

//------------------------------------------------------------------------------
// Timing Data

//------------------------------------------------------------------------------

void timing_init(Timing* timing)
{
    *timing = (Timing){0};
}

void timing_done(Timing* timing)
{
    array_free(timing->timings);
    array_free(timing->totals);
    *timing = (Timing){0};
}

void timing_add(Timing* timing, cstr stage, cstr phase, TimeDuration time)
{
    array_push(timing->timings,
               (TimingEntry){.stage = stage, .phase = phase, .time = time});

    bool found = false;
    for (usize i = 0; i < array_count(timing->totals); i++) {
        if (timing->totals[i].stage == stage) {
            timing->totals[i].total_time += time;
            found = true;
            break;
        }
    }
    if (!found) {
        array_push(
            timing->totals, (TimingTotal){.stage = stage, .total_time = time});
    }
}

//------------------------------------------------------------------------------

internal void duration_text(StringBuilder* sb, TimeDuration duration)
{
    sb_format(sb, "%.03f μs", time_usecs(duration));
}

internal void table_add_timing_row(Table*       table,
                                   Arena*       arena,
                                   cstr         stage,
                                   cstr         phase,
                                   TimeDuration duration,
                                   bool         divider_before)
{
    Array(TableCell) cells = NULL;
    array_requires_capacity(cells, 3);
    array_push(cells, table_cell_text(s(stage)), table_cell_text(s(phase)));

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    duration_text(&sb, duration);
    array_push(cells, table_cell_text(sb_to_string(&sb)));

    table_add_row(table, cells, .divider_before = divider_before);
    array_free(cells);
}

internal void table_add_timing_total_row(Table*       table,
                                         Arena*       arena,
                                         cstr         stage,
                                         string       phase,
                                         TimeDuration duration,
                                         cstr         stage_colour,
                                         cstr         phase_colour,
                                         cstr         time_colour,
                                         bool         divider_before)
{
    Array(TableCell) cells = NULL;
    array_requires_capacity(cells, 3);
    array_push(cells, table_cell_text(s(stage)), table_cell_text(phase));

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    duration_text(&sb, duration);
    array_push(cells, table_cell_text(sb_to_string(&sb)));

    cstr colours[] = {stage_colour, phase_colour, time_colour};
    table_add_row(
        table, cells, .colours = colours, .divider_before = divider_before);
    array_free(cells);
}

void timing_dump(const Timing* timing)
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

    usize totals_count          = array_count(timing->totals);
    usize timings_count         = array_count(timing->timings);

    TimeDuration compiler_total = time_from_secs(0);
    for (usize i = 0; i < totals_count; i++) {
        compiler_total += timing->totals[i].total_time;
    }

    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "stage", .colour = stage_colour},
               (TableColumn){.title = "phase", .colour = phase_colour},
               (TableColumn){.title = "duration", .colour = time_colour});
    Table table;
    table_init(&table, columns);
    array_free(columns);
    table_reserve_rows(&table, timings_count + totals_count + 1);

    Arena arena = {0};
    arena_init(&arena);

    for (usize i = 0; i < totals_count; i++) {
        cstr stage                 = timing->totals[i].stage;
        bool first_phase_for_stage = true;
        for (usize j = 0; j < timings_count; j++) {
            if (timing->timings[j].stage != stage) {
                continue;
            }
            table_add_timing_row(&table,
                                 &arena,
                                 timing->timings[j].stage,
                                 timing->timings[j].phase,
                                 timing->timings[j].time,
                                 first_phase_for_stage && i > 0);
            first_phase_for_stage = false;
        }

        StringBuilder total_phase_sb = {0};
        sb_init(&total_phase_sb, &arena);
        sb_format(&total_phase_sb, "%s total", stage);
        table_add_timing_total_row(&table,
                                   &arena,
                                   stage,
                                   sb_to_string(&total_phase_sb),
                                   timing->totals[i].total_time,
                                   total_stage_colour,
                                   total_phase_colour,
                                   total_time_colour,
                                   false);
    }

    table_add_timing_total_row(&table,
                               &arena,
                               "compiler",
                               s("compiler total"),
                               compiler_total,
                               total_stage_colour,
                               total_phase_colour,
                               total_time_colour,
                               true);

    prn("\n--- Timings ---");
    table_print(&table, border_colour, header_colour, reset);

    table_done(&table);
    arena_done(&arena);
}

//------------------------------------------------------------------------------
