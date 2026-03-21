//------------------------------------------------------------------------------
// Compiler front-end module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/build/build.h>

//------------------------------------------------------------------------------

bool front_end(NerdSource source, Timing* timing, FrontEndState* out_results);
void front_end_benchmark(NerdSource source,
                         u32        warmup_iterations,
                         u32        timed_iterations,
                         Timing*    out_timing);

void front_end_results_done(FrontEndState* results);
