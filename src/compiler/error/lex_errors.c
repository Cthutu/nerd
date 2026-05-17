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
        error_init(source, span, "Unexpected character '%c'", c);
    error_add_reference(
        &error_info, ERROR_REF_PRIMARY, span, "Unexpected character '%c'", c);
    error_add_help(&error_info, "Review the input near the failing character.");
    error_render(&error_info);
    return false;
}

bool error_0101_integer_literal_too_large(NerdSource source, ErrorSpan span)
{
    ErrorInfo error_info =
        error_init(source, span, "Integer literal is too large");
    error_add_reference(
        &error_info, ERROR_REF_PRIMARY, span, "Literal overflow starts here");
    error_add_help(&error_info, "Use a smaller integer literal.");
    error_render(&error_info);
    return false;
}

bool error_0102_file_too_large(NerdSource source)
{
    ErrorSpan span       = {.start = 0, .end = 0};
    ErrorInfo error_info = error_init(source, span, "Source file is too large");
    error_add_help(&error_info, "Split the input into a smaller source file.");
    error_render(&error_info);
    return false;
}

bool error_0103_invalid_number_literal(NerdSource source,
                                       ErrorSpan  span,
                                       char       invalid_char)
{
    ErrorInfo error_info = error_init(
        source, span, "Invalid character '%c' in number literal", invalid_char);
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
    ErrorInfo error_info = error_init(source, span, "Symbol is too long");
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
        error_init(source, span, "Too many symbols in the source file");
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

bool error_0106_unterminated_string_literal(NerdSource source, ErrorSpan span)
{
    ErrorInfo error_info =
        error_init(source, span, "Unterminated string literal");
    error_add_reference(
        &error_info, ERROR_REF_PRIMARY, span, "String literal starts here");
    error_add_help(&error_info,
                   "Add a closing double quote to terminate the string "
                   "literal.");
    error_add_help(&error_info,
                   "Use `+\"...\"` for an intentional string continuation.");
    error_render(&error_info);
    return false;
}

bool error_0107_unterminated_packed_integer_literal(NerdSource source,
                                                    ErrorSpan  span)
{
    ErrorInfo error_info =
        error_init(source, span, "Unterminated packed integer literal");
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        span,
                        "Packed integer literal starts here");
    error_add_help(&error_info,
                   "Add a closing single quote to terminate the literal.");
    error_render(&error_info);
    return false;
}

bool error_0108_packed_integer_literal_too_large(NerdSource source,
                                                 ErrorSpan  span)
{
    ErrorInfo error_info =
        error_init(source, span, "Packed integer literal is too large");
    error_add_reference(&error_info,
                        ERROR_REF_PRIMARY,
                        span,
                        "This literal exceeds the 8-byte limit");
    error_add_help(&error_info,
                   "Use at most 8 bytes inside one single-quote literal.");
    error_render(&error_info);
    return false;
}

//------------------------------------------------------------------------------
