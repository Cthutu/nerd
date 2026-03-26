//------------------------------------------------------------------------------
// Lex Errors
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------

bool error_0100_unexpected_character(NerdSource source, usize offset, char c)
{
    ErrorSpan span = {.start = offset, .end = offset + 1};
    ErrorInfo error_info =
        error_init(100, source, span, "Unexpected character '%c'", c);
    error_add_reference(
        &error_info, ERROR_REF_PRIMARY, span, "Unexpected character '%c'", c);
    error_add_help(&error_info, "Review the input near the failing character.");
    error_render(&error_info);
    return false;
}

bool error_0101_integer_literal_too_large(NerdSource source, ErrorSpan span)
{
    ErrorInfo error_info =
        error_init(101, source, span, "Integer literal is too large");
    error_add_reference(
        &error_info, ERROR_REF_PRIMARY, span, "Literal overflow starts here");
    error_add_help(&error_info, "Use a smaller integer literal.");
    error_render(&error_info);
    return false;
}

bool error_0102_file_too_large(NerdSource source)
{
    ErrorSpan span = {.start = 0, .end = 0};
    ErrorInfo error_info =
        error_init(102, source, span, "Source file is too large");
    error_add_help(&error_info, "Split the input into a smaller source file.");
    error_render(&error_info);
    return false;
}

bool error_0103_invalid_number_literal(NerdSource source,
                                       ErrorSpan  span,
                                       char       invalid_char)
{
    ErrorInfo error_info =
        error_init(103,
                   source,
                   span,
                   "Invalid character '%c' in number literal",
                   invalid_char);
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        span,
                        "Invalid character '%c' in number literal",
                        invalid_char);
    error_add_help(&error_info,
                   "Review the number literal for typos or insert whitespace.");
    error_render(&error_info);
    return false;
}

bool error_0104_symbol_too_long(NerdSource source, ErrorSpan span)
{
    ErrorInfo error_info = error_init(104, source, span, "Symbol is too long");
    error_add_reference(
        &error_info, ERROR_REF_PRIMARY, span, "This symbol is too long");
    error_add_note(&error_info, "Symbols must be 255 characters or fewer.");
    error_add_help(&error_info, "Use a shorter symbol name.");
    error_render(&error_info);
    return false;
}

bool error_0105_too_many_symbols(NerdSource source)
{
    ErrorSpan span = {.start = 0, .end = 0};
    ErrorInfo error_info =
        error_init(105, source, span, "Too many symbols in the source file");
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        span,
                        "This new symbol was one too many");
    error_add_note(
        &error_info,
        "The maximum size of all symbols combined is 4 GiB.  This is a hard "
        "limit because symbol data is stored in a single contiguous block of "
        "memory.  The maximum number of unique symbols is 16,777,216.");
    error_add_help(&error_info,
                   "Reduce the number of unique symbols in your program.");
    error_render(&error_info);
    return false;
}

//------------------------------------------------------------------------------
