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
        source, span, "Unknown symbol `" STRINGP "`", STRINGV(symbol));
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
    ErrorInfo error = error_init(source,
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
    ErrorInfo error = error_init(source,
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
        source, span, "Unknown type `" STRINGP "`", STRINGV(type_name));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This type name is not defined");
    error_add_help(&error,
                   "Use a defined type name, or one of the built-in primitive "
                   "types.");
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
        error_init(source,
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

bool error_0304_type_mismatch_with_note(NerdSource source,
                                        ErrorSpan  span,
                                        string     expected_type,
                                        string     actual_type,
                                        cstr       note_format,
                                        ...)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "Type mismatch: expected `" STRINGP "`, found `" STRINGP "`",
                   STRINGV(expected_type),
                   STRINGV(actual_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This expression has type `" STRINGP "`",
                        STRINGV(actual_type));
    va_list args;
    va_start(args, note_format);
    error_add_notev(&error, note_format, args);
    va_end(args);
    error_add_help(&error,
                   "Change the expression or annotation so both sides use the "
                   "same type.");
    error_render(&error);
    return false;
}

bool error_0304_address_of_constant_binding(NerdSource source,
                                            ErrorSpan  span,
                                            string     actual_type,
                                            string     symbol)
{
    ErrorInfo error = error_init(
        source,
        span,
        "Type mismatch: expected `addressable value`, found `" STRINGP "`",
        STRINGV(actual_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This expression has type `" STRINGP "`",
                        STRINGV(actual_type));
    error_add_note(&error,
                   "The binding `" STRINGP "` is a constant declared with "
                   "`::`; constant bindings do not provide addressable "
                   "storage.",
                   STRINGV(symbol));
    error_add_help(&error,
                   "Use `:=` or `name: Type = ...` to create a variable before "
                   "taking its address.");
    error_render(&error);
    return false;
}

bool error_0304_missing_plex_fields(NerdSource source,
                                    ErrorSpan  span,
                                    string     missing_fields,
                                    u32        missing_field_count)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Plex literal is missing required field%s",
                                 missing_field_count == 1 ? "" : "s");
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This literal does not initialise every required "
                        "field");
    error_add_note(&error,
                   "Missing field%s: " STRINGP,
                   missing_field_count == 1 ? "" : "s",
                   STRINGV(missing_fields));
    error_add_help(&error,
                   "Add all fields required by the plex type, or write `...` "
                   "in the literal to initialise omitted fields with their "
                   "default values.");
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
        source, span, "Cannot assign to `" STRINGP "`", STRINGV(symbol));
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
    ErrorInfo error = error_init(source,
                                 span,
                                 "Invalid variable type `" STRINGP "`",
                                 STRINGV(type_name));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This variable type is not supported yet");
    error_add_help(&error,
                   "Variables may use concrete integer, `bool`, `string`, "
                   "`f32`, or `f64` types.");
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
    ErrorInfo error = error_init(source,
                                 span,
                                 "Cannot cast `" STRINGP "` to `" STRINGP "`",
                                 STRINGV(source_type),
                                 STRINGV(target_type));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This cast is not supported");
    error_add_help(&error,
                   "Use explicit casts only between compatible primitive "
                   "types.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report use of a type name in a value position.

bool error_0308_type_used_as_value(NerdSource source,
                                   ErrorSpan  span,
                                   string     type_name)
{
    ErrorInfo error = error_init(source,
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
    ErrorInfo error = error_init(source,
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
// Report use of runtime interpolation outside a runtime context.

bool error_0310_invalid_interpolation_context(NerdSource source, ErrorSpan span)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "Runtime interpolated strings cannot be top-level values");
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This interpolation needs runtime string building");
    error_add_help(&error,
                   "Use only compile-time values in top-level interpolated "
                   "strings, or move the interpolation into a function.");
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
        error_init(source,
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
// Report a function call with the wrong number of arguments.

bool error_0313_argument_count_mismatch(NerdSource source,
                                        ErrorSpan  span,
                                        u32        expected_count,
                                        u32        actual_count)
{
    ErrorInfo error =
        error_init(source,
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
        error_init(source,
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
    ErrorInfo error = error_init(source, span, "Missing entry point `main`");
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
        error_init(source,
                   span,
                   "Invalid type for entry point `main`: found `" STRINGP "`",
                   STRINGV(actual_type));
    error_add_reference(
        &error,
        ERROR_REF_PRIMARY,
        span,
        "`main` must be a zero-parameter function returning `i32` or no value");
    error_add_help(&error,
                   "Change `main` so it takes no parameters and returns `i32` "
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
    ErrorInfo error = error_init(source,
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
    ErrorInfo error = error_init(source,
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
    ErrorInfo error = error_init(source,
                                 span,
                                 "`on` condition must have type `bool`, found "
                                 "`" STRINGP "`",
                                 STRINGV(actual_type));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This condition is not boolean");
    error_add_help(&error,
                   "Use a `bool` expression here, or use block-form `on` for "
                   "value matching.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a failed top-level platform assertion.

bool error_0336_platform_assertion_failed(NerdSource source,
                                          ErrorSpan  span,
                                          string     key,
                                          bool       is_negated)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Platform assertion failed for `" STRINGP "`",
                                 STRINGV(key));
    error_add_reference(
        &error,
        ERROR_REF_PRIMARY,
        span,
        is_negated ? "This file requires the platform key to be disabled"
                   : "This file requires the platform key to be enabled");
    if (is_negated) {
        error_add_help(&error,
                       "Build without this platform key, or remove the negated "
                       "assertion.");
    } else {
        error_add_help(&error,
                       "Build for a matching platform, build mode, or define "
                       "the key with `-D" STRINGP "`.",
                       STRINGV(key));
    }
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
        source, false_span, "`on` branches must return the same type");
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
        source,
        span,
        "Block-form `on` does not support values of type `" STRINGP "`",
        STRINGV(actual_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This matched value type is unsupported");
    error_add_help(&error,
                   "Block-form `on` supports `bool` and `string` scrutinees, "
                   "plus concrete integer scrutinees.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a top-level wildcard in a value-form `on` branch.

bool error_0322_invalid_on_wildcard_pattern(NerdSource source, ErrorSpan span)
{
    ErrorInfo error = error_init(
        source, span, "Block-form `on` wildcard pattern must use `else`");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This wildcard matches every value");
    error_add_help(&error,
                   "Use an `else` branch instead of `_` as a top-level "
                   "value pattern.");
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
        error_init(source,
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
        error_init(source, span, "Block-form `on` range pattern is empty");
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This range cannot match any values");
    error_add_help(&error,
                   inclusive
                       ? "Use a lower bound that is less than or equal to the "
                         "upper bound for `..=` ranges."
                       : "Use a lower bound that is strictly less than the "
                         "upper bound for `..` ranges.");
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
    ErrorInfo error = error_init(source,
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
    ErrorInfo error = error_init(source,
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
        source,
        span,
        "Value-producing block-form `on` expressions must be exhaustive");
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This `on` does not produce a value on every path");
    error_add_help(&error,
                   "Add an `else` branch, or use this `on` as a statement "
                   "when missing cases should be a no-op.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report `break` or `again` used outside a valid control-flow target.

bool error_0328_loop_control_outside_loop(NerdSource source,
                                          ErrorSpan  span,
                                          string     keyword)
{
    bool      is_break = string_eq(keyword, s("break"));
    ErrorInfo error = error_init(source,
                                 span,
                                 is_break ? "`break` can only be used inside a "
                                            "loop or expression block"
                                          : "`again` can only be used inside "
                                            "a loop");
    if (is_break) {
        error_add_reference(
            &error,
            ERROR_REF_PRIMARY,
            span,
            "This `break` is not inside a `for` loop or expression block");
        error_add_help(&error,
                       "Move `break` into a `for` loop or expression block.");
    } else {
        error_add_reference(&error,
                            ERROR_REF_PRIMARY,
                            span,
                            "This `again` is not inside a `for` loop");
        error_add_help(&error, "Move `again` into a `for` loop body.");
    }
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a value-producing expression block that can fall through.

bool error_0329_missing_expression_block_break(NerdSource source,
                                               ErrorSpan  span,
                                               string     type_name)
{
    ErrorInfo error = error_init(
        source,
        span,
        "Missing `break` for expression block returning `" STRINGP "`",
        STRINGV(type_name));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This expression block can reach the end without "
                        "breaking with a `" STRINGP "` value",
                        STRINGV(type_name));
    error_add_note(&error,
                   "Value-producing expression blocks must exit every "
                   "reachable path with `break <expr>`.");
    error_add_help(&error,
                   "Add `break <expr>` before the block ends or use a void "
                   "context if the block should not produce a value.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a labelled break or again that does not name an enclosing target.

bool error_0330_unknown_control_label(NerdSource source,
                                      ErrorSpan  span,
                                      string     label)
{
    ErrorInfo error = error_init(
        source, span, "Unknown control label `$" STRINGP "`", STRINGV(label));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "No enclosing expression block or loop has this label");
    error_add_help(&error,
                   "Use the label from an enclosing `$label { ... }` block "
                   "or `for ... $label { ... }` loop.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a labelled again that targets an expression block instead of a
// loop.

bool error_0331_continue_to_non_loop_label(NerdSource source,
                                           ErrorSpan  span,
                                           string     label)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "`again` label `$" STRINGP "` does not name a loop",
                   STRINGV(label));
    error_add_reference(
        &error,
        ERROR_REF_PRIMARY,
        span,
        "This label names an expression block, not a `for` loop");
    error_add_help(&error,
                   "Use `break $" STRINGP "` for an expression block, or "
                   "`again` to a `for ... $label { ... }` loop.",
                   STRINGV(label));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a finite value-producing loop expression without an exhaustion value.

bool error_0332_missing_loop_else(NerdSource source,
                                  ErrorSpan  span,
                                  string     type_name)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "Missing `else` for loop expression returning `" STRINGP "`",
                   STRINGV(type_name));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This loop can finish normally without breaking with a "
                        "`" STRINGP "` value",
                        STRINGV(type_name));
    error_add_note(&error,
                   "Finite value-producing loop expressions must make normal "
                   "loop exhaustion explicit with `else { break <expr> }`.");
    error_add_help(
        &error,
        "Add an `else` block that breaks with the loop result, or "
        "make the loop infinite if normal exhaustion is impossible.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an `else` block on a loop that has no value-producing breaks.

bool error_0333_invalid_loop_else(NerdSource source, ErrorSpan span)
{
    ErrorInfo error = error_init(
        source, span, "Loop `else` requires a value-producing loop expression");
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This `else` block has no matching value-producing "
                        "`break` in the loop");
    error_add_help(&error,
                   "Use `else` only on loops that return a value with "
                   "`break <expr>`, or remove the `else` block.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report reading mutable storage before every path has assigned it.

bool error_0334_read_before_assignment(NerdSource source,
                                       ErrorSpan  span,
                                       string     symbol,
                                       ErrorSpan  decl_span)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "Cannot read `" STRINGP "` before it has been assigned",
                   STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "`" STRINGP "` is read here",
                        STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        decl_span,
                        "`" STRINGP "` is declared with `undefined` here",
                        STRINGV(symbol));
    error_add_note(
        &error,
        "Variables declared with `undefined` must be assigned before they are "
        "read.");
    error_add_help(&error,
                   "Assign to `" STRINGP
                   "` on every path before using its value.",
                   STRINGV(symbol));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an unused function-local binding.

bool error_0335_unused_local(NerdSource source,
                             ErrorSpan  span,
                             string     symbol,
                             string     binding_kind)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Unused " STRINGP " `" STRINGP "`",
                                 STRINGV(binding_kind),
                                 STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This " STRINGP " is never read",
                        STRINGV(binding_kind));
    error_add_note(&error,
                   "Assigning to a variable does not count as using it.");
    error_add_help(
        &error,
        "Remove `" STRINGP
        "` or prefix the name with `_` if it is deliberately unused.",
        STRINGV(symbol));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a required parameter appearing after a parameter with a default value.

bool error_0336_required_param_after_default(NerdSource source,
                                             ErrorSpan  span,
                                             string     symbol)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Required parameter `" STRINGP
                                 "` cannot follow a defaulted parameter",
                                 STRINGV(symbol));
    error_add_reference(
        &error, ERROR_REF_PRIMARY, span, "This parameter has no default value");
    error_add_note(&error,
                   "Parameters with defaults must be the trailing parameters "
                   "in the signature.");
    error_add_help(&error,
                   "Move `" STRINGP
                   "` before the defaulted parameters or give it a default "
                   "value.",
                   STRINGV(symbol));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a default parameter on an FFI declaration.

bool error_0337_default_param_on_ffi(NerdSource source,
                                     ErrorSpan  span,
                                     string     symbol)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "FFI parameter `" STRINGP "` cannot have a default value",
                   STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This default value belongs to an FFI declaration");
    error_add_note(&error,
                   "FFI declarations describe the external function's exact C "
                   "signature.");
    error_add_help(&error,
                   "Remove the default value or wrap the FFI function in a "
                   "normal Nerd function with defaults.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a default parameter expression that references a later parameter.

bool error_0338_default_param_later_reference(NerdSource source,
                                              ErrorSpan  span,
                                              string     symbol)
{
    ErrorInfo error = error_init(
        source,
        span,
        "Default parameter cannot reference later parameter `" STRINGP "`",
        STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "`" STRINGP
                        "` is not available while this default is evaluated",
                        STRINGV(symbol));
    error_add_note(&error,
                   "Default arguments are evaluated at the call site from left "
                   "to right.");
    error_add_help(&error,
                   "Only reference parameters that appear earlier in the "
                   "signature.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report generic syntax that is parsed before semantic support is complete.

bool error_0339_generics_not_implemented(NerdSource source,
                                         ErrorSpan  span,
                                         string     construct)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "Generics are not implemented for `" STRINGP "` yet",
                   STRINGV(construct));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This generic syntax is recognised but not lowered");
    error_add_note(&error,
                   "The parser and formatter understand the square-bracket "
                   "generic syntax, but semantic instantiation is still in "
                   "progress.");
    error_add_help(&error, "Use a concrete non-generic declaration for now.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a named call argument that does not match the parameter at this
// position.

bool error_0340_named_argument_position(NerdSource source,
                                        ErrorSpan  span,
                                        string     expected,
                                        string     found)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Named argument `" STRINGP
                                 "` does not match parameter `" STRINGP "`",
                                 STRINGV(found),
                                 STRINGV(expected));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This argument is named `" STRINGP "`",
                        STRINGV(found));
    error_add_note(&error,
                   "Named arguments are currently checked in parameter order.");
    error_add_help(
        &error,
        "Move `" STRINGP
        " = ...` to the matching parameter position or provide `" STRINGP
        " = ...` here.",
        STRINGV(found),
        STRINGV(expected));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a public record field whose type names a private declaration.

bool error_0341_private_type_in_public_field(NerdSource source,
                                             ErrorSpan  span,
                                             string     public_type,
                                             string     private_type)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Public type `" STRINGP
                                 "` exposes private type `" STRINGP "`",
                                 STRINGV(public_type),
                                 STRINGV(private_type));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "`" STRINGP "` is private to this module",
                        STRINGV(private_type));
    error_add_note(&error,
                   "Public plex and union fields are part of the module's "
                   "public API.");
    error_add_help(&error,
                   "Make `" STRINGP
                   "` public or hide it behind a private implementation "
                   "detail that is not stored in the public field list.",
                   STRINGV(private_type));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an enum variant whose discriminant duplicates an earlier variant.

bool error_0342_duplicate_enum_discriminant(NerdSource source,
                                            ErrorSpan  span,
                                            i64        value,
                                            ErrorSpan  previous_span)
{
    ErrorInfo error = error_init(
        source, span, "Duplicate enum discriminant value `%lld`", value);
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This variant also uses value `%lld`",
                        value);
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        previous_span,
                        "Previous variant with value `%lld` is here",
                        value);
    error_add_help(&error,
                   "Give one of the variants a distinct explicit value, or "
                   "remove the explicit value so the implicit sequence does "
                   "not collide.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a repeated variant name in one enum.

bool error_0343_duplicate_enum_variant(NerdSource source,
                                       ErrorSpan  span,
                                       string     symbol,
                                       ErrorSpan  previous_span)
{
    ErrorInfo error = error_init(
        source, span, "Duplicate enum variant `" STRINGP "`", STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This variant reuses `" STRINGP "`",
                        STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        previous_span,
                        "Previous variant `" STRINGP "` is here",
                        STRINGV(symbol));
    error_add_help(
        &error,
        "Rename one of the variants so every variant in the enum is unique.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a type-qualified impl function call whose candidate is not a valid
// associated function.

bool error_0344_invalid_associated_function_return(NerdSource source,
                                                   ErrorSpan  span,
                                                   string     symbol)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Impl function `" STRINGP
                                 "` cannot be called as an associated function",
                                 STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "`" STRINGP
                        "` is called through a type, so it must return `Self` "
                        "or `^Self`",
                        STRINGV(symbol));
    error_add_help(&error,
                   "Change the impl function return type to `Self` or "
                   "`^Self`, or call it through a receiver value.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report an expression statement that discards a non-void value.

bool error_0345_discarded_value(NerdSource source,
                                ErrorSpan  span,
                                string     type_name)
{
    ErrorInfo error =
        error_init(source,
                   span,
                   "Expression result of type `" STRINGP "` is not used",
                   STRINGV(type_name));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This expression produces a value, but the value is "
                        "discarded");
    error_add_note(&error,
                   "Only `void` expressions can be used as standalone "
                   "statements.");
    error_add_help(&error,
                   "Bind the result to `_` when the value is intentionally "
                   "ignored.");
    error_render(&error);
    return false;
}

bool error_0346_unknown_ffi_symbol(NerdSource source,
                                   ErrorSpan  span,
                                   string     symbol,
                                   string     library)
{
    ErrorInfo error = error_init(source,
                                 span,
                                 "Foreign symbol `" STRINGP
                                 "` was not found in `" STRINGP "`",
                                 STRINGV(symbol),
                                 STRINGV(library));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "This FFI declaration links to `" STRINGP "`",
                        STRINGV(symbol));
    error_add_note(&error,
                   "Windows API names that are macros in C headers often need "
                   "an explicit `A` or `W` foreign symbol name in Nerd.");
    error_add_help(&error,
                   "Use `local_name :: foreign_name (...)` with the real "
                   "exported symbol, or move the declaration to the library "
                   "that exports it.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report use of a function-local binding marked deliberately unused.

bool error_0347_used_underscore_local(NerdSource source,
                                      ErrorSpan  use_span,
                                      ErrorSpan  decl_span,
                                      string     symbol,
                                      string     binding_kind)
{
    ErrorInfo error =
        error_init(source,
                   use_span,
                   "Used " STRINGP " `" STRINGP "` marked as unused",
                   STRINGV(binding_kind),
                   STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        use_span,
                        "This read uses `" STRINGP "`",
                        STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_SECONDARY,
                        decl_span,
                        "`" STRINGP "` is marked unused by its leading `_`",
                        STRINGV(symbol));
    error_add_note(
        &error,
        "Leading `_` names are reserved for bindings that are deliberately "
        "unused.");
    error_add_help(&error,
                   "Rename `" STRINGP
                   "` without the leading `_` now that it is used.",
                   STRINGV(symbol));
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
// Report a receiver method call that could resolve through multiple trait
// implementations.

bool error_0348_ambiguous_method_call(NerdSource source,
                                      ErrorSpan  span,
                                      string     symbol)
{
    ErrorInfo error = error_init(
        source, span, "Ambiguous method call `" STRINGP "`", STRINGV(symbol));
    error_add_reference(&error,
                        ERROR_REF_PRIMARY,
                        span,
                        "Multiple trait implementations provide `" STRINGP
                        "` for this receiver type",
                        STRINGV(symbol));
    error_add_help(&error,
                   "Call an inherent method with a unique name, or use an "
                   "explicit trait call once that syntax is available.");
    error_render(&error);
    return false;
}

//------------------------------------------------------------------------------
