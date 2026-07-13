//------------------------------------------------------------------------------
// Test command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/cmd_internal.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

typedef enum {
    SOURCE_TEST_ITEM_NAMED,
    SOURCE_TEST_ITEM_DECLS,
} SourceTestItemKind;

typedef struct {
    SourceTestItemKind kind;
    string             name;
    usize              start_offset;
    usize              body_start_offset;
    usize              body_end_offset;
    usize              end_offset;
    bool               selected;
} SourceTestDecl;

typedef struct {
    usize start_offset;
    usize end_offset;
    bool  found;
} SourceTestMainDecl;

typedef struct {
    string source;
    Array(NerdSourceFragment) fragments;
} SourceTestGenerated;

internal bool source_test_is_ident(u8 ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') || ch == '_' || ch == '.';
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
                                    usize* end_offset,
                                    bool   reject_public_decls)
{
    source_test_skip_ws_and_comments(source, offset);
    if (*offset >= source.count || source.data[*offset] != '{') {
        return error_runtime("Expected `{` after test declaration");
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

        if (reject_public_decls && depth == 1 &&
            source_test_starts_with_at(source, *offset, "pub")) {
            bool left_ok =
                *offset == 0 || !source_test_is_ident(source.data[*offset - 1]);
            bool right_ok = *offset + 3 >= source.count ||
                            !source_test_is_ident(source.data[*offset + 3]);
            if (left_ok && right_ok) {
                return error_runtime(
                    "`pub` is not valid inside a test-only declaration block");
            }
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
                                   string name_prefix,
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
                source_test_skip_ws_and_comments(source, &offset);
                bool is_named =
                    offset < source.count && source.data[offset] == '"';
                if (is_named) {
                    if (!source_test_parse_name(source, &offset, &name)) {
                        return false;
                    }
                } else if (offset >= source.count ||
                           source.data[offset] != '{') {
                    return error_runtime(
                        "Expected a string literal or `{` after `test`");
                }

                usize body_start = 0;
                usize body_end   = 0;
                usize test_end   = 0;
                if (!source_test_find_body(source,
                                           &offset,
                                           &body_start,
                                           &body_end,
                                           &test_end,
                                           !is_named)) {
                    return false;
                }

                string stable_name = {0};
                if (is_named) {
                    stable_name =
                        name_prefix.count > 0
                            ? string_format(arena,
                                            STRINGP ": " STRINGP,
                                            STRINGV(name_prefix),
                                            STRINGV(name))
                            : string_format(arena, STRINGP, STRINGV(name));
                }
                array_push(*out_tests,
                           (SourceTestDecl){
                               .kind = is_named ? SOURCE_TEST_ITEM_NAMED
                                                : SOURCE_TEST_ITEM_DECLS,
                               .name = stable_name,
                               .start_offset      = test_start,
                               .body_start_offset = body_start,
                               .body_end_offset   = body_end,
                               .end_offset        = test_end,
                               .selected = is_named && source_test_contains(
                                                           stable_name, filter),
                           });
                continue;
            }
        }

        offset += 1;
    }

    return true;
}

internal bool source_test_is_top_level_main_binding(string source, usize offset)
{
    bool left_ok =
        offset == 0 || !source_test_is_ident(source.data[offset - 1]);
    bool right_ok = offset + 4 >= source.count ||
                    !source_test_is_ident(source.data[offset + 4]);
    if (!left_ok || !right_ok) {
        return false;
    }

    usize cursor = offset + 4;
    source_test_skip_ws_and_comments(source, &cursor);
    return cursor + 1 < source.count && source.data[cursor] == ':' &&
           source.data[cursor + 1] == ':';
}

internal SourceTestMainDecl source_test_find_main_binding(string source)
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
                break;
            }
            continue;
        }

        u8 ch = source.data[offset];
        if (ch == '"') {
            if (!source_test_skip_string(source, &offset)) {
                break;
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

        if (depth == 0 && source_test_starts_with_at(source, offset, "main") &&
            source_test_is_top_level_main_binding(source, offset)) {
            return (SourceTestMainDecl){
                .start_offset = offset,
                .end_offset   = offset + 4,
                .found        = true,
            };
        }

        offset += 1;
    }

    return (SourceTestMainDecl){0};
}

