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
                                            Timing*                   out_report);
void timing_accumulate_session_done(TimingAccumulateSession* session);
void timing_dump(const Timing* timing);

//------------------------------------------------------------------------------
