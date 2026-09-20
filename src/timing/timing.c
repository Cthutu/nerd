//------------------------------------------------------------------------------
// Timing API implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#ifndef _WIN32
#    define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <time.h>
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
    table_init(&table, columns, .title = "Timing Report");
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

// Machine-readable records deliberately avoid compiler arenas and builders.
// Workers finish value-only records; only the coordinator emits JSON.
internal bool timing_probe_enabled(void)
{
    cstr value = getenv("NERD_PROFILE");
    return value && strcmp(value, "1") == 0;
}

internal u64 timing_probe_cpu_ns(void)
{
#if OS_WINDOWS
    FILETIME created, exited, kernel, user;
    if (!GetThreadTimes(
            GetCurrentThread(), &created, &exited, &kernel, &user)) {
        return U64_MAX;
    }
    u64 ticks = ((u64)kernel.dwHighDateTime << 32) | kernel.dwLowDateTime;
    ticks += ((u64)user.dwHighDateTime << 32) | user.dwLowDateTime;
    return ticks * 100;
#elif defined(CLOCK_THREAD_CPUTIME_ID)
    struct timespec ts;
    if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) != 0) {
        return U64_MAX;
    }
    return (u64)ts.tv_sec * 1000000000ull + (u64)ts.tv_nsec;
#else
    return U64_MAX;
#endif
}

internal void timing_probe_json_string(string value)
{
    fputc('"', stderr);
    for (usize i = 0; i < value.count; ++i) {
        u8 ch = value.data[i];
        if (ch == '"' || ch == '\\') {
            fputc('\\', stderr);
            fputc(ch, stderr);
        } else if (ch < 32) {
            fprintf(stderr, "\\u%04x", ch);
        } else {
            fputc(ch, stderr);
        }
    }
    fputc('"', stderr);
}

TimingProbe timing_probe_begin(void)
{
    TimingProbe probe = {.enabled = timing_probe_enabled()};
    if (probe.enabled) {
        cstr locks         = getenv("NERD_PROFILE_LOCKS");
        probe.lock_profile = locks && strcmp(locks, "1") == 0;
        if (probe.lock_profile) {
            probe.previous_lock_profile = mem_lock_profile_select(true);
            probe.locks                 = mem_lock_profile_snapshot();
        }
        probe.memory = mem_stats_thread_snapshot();
        probe.cpu_ns = timing_probe_cpu_ns();
        probe.wall   = time_now();
    }
    return probe;
}

TimingProbeResult timing_probe_finish(TimingProbe probe)
{
    if (!probe.enabled) {
        return (TimingProbeResult){0};
    }
    TimePoint       end      = time_now();
    u64             cpu      = timing_probe_cpu_ns();
    MemoryStats     activity = mem_stats_thread_snapshot();
    MemoryLockStats locks    = {0};
    if (probe.lock_profile) {
        locks = mem_lock_profile_snapshot();
        locks.acquisitions -= probe.locks.acquisitions;
        locks.acquire_ns -= probe.locks.acquire_ns;
        mem_lock_profile_select(probe.previous_lock_profile);
    }
    MemoryStats process = mem_stats_snapshot();
    return (TimingProbeResult){
        .enabled         = true,
        .lock_profile    = probe.lock_profile,
        .locks           = locks,
        .start_ns        = (u64)time_nsecs(probe.wall),
        .wall_ns         = time_duration_to_ns(time_elapsed(probe.wall, end)),
        .cpu_ns          = cpu == U64_MAX || probe.cpu_ns == U64_MAX
                               ? U64_MAX
                               : cpu - probe.cpu_ns,
        .activity        = mem_stats_delta(probe.memory, activity),
        .heap_live_bytes = process.heap_current_bytes,
        .heap_peak_bytes = process.heap_peak_bytes,
    };
}