internal NerdSourceFragment source_test_fragment_for_range(
    NerdSource source, usize start, usize end, usize generated_start)
{
    string source_path  = source.source_path;
    usize  source_start = start;

    for (u32 i = 0; i < array_count(source.fragments); ++i) {
        NerdSourceFragment fragment = source.fragments[i];
        if (start < fragment.start || start >= fragment.end) {
            continue;
        }

        source_path  = fragment.source_path;
        source_start = start - fragment.start + fragment.source_start;
        break;
    }

    return (NerdSourceFragment){
        .start        = generated_start,
        .end          = generated_start + (end - start),
        .source_start = source_start,
        .source_path  = source_path,
    };
}

internal void source_test_append_mapped_range(StringBuilder* sb,
                                              NerdSource     source,
                                              usize          start,
                                              usize          end,
                                              Array(NerdSourceFragment) *
                                                  fragments)
{
    if (end <= start) {
        return;
    }

    usize generated_start = sb->size;
    sb_append_string(sb, string_from(source.source.data + start, end - start));
    array_push(
        *fragments,
        source_test_fragment_for_range(source, start, end, generated_start));
}

internal void source_test_append_source_range(StringBuilder*     sb,
                                              NerdSource         source,
                                              usize              start,
                                              usize              end,
                                              SourceTestMainDecl main_decl,
                                              Array(NerdSourceFragment) *
                                                  fragments)
{
    if (!main_decl.found || main_decl.start_offset >= end ||
        main_decl.end_offset <= start) {
        source_test_append_mapped_range(sb, source, start, end, fragments);
        return;
    }

    source_test_append_mapped_range(
        sb, source, start, main_decl.start_offset, fragments);
    sb_append_cstr(sb, "__nerd_program_main");
    source_test_append_mapped_range(
        sb, source, main_decl.end_offset, end, fragments);
}

internal SourceTestGenerated source_test_generated_source(Arena*     arena,
                                                          NerdSource source,
                                                          Array(SourceTestDecl)
                                                              tests)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    Array(NerdSourceFragment) fragments = NULL;
    SourceTestMainDecl main_decl = source_test_find_main_binding(source.source);
    usize              next_offset      = 0;
    u32                selected_counter = 0;
    for (usize i = 0; i < array_count(tests); i++) {
        SourceTestDecl test = tests[i];
        source_test_append_source_range(
            &sb, source, next_offset, test.start_offset, main_decl, &fragments);

        if (test.kind == SOURCE_TEST_ITEM_DECLS) {
            source_test_append_mapped_range(&sb,
                                            source,
                                            test.body_start_offset,
                                            test.body_end_offset,
                                            &fragments);
            sb_append_char(&sb, '\n');
        } else if (test.selected) {
            sb_format(
                &sb, "__nerd_test_%u :: fn () -> void {", selected_counter);
            source_test_append_mapped_range(&sb,
                                            source,
                                            test.body_start_offset,
                                            test.body_end_offset,
                                            &fragments);
            sb_append_cstr(&sb, "\n    return\n}\n");
            selected_counter += 1;
        }

        next_offset = test.end_offset;
    }

    source_test_append_source_range(
        &sb, source, next_offset, source.source.count, main_decl, &fragments);

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

    return (SourceTestGenerated){
        .source    = sb_to_string(&sb),
        .fragments = fragments,
    };
}

internal void source_test_print_result(bool passed, string name)
{
    if (passed) {
        prn(ANSI_GREEN "[PASS]" ANSI_RESET " " STRINGP, STRINGV(name));
    } else {
        eprn(ANSI_RED "[FAIL]" ANSI_RESET " " STRINGP, STRINGV(name));
    }
}

