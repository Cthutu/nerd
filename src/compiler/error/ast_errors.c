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
    error_add_help(
        &error,
        "Insert a literal, parenthesized expression, or unary operator");
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

bool error_0204_unexpected_token(NerdSource source,
                                 ErrorSpan  span,
                                 TokenKind  actual_kind)
{
    string    actual = token_kind_to_string(actual_kind);
    ErrorInfo error =
        error_init(204, source, span, "Unexpected token after expression");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "Found %.*s here", STRINGV(actual));

    if (actual_kind == TK_RParen) {
        error_add_note(
            &error, "This right parenthesis does not match an opening parenthesis");
        error_add_help(
            &error,
            "Add the missing opening parenthesis or remove the extra right parenthesis");
    } else {
        error_add_help(&error,
                       "Remove the extra token or add an operator to continue the expression");
    }

    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
