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

bool front_end_program(NerdSource             source,
                       const FrontEndOptions* options,
                       Timing*                timing,
                       ProgramInfo*           out_program);

bool front_end_product_is_available(FrontEndProductState state);
bool front_end_product_is_complete(FrontEndProductState state);
cstr front_end_product_state_name(FrontEndProductState state);
void front_end_results_done(FrontEndState* results);
void program_info_done(ProgramInfo* program);
