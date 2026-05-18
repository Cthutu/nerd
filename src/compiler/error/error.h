//------------------------------------------------------------------------------
// Error module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/compiler.h>
#include <compiler/lexer/lexer.h>
#include <core/core.h>

//------------------------------------------------------------------------------
// All error functions return false for convenience so you can always do
//
// ```
// return error(...);
// ```
//
// Some notes about the error system:
//
//      - Each diagnostic has a unique error function API. This function is
//        used to set up information for the error rendering system, such as
//        the error message format and any additional information that is
//        needed, notes, help messages and source locations.
//      - The error rendering system is responsible for rendering the error
//        message, notes and help messages, and source locations in a nice
//        format.
//      - The error system can be switched to test-mode that renders the
//        error information as JSON.  This is used in the error tests.
//------------------------------------------------------------------------------

typedef enum {
    ERROR_RENDER_NORMAL,
    ERROR_RENDER_TEST,
    ERROR_RENDER_DIAGNOSTICS,
} ErrorRenderMode;

void   error_system_init(ErrorRenderMode mode);
void   error_system_done(void);
void   error_system_set_mode(ErrorRenderMode mode);
void   error_system_set_emit_output(bool emit_output);
void   error_system_clear_last_rendered(void);
string error_system_last_rendered(void);
void   error_system_store_last_rendered(string rendered);

// These functions are used during rendering
ErrorRenderMode error_system_mode(void);
bool            error_system_should_emit_output(void);
void            error_system_reset(void);

//------------------------------------------------------------------------------
// General internal compiler error.
//
// These should never happen, so when they do, it's a bug!  This function will
// abort.

bool error_ice(const char* format, ...);

//------------------------------------------------------------------------------
// Runtime/compiler-environment errors.
//
// These are failures outside the user's source program and outside compiler
// invariants: missing OS permissions, failed writes, toolchain failures, etc.
// They are reported separately from language diagnostics and ICEs.

bool error_runtime(const char* format, ...);

//------------------------------------------------------------------------------
// Lexer errors

typedef struct {
    usize start;
    usize end;
} ErrorSpan;

bool error_0100_unexpected_character(NerdSource source, usize offset, char c);
bool error_0101_integer_literal_too_large(NerdSource source, ErrorSpan span);
bool error_0102_file_too_large(NerdSource source);
bool error_0103_invalid_number_literal(NerdSource source,
                                       ErrorSpan  span,
                                       char       invalid_char);
bool error_0104_symbol_too_long(NerdSource source, ErrorSpan span);
bool error_0105_too_many_symbols(NerdSource source);
bool error_0106_unterminated_string_literal(NerdSource source, ErrorSpan span);
bool error_0107_unterminated_packed_integer_literal(NerdSource source,
                                                    ErrorSpan  span);
bool error_0108_packed_integer_literal_too_large(NerdSource source,
                                                 ErrorSpan  span);

// Used when the AST node array can't be formed because we have more than 4
// billion nodes
bool error_0200_code_too_complex(NerdSource source, ErrorSpan span);

// Used when an operator is found without a previous value.
bool error_0201_missing_value_ex(NerdSource source,
                                 ErrorSpan  span,
                                 TokenKind  expected_kind,
                                 cstr       note,
                                 cstr       help);
bool error_0201_missing_value(NerdSource source,
                              ErrorSpan  span,
                              TokenKind  expected_kind);

// Used when two values are found that are not separated with an operator.
bool error_0202_missing_operator_ex(NerdSource source,
                                    ErrorSpan  span,
                                    TokenKind  expected_kind,
                                    cstr       note,
                                    cstr       help);
bool error_0202_missing_operator(NerdSource source,
                                 ErrorSpan  span,
                                 TokenKind  expected_kind);

// Used when a specific token is required to continue parsing.
bool error_0203_expected_token_ex(NerdSource source,
                                  ErrorSpan  span,
                                  TokenKind  expected_kind,
                                  TokenKind  actual_kind,
                                  cstr       note,
                                  cstr       help);
bool error_0203_expected_token(NerdSource source,
                               ErrorSpan  span,
                               TokenKind  expected_kind,
                               TokenKind  actual_kind);
bool error_0203_expected_closing_token(NerdSource source,
                                       ErrorSpan  span,
                                       TokenKind  expected_kind,
                                       TokenKind  actual_kind,
                                       ErrorSpan  opening_span);
