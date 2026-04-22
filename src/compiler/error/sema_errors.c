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
// Report a type annotation that names an unsupported type.

bool error_0303_unknown_type(NerdSource source, ErrorSpan span, string type_name)
{
    ErrorInfo error = error_init(
        303, source, span, "Unknown type `" STRINGP "`", STRINGV(type_name));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This type name is not defined");
    error_add_help(&error,
                   "Use one of the built-in primitive types supported by the "
                   "current milestone.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a semantic type mismatch.

bool error_0304_type_mismatch(NerdSource source,
                              ErrorSpan  span,
                              string     expected_type,
                              string     actual_type)
{
    ErrorInfo error =
        error_init(304,
                   source,
                   span,
                   "Type mismatch: expected `" STRINGP "`, found `" STRINGP "`",
                   STRINGV(expected_type),
                   STRINGV(actual_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This expression has type `" STRINGP "`",
                        STRINGV(actual_type));
    error_add_help(&error,
                   "Change the expression or annotation so both sides use the "
                   "same type.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report assignment to a non-variable symbol.

bool error_0305_invalid_assignment_target(NerdSource source,
                                          ErrorSpan  span,
                                          string     symbol)
{
    ErrorInfo error = error_init(305,
                                 source,
                                 span,
                                 "Cannot assign to `" STRINGP "`",
                                 STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "`" STRINGP "` is not a mutable variable",
                        STRINGV(symbol));
    error_add_help(&error,
                   "Declare `" STRINGP "` as a variable with `:` or assign to a "
                   "different mutable symbol.",
                   STRINGV(symbol));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a variable declaration using an unsupported storage type.

bool error_0306_invalid_variable_type(NerdSource source,
                                      ErrorSpan  span,
                                      string     type_name)
{
    ErrorInfo error = error_init(306,
                                 source,
                                 span,
                                 "Invalid variable type `" STRINGP "`",
                                 STRINGV(type_name));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This variable type is not supported yet");
    error_add_help(&error,
                   "For this milestone, variables must use a concrete integer "
                   "type.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an explicit cast between incompatible types.

bool error_0307_invalid_cast(NerdSource source,
                             ErrorSpan  span,
                             string     source_type,
                             string     target_type)
{
    ErrorInfo error = error_init(307,
                                 source,
                                 span,
                                 "Cannot cast `" STRINGP "` to `" STRINGP "`",
                                 STRINGV(source_type),
                                 STRINGV(target_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This cast is not supported");
    error_add_help(&error,
                   "Use explicit casts only between compatible primitive "
                   "types in the current milestone.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report use of a type name in a value position.

bool error_0308_type_used_as_value(NerdSource source,
                                   ErrorSpan  span,
                                   string     type_name)
{
    ErrorInfo error = error_init(308,
                                 source,
                                 span,
                                 "Cannot use type `" STRINGP "` as a value",
                                 STRINGV(type_name));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This name refers to a type");
    error_add_help(&error,
                   "Use `" STRINGP "` in a type annotation or bind a runtime "
                   "value instead.",
                   STRINGV(type_name));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
