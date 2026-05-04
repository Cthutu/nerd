//------------------------------------------------------------------------------
// Test command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

typedef struct {
    string name;
    usize  start_offset;
    usize  body_start_offset;
    usize  body_end_offset;
    usize  end_offset;
    bool   selected;
} SourceTestDecl;

internal bool source_test_is_ident(u8 ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') || ch == '_';
}

internal bool source_test_starts_with_at(string source, usize offset, cstr text)
{
    usize count = strlen(text);
    return offset + count <= source.count &&
           memcmp(source.data + offset, text, count) == 0;
}

internal bool source_test_contains(string value, string needle)
{
    if (needle.count == 0) {
        return true;
    }
    if (needle.count > value.count) {
        return false;
    }
    for (usize i = 0; i + needle.count <= value.count; i++) {
        if (memcmp(value.data + i, needle.data, needle.count) == 0) {
            return true;
        }
    }
    return false;
}

internal void source_test_skip_line_comment(string source, usize* offset)
{
    while (*offset < source.count && source.data[*offset] != '\n') {
        *offset += 1;
    }
}

internal bool source_test_skip_string(string source, usize* offset)
{
    ASSERT(*offset < source.count && source.data[*offset] == '"',
           "Expected string start");

    *offset += 1;
    while (*offset < source.count) {
        u8 ch = source.data[*offset];
        if (ch == '\\') {
            *offset += 2;
            continue;
        }
        *offset += 1;
        if (ch == '"') {
            return true;
        }
    }
    return false;
}

internal bool source_test_skip_c_string(string source, usize* offset)
{
    ASSERT(source_test_starts_with_at(source, *offset, "c\""),
           "Expected c-string start");
    *offset += 1;
    return source_test_skip_string(source, offset);
}

internal void source_test_skip_ws_and_comments(string source, usize* offset)
{
    while (*offset < source.count) {
        u8 ch = source.data[*offset];
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            *offset += 1;
            continue;
        }
        if (source_test_starts_with_at(source, *offset, "--")) {
            source_test_skip_line_comment(source, offset);
            continue;
        }
        break;
    }
}

internal bool source_test_parse_name(string source, usize* offset, string* out)
{
    source_test_skip_ws_and_comments(source, offset);
    if (*offset >= source.count || source.data[*offset] != '"') {
        return error_runtime("Expected a string literal after `test`");
    }

    usize start = *offset + 1;
    *offset += 1;
    while (*offset < source.count) {
        u8 ch = source.data[*offset];
        if (ch == '\\') {
            *offset += 2;
            continue;
        }
        if (ch == '"') {
            *out = string_from(source.data + start, *offset - start);
            *offset += 1;
            return true;
        }
        *offset += 1;
    }
    return error_runtime("Unterminated test name string");
}

internal bool source_test_find_body(string source,
                                    usize* offset,
                                    usize* body_start,
                                    usize* body_end,
                                    usize* end_offset)
{
    source_test_skip_ws_and_comments(source, offset);
    if (*offset >= source.count || source.data[*offset] != '{') {
        return error_runtime("Expected `{` after test name");
    }

    *body_start = *offset + 1;
    *offset += 1;
    u32 depth = 1;
    while (*offset < source.count) {
        if (source_test_starts_with_at(source, *offset, "--")) {
            source_test_skip_line_comment(source, offset);
            continue;
        }
        if (source_test_starts_with_at(source, *offset, "c\"")) {
            if (!source_test_skip_c_string(source, offset)) {
                return error_runtime("Unterminated string in test body");
            }
            continue;
        }

        u8 ch = source.data[*offset];
        if (ch == '"') {
            if (!source_test_skip_string(source, offset)) {
                return error_runtime("Unterminated string in test body");
            }
            continue;
        }
        if (ch == '{') {
            depth += 1;
        } else if (ch == '}') {
            depth -= 1;
            if (depth == 0) {
                *body_end   = *offset;
                *end_offset = *offset + 1;
                *offset += 1;
                return true;
            }
        }
        *offset += 1;
    }
    return error_runtime("Unterminated test block");
}

