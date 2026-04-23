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

bool error_0303_unknown_type(NerdSource source,
                             ErrorSpan  span,
                             string     type_name)
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
    ErrorInfo error = error_init(
        305, source, span, "Cannot assign to `" STRINGP "`", STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "`" STRINGP "` is not a mutable variable",
                        STRINGV(symbol));
    error_add_help(&error,
                   "Declare `" STRINGP
                   "` as a variable with `:` or assign to a "
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
                   "For this milestone, variables may use concrete integer, "
                   "`bool`, `string`, `f32`, or `f64` types.");
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
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This cast is not supported");
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
// Report a dependency cycle between type aliases.

bool error_0309_type_alias_cycle(NerdSource source,
                                 ErrorSpan  span,
                                 string     symbol,
                                 ErrorSpan  dependency_span,
                                 string     dependency_symbol)
{
    ErrorInfo error = error_init(309,
                                 source,
                                 span,
                                 "Type alias cycle involving `" STRINGP "`",
                                 STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This alias participates in a type cycle");
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        dependency_span,
                        "Cycle closes through `" STRINGP "` here",
                        STRINGV(dependency_symbol));
    error_add_help(&error,
                   "Break the cycle by rewriting one of the aliases so it "
                   "resolves to a concrete type.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report use of interpolation outside a supported function-local context.

bool error_0310_invalid_interpolation_context(NerdSource source, ErrorSpan span)
{
    ErrorInfo error =
        error_init(310,
                   source,
                   span,
                   "Interpolated strings are only supported inside functions");
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This interpolated string appears at top level");
    error_add_help(&error,
                   "Move the interpolation inside a function body for the "
                   "current milestone.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report interpolation of an unsupported semantic type.

bool error_0311_invalid_interpolation_type(NerdSource source,
                                           ErrorSpan  span,
                                           string     type_name)
{
    ErrorInfo error =
        error_init(311,
                   source,
                   span,
                   "Cannot interpolate values of type `" STRINGP "`",
                   STRINGV(type_name));
    error_add_reference(
        &error,
        ERROR_REF_PRIMARY,
        span,
        "This expression type cannot be converted to string yet");
    error_add_help(&error,
                   "Use a built-in primitive or `string`, or cast the value "
                   "to a supported type first.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report use of an interpolated temporary where the value could outlive
// statement scope.

bool error_0312_interpolated_string_escapes(NerdSource source, ErrorSpan span)
{
    ErrorInfo error = error_init(
        312, source, span, "Interpolated string cannot escape statement scope");
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This value would outlive the temporary string arena");
    error_add_help(&error,
                   "Use interpolated strings only in statement-local contexts "
                   "such as call arguments for now.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a function call with the wrong number of arguments.

bool error_0313_argument_count_mismatch(NerdSource source,
                                        ErrorSpan  span,
                                        u32        expected_count,
                                        u32        actual_count)
{
    ErrorInfo error =
        error_init(313,
                   source,
                   span,
                   "Argument count mismatch: expected %u, found %u",
                   expected_count,
                   actual_count);
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This call uses the wrong arity");
    error_add_help(&error,
                   "Pass exactly %u argument%s to match the function "
                   "signature.",
                   expected_count,
                   expected_count == 1 ? "" : "s");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a block-bodied function with an explicit return type that falls
// through without returning a value.

bool error_0314_missing_return(NerdSource source,
                               ErrorSpan  span,
                               string     return_type)
{
    ErrorInfo error =
        error_init(314,
                   source,
                   span,
                   "Missing return for function returning `" STRINGP "`",
                   STRINGV(return_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This function can reach the end without returning a "
                        "`" STRINGP "` value",
                        STRINGV(return_type));
    error_add_note(&error,
                   "Block-bodied functions with explicit return types must "
                   "return a value before they end.");
    error_add_help(&error,
                   "Add `return <expr>` to the function body or remove the "
                   "explicit return type if the function should not produce a "
                   "value.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a program with no valid `main` entry point.

bool error_0315_missing_entry_point(NerdSource source, ErrorSpan span)
{
    ErrorInfo error = error_init(315, source, span, "Missing entry point `main`");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "No `main` function is defined");
    error_add_note(&error, "Programs currently require a `main` entry point.");
    error_add_help(
        &error, "Add `main :: fn () => 0` or another zero-parameter function "
                "returning `i32`.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a `main` binding that does not have the required entry-point type.

bool error_0316_invalid_entry_point(NerdSource source,
                                    ErrorSpan  span,
                                    string     actual_type)
{
    ErrorInfo error = error_init(316,
                                 source,
                                 span,
                                 "Invalid type for entry point `main`: found `"
                                 STRINGP "`",
                                 STRINGV(actual_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "`main` must be a zero-parameter function returning an integer");
    error_add_help(
        &error,
        "Change `main` so it takes no parameters and returns an integer type.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
