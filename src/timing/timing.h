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

// Opt-in serial profiling stream; independent of human-readable --timing.
// CPU time excludes child processes; U64_MAX means unavailable.
typedef struct {
    bool        enabled;
    TimePoint   wall;
    u64         cpu_ns;
    MemoryStats memory;
} TimingProbe;

TimingProbe timing_probe_begin(void);
void        timing_probe_end(TimingProbe probe,
                             cstr        stage,
                             cstr        phase,
                             string      module,
                             bool        success,
                             usize       output_bytes);
void        timing_probe_dependency(string module, string dependency);
