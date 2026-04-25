//------------------------------------------------------------------------------
// Compiler front-end module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/build/build.h>

//------------------------------------------------------------------------------

bool front_end(NerdSource             source,
               const FrontEndOptions* options,
               Timing*                timing,
               FrontEndState*         out_results);

void front_end_results_done(FrontEndState* results);
