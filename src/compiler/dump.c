//------------------------------------------------------------------------------
// Compiler state dump
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>

//------------------------------------------------------------------------------

void compiler_dump(const FrontEndState* front_end_results,
                   const BackEndState*  back_end_results)
{
    lex_dump(&front_end_results->lexer);
    ast_dump(&front_end_results->ast, &front_end_results->lexer);
    ir_dump(&front_end_results->ir);
    cgen_dump(&front_end_results->cgen);

    UNUSED(back_end_results);
}

//------------------------------------------------------------------------------