internal bool source_test_discover(Arena* arena,
                                   string source,
                                   string filter,
                                   Array(SourceTestDecl) * out_tests)
{
    usize offset = 0;
    u32   depth  = 0;

    while (offset < source.count) {
        if (source_test_starts_with_at(source, offset, "--")) {
            source_test_skip_line_comment(source, &offset);
            continue;
        }
        if (source_test_starts_with_at(source, offset, "c\"")) {
            if (!source_test_skip_c_string(source, &offset)) {
                return error_runtime("Unterminated string while finding tests");
            }
            continue;
        }

        u8 ch = source.data[offset];
        if (ch == '"') {
            if (!source_test_skip_string(source, &offset)) {
                return error_runtime("Unterminated string while finding tests");
            }
            continue;
        }
        if (ch == '{') {
            depth += 1;
            offset += 1;
            continue;
        }
        if (ch == '}') {
            if (depth > 0) {
                depth -= 1;
            }
            offset += 1;
            continue;
        }

        if (depth == 0 && source_test_starts_with_at(source, offset, "test")) {
            bool left_ok =
                offset == 0 || !source_test_is_ident(source.data[offset - 1]);
            bool right_ok = offset + 4 >= source.count ||
                            !source_test_is_ident(source.data[offset + 4]);
            if (left_ok && right_ok) {
                usize  test_start = offset;
                string name       = {0};
                offset += 4;
                if (!source_test_parse_name(source, &offset, &name)) {
                    return false;
                }

                usize body_start = 0;
                usize body_end   = 0;
                usize test_end   = 0;
                if (!source_test_find_body(
                        source, &offset, &body_start, &body_end, &test_end)) {
                    return false;
                }

                string stable_name =
                    string_format(arena, STRINGP, STRINGV(name));
                array_push(
                    *out_tests,
                    (SourceTestDecl){
                        .name              = stable_name,
                        .start_offset      = test_start,
                        .body_start_offset = body_start,
                        .body_end_offset   = body_end,
                        .end_offset        = test_end,
                        .selected = source_test_contains(stable_name, filter),
                    });
                continue;
            }
        }

        offset += 1;
    }

    return true;
}

internal string source_test_generated_source(Arena* arena,
                                             string source,
                                             Array(SourceTestDecl) tests)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    usize next_offset      = 0;
    u32   selected_counter = 0;
    for (usize i = 0; i < array_count(tests); i++) {
        SourceTestDecl test = tests[i];
        sb_append_string(&sb,
                         string_from(source.data + next_offset,
                                     test.start_offset - next_offset));

        if (test.selected) {
            sb_format(&sb, "__nerd_test_%u :: fn () {", selected_counter);
            sb_append_string(
                &sb,
                string_from(source.data + test.body_start_offset,
                            test.body_end_offset - test.body_start_offset));
            sb_append_cstr(&sb, "\n}\n");
            selected_counter += 1;
        }

        next_offset = test.end_offset;
    }

    sb_append_string(
        &sb,
        string_from(source.data + next_offset, source.count - next_offset));

    sb_append_cstr(&sb, "\nmain :: fn () -> i32 {\n");
    selected_counter = 0;
    for (usize i = 0; i < array_count(tests); i++) {
        if (!tests[i].selected) {
            continue;
        }
        sb_format(&sb, "    __nerd_test_%u()\n", selected_counter);
        selected_counter += 1;
    }
    sb_append_cstr(&sb, "    return 0\n}\n");

    return sb_to_string(&sb);
}

int compiler_cmd_test(const NerdTestConfig* config)
{
    Arena arena = {0};
    arena_init(&arena);

    cstr input_path = compiler_cmd_copy_path(&arena, config->input_path);
    if (input_path == NULL || input_path[0] == '\0') {
        arena_done(&arena);
        return error_runtime("Expected a root source file for `nerd test`");
    }

    FileMap map    = {0};
    string  source = filemap_load(input_path, &map);
    if (source.data == NULL) {
        arena_done(&arena);
        return error_runtime("Failed to load source file: %s", input_path);
    }

    Array(SourceTestDecl) tests = NULL;
    bool discovered =
        source_test_discover(&arena, source, config->filter, &tests);
    if (!discovered) {
        array_free(tests);
        filemap_unload(&map);
        arena_done(&arena);
        return 1;
    }

    u32 selected_count = 0;
    for (usize i = 0; i < array_count(tests); i++) {
        if (tests[i].selected) {
            selected_count += 1;
        }
    }

    if (config->list) {
        for (usize i = 0; i < array_count(tests); i++) {
            if (tests[i].selected) {
                prn(STRINGP, STRINGV(tests[i].name));
            }
        }
        array_free(tests);
        filemap_unload(&map);
        arena_done(&arena);
        return 0;
    }

    if (selected_count == 0) {
        prn("0 tests passed");
        array_free(tests);
        filemap_unload(&map);
        arena_done(&arena);
        return 0;
    }

    string generated = source_test_generated_source(&arena, source, tests);
    NerdRunConfig run_config = {
        .source =
            (NerdSource){
                .source      = generated,
                .source_path = config->input_path,
            },
        .verbose  = config->verbose,
        .keywords = config->keywords,
    };

    int result = compiler_cmd_run(&run_config);
    if (result == 0) {
        prn("%u tests passed", selected_count);
    } else {
        eprn("source test run failed");
    }

    array_free(tests);
    filemap_unload(&map);
    arena_done(&arena);
    return result;
}

//------------------------------------------------------------------------------