bool error_0203_expected_closing_token_before(NerdSource source,
                                              ErrorSpan  span,
                                              TokenKind  expected_kind,
                                              ErrorSpan  opening_span);

// Used when parsing succeeds for a complete expression but trailing tokens
// remain.
bool error_0204_unexpected_token_ex(NerdSource source,
                                    ErrorSpan  span,
                                    TokenKind  actual_kind,
                                    cstr       note,
                                    cstr       help);
bool error_0204_unexpected_token_here(NerdSource source,
                                      ErrorSpan  span,
                                      TokenKind  actual_kind,
                                      cstr       note,
                                      cstr       help);
bool error_0204_unexpected_token(NerdSource source,
                                 ErrorSpan  span,
                                 TokenKind  actual_kind,
                                 cstr       help_format,
                                 ...);

// Expected declaration or expression
bool error_0205_expected_declaration_or_expression_ex(NerdSource source,
                                                      ErrorSpan  span,
                                                      TokenKind  actual_kind,
                                                      cstr       note,
                                                      cstr       help);
bool error_0205_expected_declaration_or_expression(NerdSource source,
                                                   ErrorSpan  span,
                                                   TokenKind  actual_kind,
                                                   cstr       help_format,
                                                   ...);

// Used when `:=` appears after an expression that cannot start a binding.
bool error_0206_invalid_binding_target(NerdSource source, ErrorSpan span);

// Used when an operator appears in a place where the parser cannot accept one.
bool error_0207_unexpected_operator(NerdSource source,
                                    ErrorSpan  span,
                                    cstr       actual_operator,
                                    cstr       context,
                                    cstr       note,
                                    cstr       help);

// Used when a type grammar position contains another token.
bool error_0208_expected_type(NerdSource source,
                              ErrorSpan  span,
                              TokenKind  actual_kind,
                              cstr       note,
                              cstr       help);

//------------------------------------------------------------------------------
// Semantic analysis errors

bool error_0300_unknown_symbol(NerdSource source,
                               ErrorSpan  span,
                               string     symbol);
bool error_0301_duplicate_binding(NerdSource source,
                                  ErrorSpan  span,
                                  string     symbol,
                                  ErrorSpan  previous_span);
bool error_0302_dependency_cycle(NerdSource source,
                                 ErrorSpan  span,
                                 string     symbol,
                                 ErrorSpan  dependency_span,
                                 string     dependency_symbol);
bool error_0303_unknown_type(NerdSource source,
                             ErrorSpan  span,
                             string     type_name);
bool error_0304_type_mismatch(NerdSource source,
                              ErrorSpan  span,
                              string     expected_type,
                              string     actual_type);
bool error_0304_type_mismatch_with_note(NerdSource source,
                                        ErrorSpan  span,
                                        string     expected_type,
                                        string     actual_type,
                                        cstr       note_format,
                                        ...);
bool error_0304_address_of_constant_binding(NerdSource source,
                                            ErrorSpan  span,
                                            string     actual_type,
                                            string     symbol);
bool error_0304_missing_plex_fields(NerdSource source,
                                    ErrorSpan  span,
                                    string     missing_fields,
                                    u32        missing_field_count);
bool error_0305_invalid_assignment_target(NerdSource source,
                                          ErrorSpan  span,
                                          string     symbol);
bool error_0306_invalid_variable_type(NerdSource source,
                                      ErrorSpan  span,
                                      string     type_name);
bool error_0307_invalid_cast(NerdSource source,
                             ErrorSpan  span,
                             string     source_type,
                             string     target_type);
bool error_0308_type_used_as_value(NerdSource source,
                                   ErrorSpan  span,
                                   string     type_name);
bool error_0309_type_alias_cycle(NerdSource source,
                                 ErrorSpan  span,
                                 string     symbol,
                                 ErrorSpan  dependency_span,
                                 string     dependency_symbol);
bool error_0310_invalid_interpolation_context(NerdSource source,
                                              ErrorSpan  span);
bool error_0311_invalid_interpolation_type(NerdSource source,
                                           ErrorSpan  span,
                                           string     type_name);
bool error_0313_argument_count_mismatch(NerdSource source,
                                        ErrorSpan  span,
                                        u32        expected_count,
                                        u32        actual_count);
