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
    ErrorInfo error = error_init(
        300, source, span, "Unknown symbol `" STRINGP "`", STRINGV(symbol));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This symbol is not defined");
    error_add_help(&error,
                   "Add a binding for `" STRINGP "` or fix the spelling.",
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
    ErrorInfo error = error_init(301,
                                 source,
                                 span,
                                 "Duplicate binding for symbol `" STRINGP "`",
                                 STRINGV(symbol));
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
        &error,
        "Rename one of the bindings or remove the duplicate definition.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a dependency cycle between top-level bindings.

bool error_0302_dependency_cycle(NerdSource source,
                                 ErrorSpan  span,
                                 string     symbol,
                                 ErrorSpan  dependency_span,
                                 string     dependency_symbol)
{
    ErrorInfo error = error_init(302,
                                 source,
                                 span,
                                 "Dependency cycle involving `" STRINGP "`",
                                 STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This binding participates in a dependency cycle");
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        dependency_span,
                        "Cycle closes through `" STRINGP "` here",
                        STRINGV(dependency_symbol));
    error_add_help(&error,
                   "Break the cycle by rewriting one of the bindings so it no "
                   "longer depends on the other.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
