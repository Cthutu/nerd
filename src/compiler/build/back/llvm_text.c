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

internal bool back_end_llvm_line_metadata_definition_id(string line, u32* out)
{
    if (line.count < 4 || line.data[0] != '!' || line.data[1] < '0' ||
        line.data[1] > '9') {
        return false;
    }

    usize index = 1;
    u32   value = 0;
    while (index < line.count && line.data[index] >= '0' &&
           line.data[index] <= '9') {
        value = value * 10 + (u32)(line.data[index] - '0');
        index++;
    }
    if (index + 2 >= line.count || line.data[index] != ' ' ||
        line.data[index + 1] != '=' || line.data[index + 2] != ' ') {
        return false;
    }
    *out = value;
    return true;
}

internal u32 back_end_llvm_text_max_metadata_id(string text)
{
    u32   max_id     = 0;
    bool  found      = false;
    usize line_start = 0;
    for (usize i = 0; i <= text.count; ++i) {
        if (i < text.count && text.data[i] != '\n') {
            continue;
        }
        string line = {
            .data  = text.data + line_start,
            .count = i - line_start,
        };
        u32 id = 0;
        if (back_end_llvm_line_metadata_definition_id(line, &id)) {
            if (!found || id > max_id) {
                max_id = id;
            }
            found = true;
        }
        line_start = i + 1;
    }
    return found ? max_id : 0;
}

internal void back_end_append_remapped_llvm_metadata_line(StringBuilder* sb,
                                                         string line,
                                                         u32 base)
{
    for (usize i = 0; i < line.count; ++i) {
        if (line.data[i] == '!' && i + 1 < line.count &&
            line.data[i + 1] >= '0' && line.data[i + 1] <= '9') {
            usize id_start = i + 1;
            usize id_end   = id_start;
            u32   id       = 0;
            while (id_end < line.count && line.data[id_end] >= '0' &&
                   line.data[id_end] <= '9') {
                id = id * 10 + (u32)(line.data[id_end] - '0');
                id_end++;
            }
            sb_format(sb, "!%u", id + base);
            i = id_end - 1;
            continue;
        }
        sb_append_char(sb, line.data[i]);
    }
}

internal bool back_end_llvm_line_named_metadata_items(string line,
                                                      cstr   prefix,
                                                      string* out)
{
    usize prefix_count = strlen(prefix);
    if (line.count < prefix_count + 1 ||
        memcmp(line.data, prefix, prefix_count) != 0 ||
        line.data[line.count - 1] != '}') {
        return false;
    }
    *out = (string){
        .data  = line.data + prefix_count,
        .count = line.count - prefix_count - 1,
    };
    return true;
}

internal void back_end_append_named_metadata_items(StringBuilder* sb,
                                                  string items)
{
    if (items.count == 0) {
        return;
    }
    if (sb_to_string(sb).count > 0) {
        sb_append_cstr(sb, ", ");
    }
    sb_append_string(sb, items);
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
    Array(string) * declared_symbols,
    StringBuilder* dbg_cu_items,
    StringBuilder* module_flag_items,
    bool*          has_module_flags,
    u32            metadata_id_base)
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
        Arena line_arena = {0};
        arena_init(&line_arena);
        StringBuilder remapped_line = {0};
        sb_init(&remapped_line, &line_arena);
        back_end_append_remapped_llvm_metadata_line(&remapped_line,
                                                    line,
                                                    metadata_id_base);
        string rendered_line = sb_to_string(&remapped_line);

        string named_items = {0};
        if (back_end_llvm_line_named_metadata_items(
                rendered_line, "!llvm.dbg.cu = !{", &named_items)) {
            back_end_append_named_metadata_items(dbg_cu_items, named_items);
            arena_done(&line_arena);
            line_start = i + 1;
            continue;
        }
        if (back_end_llvm_line_named_metadata_items(
                rendered_line, "!llvm.module.flags = !{", &named_items)) {
            if (!*has_module_flags) {
                back_end_append_named_metadata_items(module_flag_items,
                                                     named_items);
                *has_module_flags = true;
            }
            arena_done(&line_arena);
            line_start = i + 1;
            continue;
        }

        sb_append_string(sb, rendered_line);
        arena_done(&line_arena);
        if (i < text.count) {
            sb_append_char(sb, '\n');
        }
        line_start = i + 1;
    }
}

