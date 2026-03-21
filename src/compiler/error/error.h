//------------------------------------------------------------------------------
// Error module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

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
//      - Every error has a 4-digit error code (a category)
//      - An error code also determines which area of the compiler the error
//        has occurred.  E.g. 0100-0199 are lexer errors, 0200-0299 are AST
//        errors.
//      - Each error category has a unique error function API.  This function
//        is used to set up information for the error rendering system, such
//        as the error code, the error message format, and any additional
//        information that is needed, notes, help messages and source locations.
//      - The error rendering system is responsible for rendering the error
//        message, notes and help messages, and source locations in a nice
//        format.
//      - The error system can be switched to test-mode that renders the
//        error information as JSON.  This is used in the error tests.
//------------------------------------------------------------------------------

void error_system_init(bool test_mode);
void error_system_done(void);

//------------------------------------------------------------------------------
// General internal compiler error.
//
// These should never happen, so when they do, it's a bug!  This function will
// abort.

bool error_ice(const char* format, ...);

//------------------------------------------------------------------------------
// Lexer errors

bool error_0100_unexpected_character(string source_file, usize offset, char c);
bool error_0101_integer_literal_too_large(string source_file, usize offset);
bool error_0102_file_too_many_tokens(string source_file);

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
} ErrorKind;

typedef struct {
    ErrorKind    kind;
    ErrorRefKind ref_kind;
    usize        offset; // Offset in the source code
    usize        length; // Length of the source code span
    string       message;
} ErrorRef;

typedef struct {
    u16    code; // 4-digit error code
    string error_message;
    string source_file;
    usize  primary_offset;
    Array(ErrorRef) references;
    Array(string) notes;
    Array(string) help_messages;
} ErrorInfo;

ErrorInfo error_init(
    u16 code, string source_file, usize primary_offset, cstr error_format, ...);
void error_add_reference(ErrorInfo*   error_info,
                         ErrorRefKind kind,
                         usize        offset,
                         usize        length,
                         cstr         format,
                         ...);
void error_add_note(ErrorInfo* error_info, cstr format, ...);
void error_add_help(ErrorInfo* error_info, cstr format, ...);

// Renders the error information to the console.  If the error system is in test
// mode, it renders the error information as JSON instead.
//
// In normal mode, the output should be similar to Rust compiler messages. For a
// contrived example,
//
//--------------------------------------------------------------------
// error[0100]: Unexpected character '@'
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
//      - We always show 2 code lines before and after the error for context.
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
//     "code": "0100",
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
//------------------------------------------------------------------------------

void error_render(const ErrorInfo* error_info);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
