//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>

//------------------------------------------------------------------------------

BackEndState back_end(const FrontEndState* front_end_results, Timing* timing)
{
    BackEndState results;

    UNUSED(front_end_results);
    UNUSED(timing);

    cgen_save(&front_end_results->cgen, "_output.c");
#if OS_POSIX
    shell("clang -o _output _output.c");
#elif OS_WINDOWS
    shell("clang -o _output.exe _output.c");
#endif

    return results;
}

void back_end_results_done(BackEndState* results) { UNUSED(results); }

//------------------------------------------------------------------------------