internal int source_test_run_one(Arena*     arena,
                                 NerdSource source,
                                 Array(SourceTestDecl) tests,
                                 usize                 test_index,
                                 string                generated_path,
                                 const NerdTestConfig* config)
{
    Array(SourceTestDecl) single_tests = NULL;
    for (usize i = 0; i < array_count(tests); ++i) {
        SourceTestDecl test = tests[i];
        test.selected = test.kind == SOURCE_TEST_ITEM_NAMED && i == test_index;
        array_push(single_tests, test);
    }

    SourceTestGenerated generated =
        source_test_generated_source(arena, source, single_tests);
    NerdRunConfig run_config = {
        .source =
            (NerdSource){
                .source      = generated.source,
                .source_path = generated_path,
                .fragments   = generated.fragments,
            },
        .verbose  = config->verbose,
        .keywords = config->keywords,
    };

    int result = compiler_cmd_run(&run_config);
    source_test_print_result(result == 0, tests[test_index].name);
    array_free(single_tests);
    return result;
}

internal int source_test_run_verbose(Arena*     arena,
                                     NerdSource source,
                                     Array(SourceTestDecl) tests,
                                     string                generated_path,
                                     const NerdTestConfig* config,
                                     u32*                  out_passed_count)
{
    for (usize i = 0; i < array_count(tests); ++i) {
        if (!tests[i].selected) {
            continue;
        }

        int result = source_test_run_one(
            arena, source, tests, i, generated_path, config);
        if (result != 0) {
            return result;
        }
        *out_passed_count += 1;
    }
    return 0;
}

internal bool source_test_path_matches_platform(cstr path)
{
#if OS_WINDOWS
    if (strstr(path, ".linux.") != NULL) {
        return false;
    }
#else
    if (strstr(path, ".windows.") != NULL) {
        return false;
    }
#endif

#if !OS_LINUX
    if (strstr(path, ".linux.") != NULL) {
        return false;
    }
#endif

    return true;
}

internal bool source_test_file_has_named_tests(Arena* arena, cstr path)
{
    FileMap map    = {0};
    string  source = filemap_load(path, &map);
    if (source.data == NULL) {
        return false;
    }

    Array(SourceTestDecl) tests = NULL;
    bool discovered =
        source_test_discover(arena, source, (string){0}, s(""), &tests);
    bool found = false;
    if (discovered) {
        for (usize i = 0; i < array_count(tests); ++i) {
            if (tests[i].kind == SOURCE_TEST_ITEM_NAMED) {
                found = true;
                break;
            }
        }
    }

    array_free(tests);
    filemap_unload(&map);
    return found;
}

internal string source_test_summary_label(Arena* arena, cstr path)
{
    string filename = path_filename(s(path));
    if (!string_eq(filename, s("mod.n"))) {
        return s(path);
    }

    cstr   module_dir  = path_dirname(arena, path);
    string module_name = path_filename(s(module_dir));
    return string_format(arena, "module:" STRINGP, STRINGV(module_name));
}

