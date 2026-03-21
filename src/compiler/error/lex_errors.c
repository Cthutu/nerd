//------------------------------------------------------------------------------
// Lex Errors
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------

bool error_0100_unexpected_character(NerdSource source, usize offset, char c)
{
    ErrorInfo error_info =
        error_init(100, source, offset, "Unexpected character '%c'", c);
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        offset,
                        1,
                        "Unexpected character '%c'",
                        c);
    error_add_help(&error_info, "Review the input near the failing character.");
    error_render(&error_info);
    return false;
}

bool error_0101_integer_literal_too_large(NerdSource source, usize offset)
{
    ErrorInfo error_info =
        error_init(101, source, offset, "Integer literal is too large");
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        offset,
                        1,
                        "Literal overflow starts here");
    error_add_help(&error_info, "Use a smaller integer literal.");
    error_render(&error_info);
    return false;
}

bool error_0102_file_too_large(NerdSource source)
{
    ErrorInfo error_info =
        error_init(102, source, 0, "Source file is too large");
    error_add_help(&error_info, "Split the input into a smaller source file.");
    error_render(&error_info);
    return false;
}

//------------------------------------------------------------------------------