string back_end_llvm_text_build_combined(Arena* arena,
                                         Array(string) module_llvms,
                                         string runtime_epilogue,
                                         string init_ll)
{
    Array(string) defined_symbols = NULL;
    array_requires_capacity(defined_symbols,
                            array_count(module_llvms) * 32 + 16);
    for (u32 i = 0; i < array_count(module_llvms); ++i) {
        back_end_collect_llvm_defined_symbols(&defined_symbols,
                                              module_llvms[i]);
    }
    back_end_collect_llvm_defined_symbols(&defined_symbols, runtime_epilogue);
    back_end_collect_llvm_defined_symbols(&defined_symbols, init_ll);

    StringBuilder combined_llvm_builder = {0};
    Arena         dbg_cu_arena          = {0};
    Arena         module_flag_arena     = {0};
    StringBuilder dbg_cu_items          = {0};
    StringBuilder module_flag_items     = {0};
    Array(string) declared_symbols      = NULL;
    array_requires_capacity(declared_symbols,
                            array_count(defined_symbols) + 16);
    sb_init(&combined_llvm_builder, arena);
    arena_init(&dbg_cu_arena);
    arena_init(&module_flag_arena);
    sb_init(&dbg_cu_items, &dbg_cu_arena);
    sb_init(&module_flag_items, &module_flag_arena);
    bool has_module_flags = false;
    u32  metadata_id_base = 0;
    for (u32 i = 0; i < array_count(module_llvms); ++i) {
        back_end_append_llvm_without_satisfied_declarations(
            &combined_llvm_builder,
            module_llvms[i],
            defined_symbols,
            &declared_symbols,
            &dbg_cu_items,
            &module_flag_items,
            &has_module_flags,
            metadata_id_base);
        metadata_id_base += back_end_llvm_text_max_metadata_id(module_llvms[i]) + 1;
        sb_append_cstr(&combined_llvm_builder, "\n");
    }
    back_end_append_llvm_without_satisfied_declarations(&combined_llvm_builder,
                                                        runtime_epilogue,
                                                        defined_symbols,
                                                        &declared_symbols,
                                                        &dbg_cu_items,
                                                        &module_flag_items,
                                                        &has_module_flags,
                                                        metadata_id_base);
    metadata_id_base += back_end_llvm_text_max_metadata_id(runtime_epilogue) + 1;
    sb_append_cstr(&combined_llvm_builder, "\n");
    back_end_append_llvm_without_satisfied_declarations(
        &combined_llvm_builder,
        init_ll,
        defined_symbols,
        &declared_symbols,
        &dbg_cu_items,
        &module_flag_items,
        &has_module_flags,
        metadata_id_base);

    string dbg_cu_text      = sb_to_string(&dbg_cu_items);
    string module_flag_text = sb_to_string(&module_flag_items);
    if (dbg_cu_text.count > 0 || module_flag_text.count > 0) {
        sb_append_cstr(&combined_llvm_builder, "\n");
        if (dbg_cu_text.count > 0) {
            sb_append_cstr(&combined_llvm_builder, "!llvm.dbg.cu = !{");
            sb_append_string(&combined_llvm_builder, dbg_cu_text);
            sb_append_cstr(&combined_llvm_builder, "}\n");
        }
        if (module_flag_text.count > 0) {
            sb_append_cstr(&combined_llvm_builder,
                           "!llvm.module.flags = !{");
            sb_append_string(&combined_llvm_builder, module_flag_text);
            sb_append_cstr(&combined_llvm_builder, "}\n");
        }
    }

    string combined_llvm = sb_to_string(&combined_llvm_builder);
    array_free(defined_symbols);
    array_free(declared_symbols);
    arena_done(&dbg_cu_arena);
    arena_done(&module_flag_arena);
    return combined_llvm;
}

internal u32 back_end_llvm_text_count_line(string text, cstr expected)
{
    u32   count          = 0;
    usize expected_count = strlen(expected);
    usize line_start     = 0;
    for (usize i = 0; i <= text.count; ++i) {
        if (i < text.count && text.data[i] != '\n') {
            continue;
        }
        string line = {
            .data  = text.data + line_start,
            .count = i - line_start,
        };
        if (line.count == expected_count &&
            memcmp(line.data, expected, expected_count) == 0) {
            count++;
        }
        line_start = i + 1;
    }
    return count;
}

internal bool
back_end_llvm_text_expect_line_count(string text, cstr line, u32 expected)
{
    u32 actual = back_end_llvm_text_count_line(text, line);
    if (actual != expected) {
        eprn("LLVM text self-test failed for line:");
        eprn("%s", line);
        eprn("Expected %u occurrence(s), found %u", expected, actual);
        return false;
    }
    return true;
}

bool back_end_llvm_text_self_test(void)
{
    Arena arena = {0};
    arena_init(&arena);

    Array(string) module_llvms = NULL;
    array_push(module_llvms,
               s("declare i32 @puts(ptr)\n"
                 "@.str.prelude = private unnamed_addr constant [1 x i8] "
                 "zeroinitializer\n"));
    array_push(module_llvms,
               s("declare i32 @puts(ptr)\n"
                 "declare void @fn.defined()\n"
                 "declare void @\"$main\"()\n"
                 "define void @fn.defined() {\n"
                 "  ret void\n"
                 "}\n"
                 "@\"$main\" = alias void (), ptr @fn.0\n"
                 "@.str.m0.0 = private unnamed_addr constant [1 x i8] "
                 "zeroinitializer\n"
                 "define void @fn.0() {\n"
                 "  ret void\n"
                 "}\n"));
    array_push(module_llvms,
               s("declare i32 @puts(ptr)\n"
                 "declare ptr @missing(ptr)\n"
                 "declare ptr @missing(ptr)\n"
                 "declare void @init()\n"
                 "define void @\"quoted.impl\"() {\n"
                 "  ret void\n"
                 "}\n"));

    string combined =
        back_end_llvm_text_build_combined(&arena,
                                          module_llvms,
                                          s("declare void @init()\n"
                                            "declare void @\"$main\"()\n"
                                            "define i32 @main() {\n"
                                            "  ret i32 0\n"
                                            "}\n"),
                                          s("define void @init() {\n"
                                            "  ret void\n"
                                            "}\n"));

    bool ok = true;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "declare i32 @puts(ptr)", 1) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "declare ptr @missing(ptr)", 1) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "declare void @fn.defined()", 0) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "declare void @\"$main\"()", 0) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "declare void @init()", 0) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "@\"$main\" = alias void (), ptr @fn.0", 1) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined,
                  "@.str.m0.0 = private unnamed_addr constant [1 x i8] "
                  "zeroinitializer",
                  1) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "define void @\"quoted.impl\"() {", 1) &&
              ok;
    ok      = back_end_llvm_text_expect_line_count(
                  combined, "define void @init() {", 1) &&
              ok;

    array_free(module_llvms);
    arena_done(&arena);
    if (ok) {
        prn("llvm-text ok");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
