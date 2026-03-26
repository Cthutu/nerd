//------------------------------------------------------------------------------
// Compiler back-end module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/build/build.h>

//------------------------------------------------------------------------------

bool back_end(const FrontEndState*      front_end_results,
              const NerdArtifactConfig* artifacts,
              bool                      verbose,
              Timing*                   timing,
              BackEndState*             out_results);
void back_end_results_done(BackEndState* results);
