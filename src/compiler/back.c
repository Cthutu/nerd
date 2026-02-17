//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>

//------------------------------------------------------------------------------

BackEndResults back_end(const FrontEndResults* front_end_results,
                        Timing*                timing)
{
    BackEndResults results;

    UNUSED(front_end_results);
    UNUSED(timing);
    return results;
}

void back_end_results_done(BackEndResults* results) { UNUSED(results); }

//------------------------------------------------------------------------------
