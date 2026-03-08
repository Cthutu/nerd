//------------------------------------------------------------------------------
// Compiler state dump
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>

//------------------------------------------------------------------------------

void compiler_dump(const FrontEndResults* front_end_results,
                   const BackEndResults*  back_end_results)
{
    lex_dump(&front_end_results->lexer);
    ast_dump(&front_end_results->ast, &front_end_results->lexer);
    ir_dump(&front_end_results->ir);

    UNUSED(back_end_results);
}

//------------------------------------------------------------------------------
