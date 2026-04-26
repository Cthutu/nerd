//------------------------------------------------------------------------------
// AST errors
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------

bool error_0200_code_too_complex(NerdSource source, ErrorSpan span)
{
    ErrorInfo error = error_init(200, source, span, "Code is too complex");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This expression exceeds AST limits");
    error_add_help(
        &error, "Try breaking the expression into smaller intermediate values");
    error_render(&error);
    return false;
}

bool error_0201_missing_value(NerdSource source,
                              ErrorSpan  span,
                              TokenKind  expected_kind)
{
    string    token = token_kind_to_string(expected_kind);
    ErrorInfo error = error_init(
        201, source, span, "Missing value before %.*s", STRINGV(token));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "%.*s cannot appear here",
                        STRINGV(token));
    if (expected_kind == TK_Colon) {
        error_add_note(
            &error,
            "A colon starts a type annotation or `:=` binding, so the parser "
            "was still expecting an expression value here");
        error_add_help(
            &error,
            "If you meant to update an existing value, use `=` instead of "
            "`:=`");
    } else {
        error_add_help(
            &error,
            "Insert a literal, parenthesized expression, or unary operator");
    }
    error_render(&error);
    return false;
}

bool error_0202_missing_operator(NerdSource source,
                                 ErrorSpan  span,
                                 TokenKind  expected_kind)
{
    string    token = token_kind_to_string(expected_kind);
    ErrorInfo error = error_init(
        202, source, span, "Missing operator before %.*s", STRINGV(token));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "%.*s starts a new expression here",
                        STRINGV(token));
    error_add_help(
        &error, "Insert an operator such as +, -, *, /, or %% between values");
    error_render(&error);
    return false;
}

bool error_0203_expected_token(NerdSource source,
                               ErrorSpan  span,
                               TokenKind  expected_kind,
                               TokenKind  actual_kind)
{
    string    expected = token_kind_to_string(expected_kind);
    string    actual   = token_kind_to_string(actual_kind);
    ErrorInfo error =
        error_init(203, source, span, "Expected %.*s", STRINGV(expected));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "Found %.*s here", STRINGV(actual));
    error_add_help(
        &error, "Check for a missing closing delimiter or misplaced operator");
    error_render(&error);
    return false;
}

bool error_0204_unexpected_token(
    NerdSource source, ErrorSpan span, TokenKind actual_kind, cstr format, ...)
{
    string    actual = token_kind_to_string(actual_kind);
    ErrorInfo error =
        error_init(204, source, span, "Unexpected token after expression");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "Found %.*s here", STRINGV(actual));

    if (actual_kind == TK_RParen) {
        error_add_note(
            &error,
            "This right parenthesis does not match an opening parenthesis");
        error_add_help(&error,
                       "Add the missing opening parenthesis or remove the "
                       "extra right parenthesis");
    } else {
        va_list args;
        va_start(args, format);
        error_add_helpv(&error, format, args);
        va_end(args);
    }

    error_render(&error);
    return false;
}

bool error_0205_expected_declaration_or_expression(NerdSource source,
                                                   ErrorSpan  span,
                                                   TokenKind  actual_kind,
                                                   cstr       help_format,
                                                   ...)
{
    string    actual = token_kind_to_string(actual_kind);
    ErrorInfo error =
        error_init(205, source, span, "Expected declaration or expression");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "Found %.*s here", STRINGV(actual));

    va_list args;
    va_start(args, help_format);
    error_add_helpv(&error, help_format, args);
    va_end(args);

    error_render(&error);
    return false;
}

bool error_0206_invalid_binding_target(NerdSource source, ErrorSpan span)
{
    ErrorInfo error =
        error_init(206, source, span, "Invalid binding target before `:=`");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "`:=` starts a new binding here");
    error_add_note(
        &error,
        "Bindings started with `:=` must begin with a symbol or supported "
        "destructuring pattern");
    error_add_help(
        &error,
        "Use `=` to assign to an existing value such as a dereference or "
        "field access");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
