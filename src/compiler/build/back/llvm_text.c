//------------------------------------------------------------------------------
// LLVM text utilities for the executable backend
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/back/llvm_text.h>

//------------------------------------------------------------------------------

internal bool back_end_string_starts_with_cstr(string text, cstr prefix)
{
    usize prefix_count = strlen(prefix);
    return text.count >= prefix_count &&
           memcmp(text.data, prefix, prefix_count) == 0;
}

internal bool back_end_string_contains_cstr(string text, cstr needle)
{
    usize needle_count = strlen(needle);
    if (needle_count == 0 || text.count < needle_count) {
        return false;
    }
    for (usize i = 0; i + needle_count <= text.count; ++i) {
        if (memcmp(text.data + i, needle, needle_count) == 0) {
            return true;
        }
    }
    return false;
}

// The executable backend combines independently-rendered textual LLVM modules
// into one clang input. Before concatenating, remove declarations already
// satisfied by a definition or alias in another module. This is deliberately a
// small top-level LLVM scanner, not a general LLVM parser.
internal bool back_end_llvm_line_symbol(string line, string* out)
{
    usize at = line.count;
    for (usize i = 0; i < line.count; ++i) {
        if (line.data[i] == '@') {
            at = i;
            break;
        }
    }
    if (at == line.count) {
        return false;
    }

    usize end = at + 1;
    if (end < line.count && line.data[end] == '"') {
        usize quoted_start = end + 1;
        usize quoted_end   = line.count;
        for (end = quoted_start; end < line.count; ++end) {
            if (line.data[end] == '"') {
                quoted_end = end;
                break;
            }
        }
        if (quoted_end == line.count || quoted_end == quoted_start) {
            return false;
        }
        *out = (string){
            .data  = line.data + quoted_start,
            .count = quoted_end - quoted_start,
        };
        return true;
    } else {
        while (end < line.count && line.data[end] != '(' &&
               line.data[end] != ' ' && line.data[end] != '\t' &&
               line.data[end] != ',') {
            end++;
        }
    }
    if (end <= at + 1) {
        return false;
    }
    usize symbol_start = at;
    if (at + 1 < end) {
        symbol_start = at + 1;
    }
    *out = (string){
        .data  = line.data + symbol_start,
        .count = end - symbol_start,
    };
    return true;
}

internal bool back_end_llvm_line_defines_symbol(string line, string* out)
{
    if (back_end_string_starts_with_cstr(line, "define ")) {
        return back_end_llvm_line_symbol(line, out);
    }
    // Covers top-level globals and aliases such as:
    //   @"$main" = alias void (), ptr @fn.0
    //   @.str.m0.0 = private unnamed_addr constant ...
    if (line.count > 0 && line.data[0] == '@' &&
        back_end_string_contains_cstr(line, " = ")) {
        return back_end_llvm_line_symbol(line, out);
    }
    return false;
}

internal bool back_end_llvm_line_declares_symbol(string line, string* out)
{
    if (!back_end_string_starts_with_cstr(line, "declare ")) {
        return false;
    }
    return back_end_llvm_line_symbol(line, out);
}

internal bool back_end_symbol_array_contains(Array(string) symbols,
                                             string symbol)
{
    for (u32 i = 0; i < array_count(symbols); ++i) {
        if (string_eq(symbols[i], symbol)) {
            return true;
        }
    }
    return false;
}

internal void back_end_collect_llvm_defined_symbols(Array(string) * symbols,
                                                    string text)
{
    usize line_start = 0;
    for (usize i = 0; i <= text.count; ++i) {
        if (i < text.count && text.data[i] != '\n') {
            continue;
        }
        string line = {
            .data  = text.data + line_start,
            .count = i - line_start,
        };
        string symbol = {0};
        if (back_end_llvm_line_defines_symbol(line, &symbol) &&
            !back_end_symbol_array_contains(*symbols, symbol)) {
            array_push(*symbols, symbol);
        }
        line_start = i + 1;
    }
}

internal void back_end_append_llvm_without_satisfied_declarations(
    StringBuilder* sb,
    string         text,
    Array(string) defined_symbols,
    Array(string) * declared_symbols)
{
    // Keep the first declaration for unresolved external symbols such as libc
    // calls, but drop duplicate declarations and declarations for symbols this
    // combined module defines. Clang rejects `declare @x` plus `define @x` in
    // one textual module even though separate input files allowed that shape.
    usize line_start = 0;
    for (usize i = 0; i <= text.count; ++i) {
        if (i < text.count && text.data[i] != '\n') {
            continue;
        }
        string line = {
            .data  = text.data + line_start,
            .count = i - line_start,
        };
        string declared_symbol = {0};
        if (back_end_llvm_line_declares_symbol(line, &declared_symbol)) {
            if (back_end_symbol_array_contains(defined_symbols,
                                               declared_symbol) ||
                back_end_symbol_array_contains(*declared_symbols,
                                               declared_symbol)) {
                line_start = i + 1;
                continue;
            }
            array_push(*declared_symbols, declared_symbol);
        }
        sb_append_string(sb, line);
        if (i < text.count) {
            sb_append_char(sb, '\n');
        }
        line_start = i + 1;
    }
}

string back_end_llvm_text_build_combined(Arena* arena,
                                         string runtime_prelude,
                                         Array(string) module_llvms,
                                         string runtime_epilogue,
                                         string init_ll)
{
    Array(string) defined_symbols = NULL;
    back_end_collect_llvm_defined_symbols(&defined_symbols, runtime_prelude);
    for (u32 i = 0; i < array_count(module_llvms); ++i) {
        back_end_collect_llvm_defined_symbols(&defined_symbols,
                                              module_llvms[i]);
    }
    back_end_collect_llvm_defined_symbols(&defined_symbols, runtime_epilogue);
    back_end_collect_llvm_defined_symbols(&defined_symbols, init_ll);

    StringBuilder combined_llvm_builder = {0};
    Array(string) declared_symbols      = NULL;
    sb_init(&combined_llvm_builder, arena);
    back_end_append_llvm_without_satisfied_declarations(&combined_llvm_builder,
                                                        runtime_prelude,
                                                        defined_symbols,
                                                        &declared_symbols);
    sb_append_cstr(&combined_llvm_builder, "\n\n");
    for (u32 i = 0; i < array_count(module_llvms); ++i) {
        back_end_append_llvm_without_satisfied_declarations(
            &combined_llvm_builder,
            module_llvms[i],
            defined_symbols,
            &declared_symbols);
        sb_append_cstr(&combined_llvm_builder, "\n");
    }
    back_end_append_llvm_without_satisfied_declarations(&combined_llvm_builder,
                                                        runtime_epilogue,
                                                        defined_symbols,
                                                        &declared_symbols);
    sb_append_cstr(&combined_llvm_builder, "\n");
    back_end_append_llvm_without_satisfied_declarations(
        &combined_llvm_builder, init_ll, defined_symbols, &declared_symbols);

    string combined_llvm = sb_to_string(&combined_llvm_builder);
    array_free(defined_symbols);
    array_free(declared_symbols);
    return combined_llvm;
}

//------------------------------------------------------------------------------