void timing_probe_emit(TimingProbeResult result,
                       cstr              stage,
                       cstr              phase,
                       string            module,
                       bool              success,
                       usize             output_bytes)
{
    if (!result.enabled) {
        return;
    }
    MemoryStats delta = result.activity;
    fputs("nerd-profile\t{\"kind\":\"phase\",\"stage\":", stderr);
    timing_probe_json_string(s(stage));
    fputs(",\"phase\":", stderr);
    timing_probe_json_string(s(phase));
    fputs(",\"module\":", stderr);
    timing_probe_json_string(module);
    fprintf(stderr,
            ",\"start_ns\":%llu,\"wall_ns\":%llu,\"cpu_ns\":",
            (unsigned long long)result.start_ns,
            (unsigned long long)result.wall_ns);
    if (result.cpu_ns == U64_MAX) {
        fputs("null", stderr);
    } else {
        fprintf(stderr, "%llu", (unsigned long long)result.cpu_ns);
    }
    fprintf(
        stderr,
        ",\"success\":%s,\"output_bytes\":%zu,\"heap_allocs\":%zu,"
        "\"heap_reallocs\":%zu,\"heap_live_bytes\":%zu,\"heap_peak_bytes\":%zu,"
        "\"arena_requested_bytes\":%zu,\"arena_committed_bytes\":%zu,"
        "\"array_growths\":%zu",
        success ? "true" : "false",
        output_bytes,
        delta.heap_alloc_count,
        delta.heap_realloc_count,
        result.heap_live_bytes,
        result.heap_peak_bytes,
        delta.arena_bytes_allocated,
        delta.arena_bytes_committed,
        delta.array_growth_count);
    if (result.lock_profile) {
        fprintf(stderr,
                ",\"memory_lock_acquisitions\":%llu,\"memory_lock_acquire_ns\":"
                "%llu",
                (unsigned long long)result.locks.acquisitions,
                (unsigned long long)result.locks.acquire_ns);
    }
    fputs("}\n", stderr);
}

void timing_probe_end(TimingProbe probe,
                      cstr        stage,
                      cstr        phase,
                      string      module,
                      bool        success,
                      usize       output_bytes)
{
    timing_probe_emit(timing_probe_finish(probe),
                      stage,
                      phase,
                      module,
                      success,
                      output_bytes);
}

bool timing_probe_self_test(void)
{
    TimingProbe probe = timing_probe_begin();
    if (!probe.enabled) {
        return false;
    }
    void*             block = mem_alloc(37, __FILE__, __LINE__);
    TimingProbeResult first = timing_probe_finish(probe);
    // A second task's work must not change the finished result. Results may
    // be emitted later in coordinator order rather than completion order.
    probe                   = timing_probe_begin();
    block                   = mem_realloc(block, 83, __FILE__, __LINE__);
    mem_free(block, __FILE__, __LINE__);
    TimingProbeResult second = timing_probe_finish(probe);
    if (first.activity.heap_alloc_count != 1 ||
        first.activity.heap_bytes_allocated != 37 ||
        first.activity.heap_realloc_count != 0 ||
        first.activity.heap_free_count != 0 ||
        second.activity.heap_alloc_count != 0 ||
        second.activity.heap_realloc_count != 1 ||
        second.activity.heap_bytes_freed != 83) {
        return false;
    }
    // No profile output is allowed before this marker.
    fputs("profile-results-ready\n", stderr);
    timing_probe_emit(second, "test", "second", s("quoted\"module"), false, 83);
    timing_probe_emit(first, "test", "first", s("first\nmodule"), true, 37);
    timing_probe_emit(
        (TimingProbeResult){0}, "test", "disabled", s(""), true, 0);
    return true;
}

void timing_probe_dependency(string module, string dependency)
{
    if (!timing_probe_enabled()) {
        return;
    }
    fputs("nerd-profile\t{\"kind\":\"dependency\",\"module\":", stderr);
    timing_probe_json_string(module);
    fputs(",\"dependency\":", stderr);
    timing_probe_json_string(dependency);
    fputs("}\n", stderr);
}
