//------------------------------------------------------------------------------
// Semantic analysis errors
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>

//------------------------------------------------------------------------------
// Report use of a symbol that has no matching top-level binding.

bool error_0300_unknown_symbol(NerdSource source, ErrorSpan span, string symbol)
{
    ErrorInfo error =
        error_init(300, source, span, "Unknown symbol `" STRINGP "`", STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This symbol is not defined");
    error_add_help(&error,
                   "Add a binding for `" STRINGP
                   "` before it is used or fix the spelling.",
                   STRINGV(symbol));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a repeated top-level binding for the same symbol.

bool error_0301_duplicate_binding(NerdSource source,
                                  ErrorSpan  span,
                                  string     symbol,
                                  ErrorSpan  previous_span)
{
    ErrorInfo error = error_init(
        301, source, span, "Duplicate binding for symbol `" STRINGP "`", STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This binding redefines `" STRINGP "`",
                        STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        previous_span,
                        "Previous binding of `" STRINGP "` is here",
                        STRINGV(symbol));
    error_add_help(
        &error, "Rename one of the bindings or remove the duplicate definition.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
