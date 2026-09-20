//------------------------------------------------------------------------------
// Timing module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core table

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------

typedef struct {
    cstr         stage;
    cstr         phase;
    TimeDuration time;
} TimingEntry;

typedef struct {
    cstr         stage;
    TimeDuration total_time;
} TimingTotal;

typedef struct {
    Array(TimingEntry) timings;
    Array(TimingTotal) totals;
} Timing;

typedef struct {
    Timing aggregate;
    u32    count;
} TimingAccumulateSession;

void timing_init(Timing* timing);
void timing_done(Timing* timing);
void timing_add(Timing* timing, cstr stage, cstr phase, TimeDuration time);
void timing_accumulate_session_init(TimingAccumulateSession* session);
void timing_accumulate_session_add(TimingAccumulateSession* session,
                                   const Timing*            timing);
void timing_accumulate_session_build_report(TimingAccumulateSession* session,
                                            Timing* out_report);
void timing_accumulate_session_done(TimingAccumulateSession* session);
void timing_dump(const Timing* timing);

//------------------------------------------------------------------------------

// Opt-in profiling; independent of human-readable --timing.
// A probe must begin and finish on one thread without executing other tasks.
// CPU time excludes child processes; U64_MAX means unavailable.
typedef struct {
    bool        enabled;
    TimePoint   wall;
    u64         cpu_ns;
    MemoryStats memory;
} TimingProbe;

// Value-only result owned by the task. It retains no input or scratch pointers.
// Activity is task-local; heap live/peak values are process-wide observations.
typedef struct {
    bool        enabled;
    u64         start_ns;
    u64         wall_ns;
    u64         cpu_ns;
    MemoryStats activity;
    usize       heap_live_bytes;
    usize       heap_peak_bytes;
} TimingProbeResult;

TimingProbe       timing_probe_begin(void);
TimingProbeResult timing_probe_finish(TimingProbe probe);
// Emit only on the coordinator, with labels valid for the duration of this
// call.
void              timing_probe_emit(TimingProbeResult result,
                                    cstr              stage,
                                    cstr              phase,
                                    string            module,
                                    bool              success,
                                    usize             output_bytes);
bool              timing_probe_self_test(void);
void              timing_probe_end(TimingProbe probe,
                                   cstr        stage,
                                   cstr        phase,
                                   string      module,
                                   bool        success,
                                   usize       output_bytes);
void              timing_probe_dependency(string module, string dependency);