internal int source_test_run_directory(Arena*                arena,
                                       cstr                  path,
                                       const NerdTestConfig* config,
                                       u32*                  out_file_count)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, path)) {
        return error_runtime("Failed to read test directory: %s", path);
    }

    int  result       = 0;
    cstr child_path   = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, arena, &child_path, &is_directory)) {
        if (is_directory) {
            result = source_test_run_directory(
                arena, child_path, config, out_file_count);
            if (result != 0) {
                break;
            }
            continue;
        }

        if (!path_has_extension(s(child_path), ".n") ||
            !source_test_path_matches_platform(child_path) ||
            !source_test_file_has_named_tests(arena, child_path)) {
            continue;
        }

        NerdTestConfig child_config = *config;
        child_config.input_path     = s(child_path);
        child_config.summary_label =
            source_test_summary_label(arena, child_path);
        result = compiler_cmd_test(&child_config);
        if (result != 0) {
            break;
        }
        *out_file_count += 1;
    }

    dir_iter_done(&iter);
    return result;
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

    if (path_is_directory(input_path)) {
        u32 file_count = 0;
        int result =
            source_test_run_directory(&arena, input_path, config, &file_count);
        if (result == 0 && file_count == 0) {
            prn(ANSI_BOLD_YELLOW "0 tests passed" ANSI_RESET);
        }
        arena_done(&arena);
        return result;
    }

    FileMap map    = {0};
    string  source = filemap_load(input_path, &map);
    if (source.data == NULL) {
        arena_done(&arena);
        return error_runtime("Failed to load source file: %s", input_path);
    }

    ProgramInfo     program = {0};
    FrontEndOptions options = {
        .verbose             = config->verbose,
        .require_entry_point = false,
        .skip_hir_generation = true,
        .keywords            = config->keywords,
    };
    NerdSource root_source = {
        .source      = source,
        .source_path = s(input_path),
    };
    if (!front_end_program(root_source, &options, NULL, &program)) {
        Array(SourceTestDecl) tests = NULL;
        bool root_discovered        = source_test_discover(
            &arena, source, (string){0}, config->filter, &tests);
        if (!root_discovered) {
            array_free(tests);
            filemap_unload(&map);
            arena_done(&arena);
            return 1;
        }

        u32 root_selected_count = 0;
        for (usize i = 0; i < array_count(tests); i++) {
            if (tests[i].selected) {
                root_selected_count += 1;
            }
        }

        if (config->list) {
            for (usize i = 0; i < array_count(tests); i++) {
                if (tests[i].selected) {
                    prn(ANSI_CYAN STRINGP ANSI_RESET, STRINGV(tests[i].name));
                }
            }
            array_free(tests);
            filemap_unload(&map);
            arena_done(&arena);
            return 0;
        }

        if (root_selected_count == 0) {
            prn(ANSI_BOLD_YELLOW "0 tests passed" ANSI_RESET);
            array_free(tests);
            filemap_unload(&map);
            arena_done(&arena);
            return 0;
        }

        if (config->list_results) {
            u32 result_count = 0;
            int result =
                source_test_run_verbose(&arena,
                                        (NerdSource){
                                            .source      = source,
                                            .source_path = config->input_path,
                                        },
                                        tests,
                                        config->input_path,
                                        config,
                                        &result_count);
            if (result == 0) {
                if (config->summary_label.count > 0) {
                    prn(ANSI_BOLD_GREEN STRINGP ": %u tests passed" ANSI_RESET,
                        STRINGV(config->summary_label),
                        result_count);
                } else {
                    prn(ANSI_BOLD_GREEN "%u tests passed" ANSI_RESET,
                        result_count);
                }
            } else {
                eprn(ANSI_BOLD_RED "source test run failed" ANSI_RESET);
            }

            array_free(tests);
            filemap_unload(&map);
            arena_done(&arena);
            return result;
        }

        SourceTestGenerated generated =
            source_test_generated_source(&arena,
                                         (NerdSource){
                                             .source      = source,
                                             .source_path = config->input_path,
                                         },
                                         tests);
        NerdRunConfig run_config = {
            .source =
                (NerdSource){
                    .source      = generated.source,
                    .source_path = config->input_path,
                    .fragments   = generated.fragments,
                },
            .verbose  = config->verbose,
            .keywords = config->keywords,
        };

        int result = compiler_cmd_run(&run_config);
        if (result == 0) {
            if (config->summary_label.count > 0) {
                prn(ANSI_BOLD_GREEN STRINGP ": %u tests passed" ANSI_RESET,
                    STRINGV(config->summary_label),
                    root_selected_count);
            } else {
                prn(ANSI_BOLD_GREEN "%u tests passed" ANSI_RESET,
                    root_selected_count);
            }
        } else {
            eprn(ANSI_BOLD_RED "source test run failed" ANSI_RESET);
        }

        array_free(tests);
        filemap_unload(&map);
        arena_done(&arena);
        return result;
    }

    Array(Array(SourceTestDecl)) module_tests = NULL;
    u32  selected_count                       = 0;
    bool discovered                           = true;
    for (u32 module_index = 0; module_index < array_count(program.modules);
         ++module_index) {
        ModuleInfo* module          = &program.modules[module_index];
        string      prefix          = module_index == program.root_module_index
                                          ? (string){0}
                                          : module->qualified_name;
        Array(SourceTestDecl) tests = NULL;
        if (!source_test_discover(&arena,
                                  module->front_end.lexer.source.source,
                                  prefix,
                                  config->filter,
                                  &tests)) {
            array_free(tests);
            discovered = false;
            break;
        }
        for (usize i = 0; i < array_count(tests); i++) {
            if (tests[i].selected) {
                selected_count += 1;
            }
        }
        array_push(module_tests, tests);
    }

    if (!discovered) {
        for (usize i = 0; i < array_count(module_tests); ++i) {
            array_free(module_tests[i]);
        }
        array_free(module_tests);
        program_info_done(&program);
        filemap_unload(&map);
        arena_done(&arena);
        return 1;
    }

    if (config->list) {
        for (usize module_index = 0; module_index < array_count(module_tests);
             ++module_index) {
            Array(SourceTestDecl) tests = module_tests[module_index];
            for (usize i = 0; i < array_count(tests); i++) {
                if (tests[i].selected) {
                    prn(ANSI_CYAN STRINGP ANSI_RESET, STRINGV(tests[i].name));
                }
            }
        }
        for (usize i = 0; i < array_count(module_tests); ++i) {
            array_free(module_tests[i]);
        }
        array_free(module_tests);
        program_info_done(&program);
        filemap_unload(&map);
        arena_done(&arena);
        return 0;
    }

    if (selected_count == 0) {
        prn(ANSI_BOLD_YELLOW "0 tests passed" ANSI_RESET);
        for (usize i = 0; i < array_count(module_tests); ++i) {
            array_free(module_tests[i]);
        }
        array_free(module_tests);
        program_info_done(&program);
        filemap_unload(&map);
        arena_done(&arena);
        return 0;
    }

    int result       = 0;
    u32 result_count = 0;
    for (u32 module_index = 0; module_index < array_count(program.modules);
         ++module_index) {
        Array(SourceTestDecl) tests = module_tests[module_index];
        bool has_selected           = false;
        for (usize i = 0; i < array_count(tests); i++) {
            if (tests[i].selected) {
                has_selected = true;
                break;
            }
        }
        if (!has_selected) {
            continue;
        }

        ModuleInfo* module        = &program.modules[module_index];
        cstr        generated_dir = path_dirname(&arena, module->resolved_path);
        cstr        generated_path =
            path_join(&arena, generated_dir, "__nerd_source_test");
        if (config->list_results) {
            result = source_test_run_verbose(&arena,
                                             module->front_end.lexer.source,
                                             tests,
                                             s(generated_path),
                                             config,
                                             &result_count);
            if (result != 0) {
                break;
            }
            continue;
        }

        SourceTestGenerated generated = source_test_generated_source(
            &arena, module->front_end.lexer.source, tests);
        NerdRunConfig run_config = {
            .source =
                (NerdSource){
                    .source      = generated.source,
                    .source_path = s(generated_path),
                    .fragments   = generated.fragments,
                },
            .verbose  = config->verbose,
            .keywords = config->keywords,
        };

        result = compiler_cmd_run(&run_config);
        if (result != 0) {
            break;
        }
    }
    if (result == 0) {
        if (config->summary_label.count > 0) {
            prn(ANSI_BOLD_GREEN STRINGP ": %u tests passed" ANSI_RESET,
                STRINGV(config->summary_label),
                selected_count);
        } else {
            prn(ANSI_BOLD_GREEN "%u tests passed" ANSI_RESET, selected_count);
        }
    } else {
        eprn(ANSI_BOLD_RED "source test run failed" ANSI_RESET);
    }

    for (usize i = 0; i < array_count(module_tests); ++i) {
        array_free(module_tests[i]);
    }
    array_free(module_tests);
    program_info_done(&program);
    filemap_unload(&map);
    arena_done(&arena);
    return result;
}

//------------------------------------------------------------------------------
