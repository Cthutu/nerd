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
    ErrorInfo error =
        error_init(315, source, span, "Missing entry point `main`");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "No `main` function is defined");
    error_add_note(&error, "Programs currently require a `main` entry point.");
    error_add_help(
        &error,
        "Add `main :: fn () => 0` or another zero-parameter function bound "
        "to main returning `i32` or no type at all.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a `main` binding that does not have the required entry-point type.

bool error_0316_invalid_entry_point(NerdSource source,
                                    ErrorSpan  span,
                                    string     actual_type)
{
    ErrorInfo error =
        error_init(316,
                   source,
                   span,
                   "Invalid type for entry point `main`: found `" STRINGP "`",
                   STRINGV(actual_type));
    error_add_reference(
        &error,
        ERROR_REF_PRIMARY,
        span,
        "`main` must be a zero-parameter function returning an integer");
    error_add_help(
        &error,
        "Change `main` so it takes no parameters and returns an integer type "
        "or no type at all.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a nested function trying to capture an enclosing local binding.

bool error_0317_non_closure_capture(NerdSource source,
                                    ErrorSpan  span,
                                    string     symbol)
{
    ErrorInfo error = error_init(317,
                                 source,
                                 span,
                                 "Cannot capture outer binding `" STRINGP "`",
                                 STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "Nested functions are non-closures, so `" STRINGP
                        "` is not available here",
                        STRINGV(symbol));
    error_add_help(&error,
                   "Pass `" STRINGP
                   "` as a parameter instead of referencing it from the "
                   "enclosing function scope.",
                   STRINGV(symbol));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report use of an explicit return type together with a fat-arrow body.

bool error_0318_mixed_function_return_style(NerdSource source, ErrorSpan span)
{
    ErrorInfo error = error_init(318,
                                 source,
                                 span,
                                 "Cannot combine an explicit return type with "
                                 "a fat-arrow body");
    error_add_reference(
        &error,
        ERROR_REF_PRIMARY,
        span,
        "This function uses `->` for an explicit return type and `=>` for an "
        "expression body");
    error_add_note(&error,
                   "Fat-arrow functions infer their return type from the body "
                   "expression.");
    error_add_help(&error,
                   "Use `fn (...) => expr` for inferred returns, or rewrite "
                   "the function as `fn (...) -> T { return expr }`.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a non-boolean condition in an `on` expression.

bool error_0319_invalid_on_condition(NerdSource source,
                                     ErrorSpan  span,
                                     string     actual_type)
{
    ErrorInfo error = error_init(319,
                                 source,
                                 span,
                                 "`on` condition must have type `bool`, found "
                                 "`" STRINGP "`",
                                 STRINGV(actual_type));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This condition is not boolean");
    error_add_help(&error,
                   "Use a `bool` expression here. Comparisons and richer "
                   "patterns can be added in later milestones.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report mismatched branch result types in an `on` expression.

bool error_0320_on_branch_type_mismatch(NerdSource source,
                                        ErrorSpan  true_span,
                                        string     true_type,
                                        ErrorSpan  false_span,
                                        string     false_type)
{
    ErrorInfo error = error_init(
        320, source, false_span, "`on` branches must return the same type");
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        false_span,
                        "This branch has type `" STRINGP "`",
                        STRINGV(false_type));
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        true_span,
                        "The other branch has type `" STRINGP "`",
                        STRINGV(true_type));
    error_add_help(&error, "Make both branches produce exactly the same type.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report value-form `on` over an unsupported scrutinee type.

bool error_0321_invalid_on_match_type(NerdSource source,
                                      ErrorSpan  span,
                                      string     actual_type)
{
    ErrorInfo error = error_init(
        321,
        source,
        span,
        "Block-form `on` does not support values of type `" STRINGP "`",
        STRINGV(actual_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This matched value type is unsupported");
    error_add_help(&error,
                   "For this milestone, block-form `on` supports `bool` and "
                   "concrete integer scrutinees.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a value-form `on` branch pattern that is not compile-time constant.

bool error_0322_non_constant_on_pattern(NerdSource source, ErrorSpan span)
{
    ErrorInfo error =
        error_init(322,
                   source,
                   span,
                   "Block-form `on` patterns must be compile-time constants");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This pattern is not constant");
    error_add_help(&error,
                   "Use a literal or folded constant binding for this pattern "
                   "until richer pattern forms land.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report implicit use of a negative untyped integer where an unsigned type is
// required.

bool error_0323_negative_unsigned_inference(NerdSource source,
                                            ErrorSpan  span,
                                            string     target_type)
{
    ErrorInfo error =
        error_init(323,
                   source,
                   span,
                   "Cannot infer negative integer as `" STRINGP "`",
                   STRINGV(target_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This value is negative but `" STRINGP "` is unsigned",
                        STRINGV(target_type));
    error_add_help(&error,
                   "Use a non-negative value here, or change the destination "
                   "type to a signed integer.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an integer range pattern whose bounds cannot match any value.

bool error_0324_invalid_on_range_bounds(NerdSource source,
                                        ErrorSpan  span,
                                        bool       inclusive)
{
    ErrorInfo error =
        error_init(324, source, span, "Block-form `on` range pattern is empty");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This range cannot match any values");
    error_add_help(&error,
                   inclusive
                       ? "Use a lower bound that is less than or equal to the "
                         "upper bound for `..=` ranges."
                       : "Use a lower bound that is strictly less than the "
                         "upper bound for `..<` ranges.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an invalid unary operator operand type.

bool error_0325_invalid_unary_operand(NerdSource source,
                                      ErrorSpan  span,
                                      string     operator_name,
                                      string     expected_type,
                                      string     actual_type)
{
    ErrorInfo error = error_init(325,
                                 source,
                                 span,
                                 "Operator `" STRINGP "` requires `" STRINGP
                                 "`, found `" STRINGP "`",
                                 STRINGV(operator_name),
                                 STRINGV(expected_type),
                                 STRINGV(actual_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This operand has type `" STRINGP "`",
                        STRINGV(actual_type));
    error_add_help(&error,
                   "Apply `" STRINGP "` only to `" STRINGP "` values.",
                   STRINGV(operator_name),
                   STRINGV(expected_type));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an invalid binary operator operand combination.

bool error_0326_invalid_binary_operands(NerdSource source,
                                        ErrorSpan  span,
                                        string     operator_name,
                                        string     expected_rule,
                                        string     left_type,
                                        string     right_type)
{
    ErrorInfo error = error_init(326,
                                 source,
                                 span,
                                 "Operator `" STRINGP "` requires " STRINGP
                                 ", found `" STRINGP "` and `" STRINGP "`",
                                 STRINGV(operator_name),
                                 STRINGV(expected_rule),
                                 STRINGV(left_type),
                                 STRINGV(right_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "These operands have types `" STRINGP "` and `" STRINGP
                        "`",
                        STRINGV(left_type),
                        STRINGV(right_type));
    error_add_help(&error,
                   "Use `" STRINGP "` only with " STRINGP ".",
                   STRINGV(operator_name),
                   STRINGV(expected_rule));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a value-producing block-form `on` that is not exhaustive.

bool error_0327_non_exhaustive_on(NerdSource source, ErrorSpan span)
{
    ErrorInfo error = error_init(
        327,
        source,
        span,
        "Value-producing block-form `on` expressions must be exhaustive");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This `on` can miss a value");
    error_add_help(&error,
                   "Add an `else` branch, or use this `on` as a statement "
                   "when missing cases should be a no-op.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
