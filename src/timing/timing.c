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

void timing_init(Timing* timing) { *timing = (Timing){0}; }

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
        array_push(timing->totals,
                   (TimingTotal){.stage = stage, .total_time = time});
    }
}

internal void timing_accumulate(Timing* dst, const Timing* src)
{
    for (usize i = 0; i < array_count(src->timings); i++) {
        cstr         stage = src->timings[i].stage;
        cstr         phase = src->timings[i].phase;
        TimeDuration time  = src->timings[i].time;

        bool found_phase   = false;
        for (usize j = 0; j < array_count(dst->timings); j++) {
            if (strcmp(dst->timings[j].stage, stage) == 0 &&
                strcmp(dst->timings[j].phase, phase) == 0) {
                dst->timings[j].time += time;
                found_phase = true;
                break;
            }
        }
        if (!found_phase) {
            array_push(
                dst->timings,
                (TimingEntry){.stage = stage, .phase = phase, .time = time});
        }

        bool found_stage = false;
        for (usize j = 0; j < array_count(dst->totals); j++) {
            if (strcmp(dst->totals[j].stage, stage) == 0) {
                dst->totals[j].total_time += time;
                found_stage = true;
                break;
            }
        }
        if (!found_stage) {
            array_push(dst->totals,
                       (TimingTotal){.stage = stage, .total_time = time});
        }
    }
}

void timing_accumulate_session_init(TimingAccumulateSession* session)
{
    session->count = 0;
    timing_init(&session->aggregate);
}

void timing_accumulate_session_add(TimingAccumulateSession* session,
                                   const Timing*            timing)
{
    if (array_count(timing->timings) == 0 && array_count(timing->totals) == 0) {
        return;
    }

    timing_accumulate(&session->aggregate, timing);
    session->count++;
}

void timing_accumulate_session_build_report(TimingAccumulateSession* session,
                                            Timing*                  out_report)
{
    timing_init(out_report);

    if (session->count == 0) {
        return;
    }

    for (usize i = 0; i < array_count(session->aggregate.timings); i++) {
        cstr         stage = session->aggregate.timings[i].stage;
        cstr         phase = session->aggregate.timings[i].phase;
        TimeDuration total = session->aggregate.timings[i].time;
        TimeDuration avg   = total / (TimeDuration)session->count;
        timing_add(out_report, stage, phase, avg);
    }
}

void timing_accumulate_session_done(TimingAccumulateSession* session)
{
    timing_done(&session->aggregate);
    *session = (TimingAccumulateSession){0};
}

//------------------------------------------------------------------------------

internal void table_add_timing_row(Table*       table,
                                   cstr         stage,
                                   cstr         phase,
                                   TimeDuration duration,
                                   bool         divider_before)
{
    Array(TableCell) cells = NULL;
    array_requires_capacity(cells, 3);
    array_push(cells,
               table_cell_string(s(stage)),
               table_cell_string(s(phase)),
               table_cell_time(duration));

    table_add_row(table, cells, .divider_before = divider_before);
    array_free(cells);
}

internal void table_add_timing_total_row(Table*       table,
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
    array_push(cells,
               table_cell_string(s(stage)),
               table_cell_string(phase),
               table_cell_time(duration));

    cstr colours[] = {stage_colour, phase_colour, time_colour};
    table_add_row(
        table, cells, .colours = colours, .divider_before = divider_before);
    array_free(cells);
}

void timing_dump(const Timing* timing)
{
    cstr stage_colour           = "\x1b[38;5;214m";
    cstr phase_colour           = "\x1b[38;5;111m";
    cstr time_colour            = "\x1b[38;5;82m";
    cstr total_stage_colour     = "\x1b[1;38;5;220m";
    cstr total_phase_colour     = "\x1b[1;38;5;222m";
    cstr total_time_colour      = "\x1b[1;38;5;229m";

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
    table_set_title(&table, "Timing Report");
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
                                   stage,
                                   sb_to_string(&total_phase_sb),
                                   timing->totals[i].total_time,
                                   total_stage_colour,
                                   total_phase_colour,
                                   total_time_colour,
                                   false);
    }

    table_add_timing_total_row(&table,
                               "compiler",
                               s("compiler total"),
                               compiler_total,
                               total_stage_colour,
                               total_phase_colour,
                               total_time_colour,
                               true);

    table_print(&table);

    table_done(&table);
    arena_done(&arena);
}

//------------------------------------------------------------------------------
