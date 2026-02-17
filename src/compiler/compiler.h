//------------------------------------------------------------------------------
// Compiler module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: lexer timing

#pragma once

#include <compiler/lexer/lexer.h>
#include <timing/timing.h>

//------------------------------------------------------------------------------

typedef struct {
    Lexer lexer;
} FrontEndResults;

typedef struct {
} BackEndResults;

FrontEndResults front_end(string source_code, Timing* timing);
void            front_end_results_done(FrontEndResults* results);

BackEndResults back_end(const FrontEndResults* front_end_results,
                        Timing*                timing);
void           back_end_results_done(BackEndResults* results);

void compiler_dump(bool                   dump_lexer,
                   const FrontEndResults* front_end_results,
                   const BackEndResults*  back_end_results);

//------------------------------------------------------------------------------
