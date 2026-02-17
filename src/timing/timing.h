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

void timing_init(Timing* timing);
void timing_done(Timing* timing);
void timing_add(Timing* timing, cstr stage, cstr phase, TimeDuration time);
void timing_dump(const Timing* timing);

//------------------------------------------------------------------------------
