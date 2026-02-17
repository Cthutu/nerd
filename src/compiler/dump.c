//------------------------------------------------------------------------------
// Compiler state dump
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>

//------------------------------------------------------------------------------

void compiler_dump(bool                   dump_lexer,
                   const FrontEndResults* front_end_results,
                   const BackEndResults*  back_end_results)
{
    if (dump_lexer) {
        Lexer lexer = front_end_results->lexer;
        lex_dump(&lexer);
    }

    UNUSED(back_end_results);
}

//------------------------------------------------------------------------------