bool error_0314_missing_return(NerdSource source,
                               ErrorSpan  span,
                               string     return_type);
bool error_0315_missing_entry_point(NerdSource source, ErrorSpan span);
bool error_0316_invalid_entry_point(NerdSource source,
                                    ErrorSpan  span,
                                    string     actual_type);
bool error_0317_non_closure_capture(NerdSource source,
                                    ErrorSpan  span,
                                    string     symbol);
bool error_0318_mixed_function_return_style(NerdSource source, ErrorSpan span);
bool error_0319_invalid_on_condition(NerdSource source,
                                     ErrorSpan  span,
                                     string     actual_type);
bool error_0336_platform_assertion_failed(NerdSource source,
                                          ErrorSpan  span,
                                          string     key,
                                          bool       is_negated);
bool error_0320_on_branch_type_mismatch(NerdSource source,
                                        ErrorSpan  true_span,
                                        string     true_type,
                                        ErrorSpan  false_span,
                                        string     false_type);
bool error_0321_invalid_on_match_type(NerdSource source,
                                      ErrorSpan  span,
                                      string     actual_type);
bool error_0322_invalid_on_wildcard_pattern(NerdSource source, ErrorSpan span);
bool error_0323_negative_unsigned_inference(NerdSource source,
                                            ErrorSpan  span,
                                            string     target_type);
bool error_0324_invalid_on_range_bounds(NerdSource source,
                                        ErrorSpan  span,
                                        bool       inclusive);
bool error_0325_invalid_unary_operand(NerdSource source,
                                      ErrorSpan  span,
                                      string     operator_name,
                                      string     expected_type,
                                      string     actual_type);
bool error_0326_invalid_binary_operands(NerdSource source,
                                        ErrorSpan  span,
                                        string     operator_name,
                                        string     expected_rule,
                                        string     left_type,
                                        string     right_type);
bool error_0327_non_exhaustive_on(NerdSource source, ErrorSpan span);
bool error_0328_loop_control_outside_loop(NerdSource source,
                                          ErrorSpan  span,
                                          string     keyword);
bool error_0329_missing_expression_block_break(NerdSource source,
                                               ErrorSpan  span,
                                               string     type_name);
bool error_0330_unknown_control_label(NerdSource source,
                                      ErrorSpan  span,
                                      string     label);
bool error_0331_continue_to_non_loop_label(NerdSource source,
                                           ErrorSpan  span,
                                           string     label);
bool error_0332_missing_loop_else(NerdSource source,
                                  ErrorSpan  span,
                                  string     type_name);
bool error_0333_invalid_loop_else(NerdSource source, ErrorSpan span);
bool error_0334_read_before_assignment(NerdSource source,
                                       ErrorSpan  span,
                                       string     symbol,
                                       ErrorSpan  decl_span);
bool error_0335_unused_local(NerdSource source,
                             ErrorSpan  span,
                             string     symbol,
                             string     binding_kind);
bool error_0336_required_param_after_default(NerdSource source,
                                             ErrorSpan  span,
                                             string     symbol);
bool error_0337_default_param_on_ffi(NerdSource source,
                                     ErrorSpan  span,
                                     string     symbol);
bool error_0338_default_param_later_reference(NerdSource source,
                                              ErrorSpan  span,
                                              string     symbol);
bool error_0339_generics_not_implemented(NerdSource source,
                                         ErrorSpan  span,
                                         string     construct);
bool error_0340_named_argument_position(NerdSource source,
                                        ErrorSpan  span,
                                        string     expected,
                                        string     found);
bool error_0341_private_type_in_public_field(NerdSource source,
                                             ErrorSpan  span,
                                             string     public_type,
                                             string     private_type);
bool error_0342_duplicate_enum_discriminant(NerdSource source,
                                            ErrorSpan  span,
                                            i64        value,
                                            ErrorSpan  previous_span);
bool error_0343_duplicate_enum_variant(NerdSource source,
                                       ErrorSpan  span,
                                       string     symbol,
                                       ErrorSpan  previous_span);
bool error_0344_invalid_associated_function_return(NerdSource source,
                                                   ErrorSpan  span,
                                                   string     symbol);
bool error_0345_discarded_value(NerdSource source,
                                ErrorSpan  span,
                                string     type_name);
bool error_0346_unknown_ffi_symbol(NerdSource source,
                                   ErrorSpan  span,
                                   string     symbol,
                                   string     library);
