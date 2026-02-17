//------------------------------------------------------------------------------
// Front-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>

//------------------------------------------------------------------------------

FrontEndResults front_end(string source_code, Timing* timing)
{
    FrontEndResults results = {0};
    PHASE_DO(timing,
             COMPILER_STAGE_FRONT_END,
             COMPILER_PHASE_LEX,
             results.lexer = lex(source_code));
    return results;
}

void front_end_results_done(FrontEndResults* results)
{
    lex_done(&results->lexer);
    *results = (FrontEndResults){0};
}

//------------------------------------------------------------------------------