bool error_0347_used_underscore_local(NerdSource source,
                                      ErrorSpan  use_span,
                                      ErrorSpan  decl_span,
                                      string     symbol,
                                      string     binding_kind);
bool error_0348_ambiguous_method_call(NerdSource source,
                                      ErrorSpan  span,
                                      string     symbol);
bool error_0349_unqualified_trait_member_call(NerdSource source,
                                              ErrorSpan  span,
                                              string     member,
                                              string     trait);
bool error_0350_unexpected_return_value(NerdSource source, ErrorSpan span);
bool error_0351_loop_else_on_infinite_loop(NerdSource source, ErrorSpan span);

//------------------------------------------------------------------------------
// Low-level error system

typedef enum {
    ERROR_REF_PRIMARY,
    ERROR_REF_SECONDARY,
} ErrorRefKind;

typedef enum {
    ERROR_KIND_WARNING,
    ERROR_KIND_ERROR,
    ERROR_KIND_INTERNAL,
    ERROR_KIND_RUNTIME,
} ErrorKind;

typedef struct {
    ErrorRefKind ref_kind;
    ErrorSpan    span; // Byte span in the source code
    string       message;
} ErrorRef;

typedef struct {
    ErrorKind  kind;
    string     error_message;
    NerdSource source;
    ErrorSpan  span;
    Array(ErrorRef) references;
    Array(string) notes;
    Array(string) help_messages;
} ErrorInfo;

ErrorInfo error_init(NerdSource source, ErrorSpan span, cstr error_format, ...);
ErrorInfo
warning_init(NerdSource source, ErrorSpan span, cstr error_format, ...);
void error_add_reference(
    ErrorInfo* error_info, ErrorRefKind kind, ErrorSpan span, cstr format, ...);
void error_add_notev(ErrorInfo* error_info, cstr format, va_list args);
void error_add_note(ErrorInfo* error_info, cstr format, ...);
void error_add_helpv(ErrorInfo* error_info, cstr format, va_list args);
void error_add_help(ErrorInfo* error_info, cstr format, ...);

// Renders the error information to the console.  If the error system is in test
// mode, it renders the error information as JSON instead.
//
// In normal mode, the output should be similar to Rust compiler messages. For a
// contrived example,
//
//--------------------------------------------------------------------
// error: Unexpected character '@'
//  --> src/main.nerd:42:5
//    |
// 40 |     a := 2;
// 41 |     b := 3;
// 42 |     @ := 10;
//    |     ^ Unexpected character '@'
// 43 |     prn($"a = {a}")
// 44 |     prn($"b = {b}")
//    |
// note: known characters are most ANSI characters. help: it is probably a typo
// so review what you've entered.
//--------------------------------------------------------------------
//
// Notice a few things about the output:
//  - There is a code snippet that shows the code where the error is.
//      - There is a blank non-numbered line before and after
//      - The line with the error has a marker (^) that points to the error. `^`
//        is a primary marker, and `~` is used as a secondary marker.
//      - The gutter width adapts to the width of the widest line number.
//      - We show up to 2 code lines before and after each referenced source
//        location. When referenced areas are far apart, a gutter-only divider
//        separates the displayed regions.
//  - There is a note that provides additional information about the error.
//  - There is a help message to tell the user what they should do to attempt to
//    fix the problem.
//  - The error uses colour coding:
//      - Red means error
//      - Yellow means warning
//      - Blue means internal compiler error
//      - Cyan means notes and help messages
//
// In test mode, JSON is produced instead and pretty-printed:
//
// {
//     "message": "Unexpected character '@'",
//     "source_file": "src/main.nerd",
//     "primary_location": {
//         "line": 42,
//         "column": 5
//     },
//     "references": [
//         {
//             "kind": "primary",
//             "line": 42,
//             "column": 5,
//             "length": 1,
//             "message": "Unexpected character '@'"
//         }
//     ],
//     "notes": [
//         "Known characters are most ANSI characters."
//     ],
//     "help": [
//         "It is probably a typo so review what you've entered."
//     ]
// }
//
// The JSON output is used in the error tests to verify that the correct error
// information is produced for various error cases.
//
// The `error_info` is freed after rendering
//------------------------------------------------------------------------------

void error_render(ErrorInfo* error_info);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
