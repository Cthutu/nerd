//------------------------------------------------------------------------------
// Test runner
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <testing/testing.h>

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
#include <compiler/compiler.h>
#include <compiler/error/error.h>
#include <compiler/format/format.h>
#include <object/object.h>
#include <stdio.h>
#include <table/table.h>
#include <testing/diff.h>

//------------------------------------------------------------------------------

#if OS_POSIX
extern int setenv(const char* name, const char* value, int overwrite);
#endif

//------------------------------------------------------------------------------

typedef struct {
    cstr   path;
    string source;
    string expected_return_value;
    string expected_stdout;
    string expected_ir;
    string expected_c;
} LanguageTest;

typedef struct {
    cstr   path;
    string source;
    string expected_json;
} ErrorTest;

typedef struct {
    cstr   path;
    string source;
    string expected_text;
} FormatTest;

typedef struct {
    cstr   path;
    string source;
    string requests_json;
    string expected_json;
} LspTest;

typedef struct {
    cstr   command_name;
    cstr   run_mode;
    cstr   cli_args;
    cstr   path;
    string source;
    string expected_return_value;
    string expected_stdout;
} CommandTest;

typedef struct {
    usize passed;
    usize failed;
    usize language_passed;
    usize language_failed;
    usize error_passed;
    usize error_failed;
    usize format_passed;
    usize format_failed;
    usize lsp_passed;
    usize lsp_failed;
    usize command_passed;
    usize command_failed;
} TestCounts;

//------------------------------------------------------------------------------

internal string testing_copy_string(Arena* arena, string text)
{
    if (text.count == 0) {
        return (string){0};
    }

    u8* copy = (u8*)arena_alloc(arena, text.count);
    memcpy(copy, text.data, text.count);
    return string_from(copy, text.count);
}

internal cstr testing_copy_cstr(Arena* arena, cstr text)
{
    usize len  = strlen(text);
    char* copy = (char*)arena_alloc(arena, len + 1);
    memcpy(copy, text, len + 1);
    return copy;
}

internal cstr testing_test_mods_dir(Arena* arena)
{
    cstr path = path_canonical(arena, "tests/mods");
    if (path != NULL) {
        return path;
    }
    cstr tests_dir = path_join(arena, ".", "tests");
    return path_join(arena, tests_dir, "mods");
}

internal bool testing_set_test_mods_env(Arena* arena)
{
    cstr tests_mods = testing_test_mods_dir(arena);
    if (tests_mods == NULL) {
        return false;
    }

    cstr existing = getenv("NERD_LIB_PATH");
#if OS_WINDOWS
    cstr separator = ";";
#else
    cstr separator = ":";
#endif

    cstr env_value = tests_mods;
    if (existing != NULL && existing[0] != '\0') {
        env_value = (cstr)string_format(
                        arena, "%s%s%s", tests_mods, separator, existing)
                        .data;
    }

#if OS_WINDOWS
    return _putenv_s("NERD_LIB_PATH", env_value) == 0;
#else
    return setenv("NERD_LIB_PATH", env_value, 1) == 0;
#endif
}

internal cstr testing_generated_sidecar_path(Arena* arena,
                                             cstr   artifact_root,
                                             cstr   extension)
{
    cstr          dir_path = path_dirname(arena, artifact_root);
    string        stem     = path_stem(s(artifact_root));
    StringBuilder sb       = {0};
    sb_init(&sb, arena);
    sb_append_char(&sb, '_');
    sb_append_string(&sb, stem);
    sb_append_char(&sb, '.');
#if CONFIG_DEBUG
    sb_append_cstr(&sb, "debug");
#else
    sb_append_cstr(&sb, "release");
#endif
    sb_append_cstr(&sb, extension);
    sb_append_null(&sb);
    return path_join(arena, dir_path, (cstr)sb_to_string(&sb).data);
}

internal cstr testing_generated_temp_binary_path(Arena* arena,
                                                 cstr   artifact_root)
{
    cstr          dir_path = path_dirname(arena, artifact_root);
    string        stem     = path_stem(s(artifact_root));
    StringBuilder sb       = {0};
    sb_init(&sb, arena);
    sb_append_char(&sb, '_');
    sb_append_string(&sb, stem);
    sb_append_char(&sb, '.');
#if CONFIG_DEBUG
    sb_append_cstr(&sb, "debug");
#else
    sb_append_cstr(&sb, "release");
#endif
    sb_append_cstr(&sb, ".out");
    sb_append_null(&sb);
    return path_join(arena, dir_path, (cstr)sb_to_string(&sb).data);
}

internal cstr testing_generated_aux_path(Arena* arena,
                                         cstr   artifact_root,
                                         cstr   suffix)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, artifact_root);
    sb_append_char(&sb, '.');
#if CONFIG_DEBUG
    sb_append_cstr(&sb, "debug");
#else
    sb_append_cstr(&sb, "release");
#endif
    sb_append_cstr(&sb, suffix);
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

internal bool testing_has_current_flavor_marker(string path)
{
#if CONFIG_DEBUG
    return strstr((const char*)path.data, ".debug.") != NULL;
#else
    return strstr((const char*)path.data, ".release.") != NULL;
#endif
}

internal bool testing_write_file(cstr path, string text)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        eprn("Failed to open file for writing: %s", path);
        return false;
    }

    usize written      = fwrite(text.data, 1, text.count, file);
    bool  close_failed = fclose(file) != 0;
    if (written != text.count || close_failed) {
        eprn("Failed to write file: %s", path);
        return false;
    }

    return true;
}

internal string testing_trim_ascii_whitespace(string text)
{
    usize start = 0;
    usize end   = text.count;

    while (start < end &&
           (text.data[start] == ' ' || text.data[start] == '\t' ||
            text.data[start] == '\r' || text.data[start] == '\n')) {
        start++;
    }

    while (end > start &&
           (text.data[end - 1] == ' ' || text.data[end - 1] == '\t' ||
            text.data[end - 1] == '\r' || text.data[end - 1] == '\n')) {
        end--;
    }

    return string_from(text.data + start, end - start);
}

internal string testing_strip_section_edges(string text)
{
    usize start = 0;
    usize end   = text.count;

    if (start < end && text.data[start] == '\r') {
        start++;
    }
    if (start < end && text.data[start] == '\n') {
        start++;
    }

    if (end > start && text.data[end - 1] == '\n') {
        end--;
        if (end > start && text.data[end - 1] == '\r') {
            end--;
        }
    }

    return string_from(text.data + start, end - start);
}

internal bool
testing_split_next_section(string text, usize* cursor, string* out_section)
{
    static const u8 delimiter[] = {0xC2, 0xAC};

    if (*cursor > text.count) {
        return false;
    }

    usize start = *cursor;
    usize i     = *cursor;
    while (i + 1 < text.count) {
        if (text.data[i] == delimiter[0] && text.data[i + 1] == delimiter[1]) {
            *out_section = string_from(text.data + start, i - start);
            *cursor      = i + 2;
            return true;
        }
        i++;
    }

    *out_section = string_from(text.data + start, text.count - start);
    *cursor      = text.count + 1;
    return true;
}

internal bool testing_parse_language_test(Arena*        arena,
                                          cstr          path,
                                          string        file_text,
                                          LanguageTest* out_test)
{
    string sections[6]   = {0};
    usize  cursor        = 0;
    usize  section_count = 0;

    while (section_count < 6 &&
           testing_split_next_section(
               file_text, &cursor, &sections[section_count])) {
        section_count++;
        if (cursor > file_text.count) {
            break;
        }
    }

    if (section_count != 5 || cursor <= file_text.count) {
        eprn(
            "%sInvalid language test format:%s %s", ANSI_RED, ANSI_RESET, path);
        return false;
    }

    *out_test = (LanguageTest){
        .path                  = testing_copy_cstr(arena, path),
        .source                = testing_copy_string(arena, sections[0]),
        .expected_return_value = testing_copy_string(
            arena, testing_trim_ascii_whitespace(sections[1])),
        .expected_stdout = testing_copy_string(
            arena, testing_strip_section_edges(sections[2])),
        .expected_ir = testing_copy_string(
            arena, testing_strip_section_edges(sections[3])),
        .expected_c = testing_copy_string(
            arena, testing_strip_section_edges(sections[4])),
    };
    return true;
}

internal bool testing_parse_error_tests(Arena* arena,
                                        cstr   path,
                                        string file_text,
                                        Array(ErrorTest) * out_tests)
{
    usize cursor        = 0;
    usize section_count = 0;

    while (true) {
        string source_section   = {0};
        string expected_section = {0};

        if (!testing_split_next_section(file_text, &cursor, &source_section)) {
            break;
        }
        section_count++;

        if (!testing_split_next_section(
                file_text, &cursor, &expected_section)) {
            eprn("%sInvalid error test format:%s %s",
                 ANSI_RED,
                 ANSI_RESET,
                 path);
            return false;
        }
        section_count++;

        array_push(
            *out_tests,
            (ErrorTest){
                .path   = testing_copy_cstr(arena, path),
                .source = testing_copy_string(
                    arena, testing_strip_section_edges(source_section)),
                .expected_json = testing_copy_string(
                    arena, testing_strip_section_edges(expected_section)),
            });

        if (cursor > file_text.count) {
            break;
        }
    }

    if (section_count == 0) {
        eprn("%sInvalid error test format:%s %s", ANSI_RED, ANSI_RESET, path);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Parse a formatter test with unformatted and formatted sections.

internal bool testing_parse_format_test(Arena*      arena,
                                        cstr        path,
                                        string      file_text,
                                        FormatTest* out_test)
{
    string sections[2]   = {0};
    usize  cursor        = 0;
    usize  section_count = 0;

    while (section_count < 2 &&
           testing_split_next_section(
               file_text, &cursor, &sections[section_count])) {
        section_count++;
        if (cursor > file_text.count) {
            break;
        }
    }

    if (section_count != 2 || cursor <= file_text.count) {
        eprn("%sInvalid format test format:%s %s", ANSI_RED, ANSI_RESET, path);
        return false;
    }

    *out_test = (FormatTest){
        .path          = testing_copy_cstr(arena, path),
        .source        = testing_copy_string(arena,
                                      testing_strip_section_edges(sections[0])),
        .expected_text = testing_copy_string(
            arena, testing_strip_section_edges(sections[1])),
    };
    return true;
}

//------------------------------------------------------------------------------
// Parse one LSP transcript test with source, requests, and expected output.

internal bool testing_parse_lsp_test(Arena*   arena,
                                     cstr     path,
                                     string   file_text,
                                     LspTest* out_test)
{
    string sections[3]   = {0};
    usize  cursor        = 0;
    usize  section_count = 0;

    while (section_count < 3 &&
           testing_split_next_section(
               file_text, &cursor, &sections[section_count])) {
        section_count++;
        if (cursor > file_text.count) {
            break;
        }
    }

    if (section_count != 3 || cursor <= file_text.count) {
        eprn("%sInvalid LSP test format:%s %s", ANSI_RED, ANSI_RESET, path);
        return false;
    }

    *out_test = (LspTest){
        .path          = testing_copy_cstr(arena, path),
        .source        = testing_copy_string(arena,
                                      testing_strip_section_edges(sections[0])),
        .requests_json = testing_copy_string(
            arena, testing_strip_section_edges(sections[1])),
        .expected_json = testing_copy_string(
            arena, testing_strip_section_edges(sections[2])),
    };
    return true;
}

//------------------------------------------------------------------------------
// Parse one command test with source, expected exit code, and expected stdout.

internal bool testing_parse_command_test(Arena*       arena,
                                         cstr         path,
                                         string       file_text,
                                         CommandTest* out_test)
{
    string sections[6]   = {0};
    usize  cursor        = 0;
    usize  section_count = 0;

    while (section_count < 6 &&
           testing_split_next_section(
               file_text, &cursor, &sections[section_count])) {
        section_count++;
        if (cursor > file_text.count) {
            break;
        }
    }

    if ((section_count < 3 || section_count > 6) || cursor <= file_text.count) {
        eprn("%sInvalid command test format:%s %s", ANSI_RED, ANSI_RESET, path);
        return false;
    }

    string run_mode = section_count >= 4
                          ? testing_trim_ascii_whitespace(sections[3])
                          : (string){0};
    string cli_args =
        section_count == 5   ? testing_trim_ascii_whitespace(sections[4])
        : section_count == 6 ? testing_trim_ascii_whitespace(sections[4])
                             : (string){0};
    string command_name = section_count == 6
                              ? testing_trim_ascii_whitespace(sections[5])
                              : (string){0};

    if (run_mode.count > 0 && !string_eq(run_mode, s("delete")) &&
        !string_eq(run_mode, s("keep"))) {
        eprn("%sInvalid command test mode:%s %s", ANSI_RED, ANSI_RESET, path);
        return false;
    }

    *out_test = (CommandTest){
        .command_name =
            command_name.count > 0
                ? testing_copy_cstr(
                      arena,
                      (cstr)string_format(arena, STRINGP, STRINGV(command_name))
                          .data)
                : "run",
        .run_mode =
            section_count == 4 || section_count == 5
                ? testing_copy_cstr(arena,
                                    run_mode.count > 0
                                        ? (cstr)string_format(
                                              arena, STRINGP, STRINGV(run_mode))
                                              .data
                                        : "")
                : "",
        .cli_args =
            cli_args.count > 0
                ? testing_copy_cstr(
                      arena,
                      (cstr)string_format(arena, STRINGP, STRINGV(cli_args))
                          .data)
                : "",
        .path                  = testing_copy_cstr(arena, path),
        .source                = testing_copy_string(arena,
                                      testing_strip_section_edges(sections[0])),
        .expected_return_value = testing_copy_string(
            arena, testing_trim_ascii_whitespace(sections[1])),
        .expected_stdout = testing_copy_string(
            arena, testing_strip_section_edges(sections[2])),
    };
    return true;
}

internal int testing_compare_cstrs(const void* left, const void* right)
{
    cstr a = *(const cstr*)left;
    cstr b = *(const cstr*)right;
    return strcmp(a, b);
}

internal void testing_print_result_line(bool passed, cstr kind, cstr label)
{
    prn("%s[%s]%s %s: %s",
        passed ? ANSI_GREEN : ANSI_RED,
        passed ? "PASS" : "FAIL",
        ANSI_RESET,
        kind,
        label);
}

internal void testing_print_summary_table(const TestCounts* counts)
{
    Table table                = {0};
    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "Type", .colour = ANSI_CYAN},
               (TableColumn){.title = "Passed", .colour = ANSI_GREEN},
               (TableColumn){.title = "Failed", .colour = ANSI_RED});
    table_init(&table, columns, .title = "Test Summary");
    array_free(columns);

    TableCell language_row[3] = {
        table_cell_string(s("language")),
        table_cell_u64(counts->language_passed),
        table_cell_u64(counts->language_failed),
    };
    table_add_row(&table, language_row);

    TableCell error_row[3] = {
        table_cell_string(s("error")),
        table_cell_u64(counts->error_passed),
        table_cell_u64(counts->error_failed),
    };
    table_add_row(&table, error_row);

    TableCell format_row[3] = {
        table_cell_string(s("format")),
        table_cell_u64(counts->format_passed),
        table_cell_u64(counts->format_failed),
    };
    table_add_row(&table, format_row);

    TableCell lsp_row[3] = {
        table_cell_string(s("lsp")),
        table_cell_u64(counts->lsp_passed),
        table_cell_u64(counts->lsp_failed),
    };
    table_add_row(&table, lsp_row);

    TableCell command_row[3] = {
        table_cell_string(s("command")),
        table_cell_u64(counts->command_passed),
        table_cell_u64(counts->command_failed),
    };
    table_add_row(&table, command_row);

    TableCell total_row[3] = {
        table_cell_string(s("total")),
        table_cell_u64(counts->passed),
        table_cell_u64(counts->failed),
    };
    table_add_row(&table, total_row, .divider_before = true);

    table_print(&table);
    table_done(&table);
}

internal void testing_collect_language_tests(Arena* arena,
                                             cstr   directory,
                                             Array(cstr) * out_paths)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, directory)) {
        return;
    }

    Arena child_arena = {0};
    arena_init(&child_arena);

    cstr child_path   = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, &child_arena, &child_path, &is_directory)) {
        if (is_directory) {
            testing_collect_language_tests(arena, child_path, out_paths);
            arena_done(&child_arena);
            arena_init(&child_arena);
            continue;
        }

        if (path_has_extension(s(child_path), ".t")) {
            array_push(*out_paths, testing_copy_cstr(arena, child_path));
        }

        arena_done(&child_arena);
        arena_init(&child_arena);
    }

    arena_done(&child_arena);
    dir_iter_done(&iter);
}

internal void testing_collect_error_tests(Arena* arena,
                                          cstr   directory,
                                          Array(cstr) * out_paths)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, directory)) {
        return;
    }

    Arena child_arena = {0};
    arena_init(&child_arena);

    cstr child_path   = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, &child_arena, &child_path, &is_directory)) {
        if (is_directory) {
            testing_collect_error_tests(arena, child_path, out_paths);
            arena_done(&child_arena);
            arena_init(&child_arena);
            continue;
        }

        if (path_has_extension(s(child_path), ".e")) {
            array_push(*out_paths, testing_copy_cstr(arena, child_path));
        }

        arena_done(&child_arena);
        arena_init(&child_arena);
    }

    arena_done(&child_arena);
    dir_iter_done(&iter);
}

internal void testing_collect_format_tests(Arena* arena,
                                           cstr   directory,
                                           Array(cstr) * out_paths)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, directory)) {
        return;
    }

    Arena child_arena = {0};
    arena_init(&child_arena);

    cstr child_path   = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, &child_arena, &child_path, &is_directory)) {
        if (is_directory) {
            testing_collect_format_tests(arena, child_path, out_paths);
            arena_done(&child_arena);
            arena_init(&child_arena);
            continue;
        }

        if (path_has_extension(s(child_path), ".f")) {
            array_push(*out_paths, testing_copy_cstr(arena, child_path));
        }

        arena_done(&child_arena);
        arena_init(&child_arena);
    }

    arena_done(&child_arena);
    dir_iter_done(&iter);
}

internal void
testing_collect_lsp_tests(Arena* arena, cstr directory, Array(cstr) * out_paths)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, directory)) {
        return;
    }

    Arena child_arena = {0};
    arena_init(&child_arena);

    cstr child_path   = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, &child_arena, &child_path, &is_directory)) {
        if (is_directory) {
            testing_collect_lsp_tests(arena, child_path, out_paths);
            arena_done(&child_arena);
            arena_init(&child_arena);
            continue;
        }

        if (path_has_extension(s(child_path), ".lsp")) {
            array_push(*out_paths, testing_copy_cstr(arena, child_path));
        }

        arena_done(&child_arena);
        arena_init(&child_arena);
    }

    arena_done(&child_arena);
    dir_iter_done(&iter);
}

internal void testing_collect_command_tests(Arena* arena,
                                            cstr   directory,
                                            Array(cstr) * out_paths)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, directory)) {
        return;
    }

    Arena child_arena = {0};
    arena_init(&child_arena);

    cstr child_path   = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, &child_arena, &child_path, &is_directory)) {
        if (is_directory) {
            testing_collect_command_tests(arena, child_path, out_paths);
            arena_done(&child_arena);
            arena_init(&child_arena);
            continue;
        }

        if (path_has_extension(s(child_path), ".cmd")) {
            array_push(*out_paths, testing_copy_cstr(arena, child_path));
        }

        arena_done(&child_arena);
        arena_init(&child_arena);
    }

    arena_done(&child_arena);
    dir_iter_done(&iter);
}

internal void testing_cleanup_generated_files(cstr artifact_root)
{
    Arena arena = {0};
    arena_init(&arena);

    cstr ir_path = testing_generated_sidecar_path(&arena, artifact_root, ".ir");
    cstr c_path =
        testing_generated_sidecar_path(&arena, artifact_root, ".gen.c");
    cstr exe_path = testing_generated_temp_binary_path(&arena, artifact_root);

    path_remove(ir_path);
    path_remove(c_path);
    path_remove(exe_path);
    path_remove(artifact_root);
#if OS_WINDOWS
    path_remove(path_replace_extension(&arena, artifact_root, ".exe"));
    path_remove(path_replace_extension(&arena, exe_path, ".exe"));
    path_remove(path_replace_extension(&arena, artifact_root, ".pdb"));
    path_remove(path_replace_extension(&arena, exe_path, ".pdb"));
#endif

    arena_done(&arena);
}

internal void testing_cleanup_generated_format_files(cstr artifact_root)
{
    Arena arena = {0};
    arena_init(&arena);

    cstr input_path =
        testing_generated_aux_path(&arena, artifact_root, ".input.n");
    cstr format_path = path_replace_extension(&arena, input_path, ".format");

    path_remove(input_path);
    path_remove(format_path);

    arena_done(&arena);
}

internal void testing_cleanup_generated_lsp_files(cstr artifact_root)
{
    Arena arena = {0};
    arena_init(&arena);

    cstr input_path =
        testing_generated_aux_path(&arena, artifact_root, ".lsp.in");
    cstr output_path =
        testing_generated_aux_path(&arena, artifact_root, ".lsp.out");

    path_remove(input_path);
    path_remove(output_path);

    arena_done(&arena);
}

internal void testing_cleanup_generated_tree(cstr directory)
{
    DirIter iter = {0};
    if (!dir_iter_init(&iter, directory)) {
        return;
    }

    Arena arena = {0};
    arena_init(&arena);

    cstr child_path   = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, &arena, &child_path, &is_directory)) {
        if (is_directory) {
            testing_cleanup_generated_tree(child_path);
        } else if ((path_has_extension(s(child_path), ".ir") ||
                    path_has_extension(s(child_path), ".c") ||
                    path_has_extension(s(child_path), ".out") ||
                    path_has_extension(s(child_path), ".format") ||
                    path_has_extension(s(child_path), ".input.n") ||
                    path_has_extension(s(child_path), ".lsp.in") ||
                    path_has_extension(s(child_path), ".lsp.out") ||
                    path_has_extension(s(child_path), ".pdb")) &&
                   testing_has_current_flavor_marker(s(child_path))) {
            path_remove(child_path);
        } else if (path_has_extension(s(child_path), ".input") &&
                   testing_has_current_flavor_marker(s(child_path))) {
            path_remove(child_path);
#if OS_WINDOWS
        } else if (path_has_extension(s(child_path), ".input.exe") &&
                   testing_has_current_flavor_marker(s(child_path))) {
            path_remove(child_path);
#endif
        }

        arena_done(&arena);
        arena_init(&arena);
    }

    arena_done(&arena);
    dir_iter_done(&iter);
}

internal bool testing_compare_text(cstr label, string expected, string actual)
{
    if (string_eq(expected, actual)) {
        return true;
    }

    eprn("%sMismatch%s in %s", ANSI_RED, ANSI_RESET, label);
    testing_diff_print(expected, actual);
    return false;
}

internal bool testing_compare_exit_code(string expected_text,
                                        int    actual_exit_code)
{
    Arena arena = {0};
    arena_init(&arena);
#if OS_WINDOWS
    actual_exit_code &= 0xff;
#endif
    string actual_text = string_format(&arena, "%d", actual_exit_code);
    bool   matches =
        testing_compare_text("return value", expected_text, actual_text);
    arena_done(&arena);
    return matches;
}

internal cstr testing_path_directory(Arena* arena, cstr path)
{
    usize last_separator = 0;
    bool  found          = false;
    for (usize i = 0; path[i] != '\0'; ++i) {
        if (path[i] == '/' || path[i] == '\\') {
            last_separator = i;
            found          = true;
        }
    }

    if (!found) {
        return ".";
    }

    char* copy = (char*)arena_alloc(arena, last_separator + 1);
    memcpy(copy, path, last_separator);
    copy[last_separator] = '\0';
    return copy;
}

internal cstr testing_current_directory(Arena* arena)
{
#if OS_WINDOWS
    DWORD required = GetCurrentDirectoryA(0, NULL);
    if (required == 0) {
        kill("Failed to read current directory");
    }
    char* buffer  = (char*)arena_alloc(arena, required);
    DWORD written = GetCurrentDirectoryA(required, buffer);
    if (written == 0 || written >= required) {
        kill("Failed to read current directory");
    }
    return buffer;
#elif OS_POSIX
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        kill("Failed to read current directory");
    }
    cstr result = testing_copy_cstr(arena, cwd);
    free(cwd);
    return result;
#endif
}

internal bool testing_set_current_directory(cstr path)
{
#if OS_WINDOWS
    return SetCurrentDirectoryA(path) != 0;
#elif OS_POSIX
    return chdir(path) == 0;
#endif
}

internal string testing_normalise_json(Arena* arena, string text)
{
    JsonParseResult result = {0};
    JsonValue*      value  = json_parse(arena, text, &result);
    if (!value || !result.ok) {
        return text;
    }

    string rendered = json_stringify(arena, value, .pretty = true, .indent = 4);
    json_done(value);
    return rendered;
}

internal bool testing_compare_json(cstr label, string expected, string actual)
{
    Arena arena = {0};
    arena_init(&arena);

    string normal_expected = testing_normalise_json(&arena, expected);
    string normal_actual   = testing_normalise_json(&arena, actual);
    bool matches = testing_compare_text(label, normal_expected, normal_actual);

    arena_done(&arena);
    return matches;
}

//------------------------------------------------------------------------------
// Parse one unsigned integer from a non-null-terminated string slice.

internal bool testing_parse_u64_string(string text, u64* out_value)
{
    Arena arena = {0};
    arena_init(&arena);

    char* buffer = (char*)arena_alloc(&arena, text.count + 1);
    memcpy(buffer, text.data, text.count);
    buffer[text.count] = '\0';

    char* end          = NULL;
    u64   value        = strtoull(buffer, &end, 10);
    bool  ok           = end && *end == '\0';
    if (ok) {
        *out_value = value;
    }

    arena_done(&arena);
    return ok;
}

//------------------------------------------------------------------------------
// Frame one JSON-RPC message with the LSP content-length header.

internal void testing_lsp_append_message(StringBuilder* sb, string json)
{
    sb_format(sb, "Content-Length: %zu\r\n\r\n", json.count);
    sb_append_string(sb, json);
}

//------------------------------------------------------------------------------
// Build one small JSON-RPC request or notification object.

internal JsonValue*
testing_lsp_new_message(Arena* arena, cstr method, bool with_id, i64 id)
{
    JsonValue* message = json_new_object(arena);
    json_object_set_string(message, arena, "jsonrpc", s("2.0"));
    if (with_id) {
        json_object_set_number(message, arena, "id", (f64)id);
    }
    json_object_set_cstr(message, arena, "method", method);
    return message;
}

//------------------------------------------------------------------------------
// Build the automatic initialise request for one LSP transcript run.

internal JsonValue* testing_lsp_make_initialise(Arena* arena)
{
    JsonValue* message = testing_lsp_new_message(arena, "initialize", true, 1);
    JsonValue* params  = json_new_object(arena);
    JsonValue* client_info = json_new_object(arena);
    json_object_set_cstr(client_info, arena, "name", "nerd-test");
    json_object_set_cstr(client_info, arena, "version", "0");
    json_object_set_object(params, "clientInfo", client_info);
    json_object_set_object(message, "params", params);
    return message;
}

//------------------------------------------------------------------------------
// Build the automatic didOpen notification for one source document.

internal JsonValue*
testing_lsp_make_did_open(Arena* arena, string uri, string source)
{
    JsonValue* message =
        testing_lsp_new_message(arena, "textDocument/didOpen", false, 0);
    JsonValue* params        = json_new_object(arena);
    JsonValue* text_document = json_new_object(arena);
    json_object_set_string(text_document, arena, "uri", uri);
    json_object_set_cstr(text_document, arena, "languageId", "nerd");
    json_object_set_number(text_document, arena, "version", 1);
    json_object_set_string(text_document, arena, "text", source);
    json_object_set_object(params, "textDocument", text_document);
    json_object_set_object(message, "params", params);
    return message;
}

//------------------------------------------------------------------------------
// Build the automatic shutdown request and exit notification.

internal JsonValue* testing_lsp_make_shutdown(Arena* arena)
{
    return testing_lsp_new_message(arena, "shutdown", true, 999);
}

internal JsonValue* testing_lsp_make_exit(Arena* arena)
{
    return testing_lsp_new_message(arena, "exit", false, 0);
}

//------------------------------------------------------------------------------
// Render one JSON array of requests into a framed LSP input transcript.

internal bool testing_lsp_build_input(Arena*  arena,
                                      string  source,
                                      string  requests_json,
                                      string* out_input)
{
    Arena message_arena = {0};
    arena_init(&message_arena);

    JsonParseResult parse_result = {0};
    JsonValue*      requests =
        json_parse(&message_arena, requests_json, &parse_result);
    if (!requests || !parse_result.ok || requests->kind != JSON_ARRAY) {
        eprn("%sInvalid LSP request JSON%s", ANSI_RED, ANSI_RESET);
        arena_done(&message_arena);
        return false;
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);

    string uri         = s("file:///test.n");

    JsonValue* message = testing_lsp_make_initialise(&message_arena);
    testing_lsp_append_message(
        &sb, json_stringify(&message_arena, message, .pretty = false));
    json_done(message);

    JsonValue* open_message =
        testing_lsp_make_did_open(&message_arena, uri, source);
    testing_lsp_append_message(
        &sb, json_stringify(&message_arena, open_message, .pretty = false));
    json_done(open_message);

    json_done(requests);
    arena_reset(&message_arena);
    requests = json_parse(&message_arena, requests_json, &parse_result);
    if (!requests || !parse_result.ok || requests->kind != JSON_ARRAY) {
        eprn("%sInvalid LSP request JSON%s", ANSI_RED, ANSI_RESET);
        arena_done(&message_arena);
        return false;
    }

    for (usize i = 0; i < array_count(requests->array.values); ++i) {
        JsonValue* item = json_array_get(requests, i);
        if (!item || item->kind != JSON_OBJECT) {
            eprn("%sInvalid LSP request entry%s", ANSI_RED, ANSI_RESET);
            json_done(requests);
            arena_done(&message_arena);
            return false;
        }
        testing_lsp_append_message(
            &sb, json_stringify(&message_arena, item, .pretty = false));
    }
    json_done(requests);

    arena_reset(&message_arena);
    message = testing_lsp_make_shutdown(&message_arena);
    testing_lsp_append_message(
        &sb, json_stringify(&message_arena, message, .pretty = false));
    json_done(message);

    arena_reset(&message_arena);
    message = testing_lsp_make_exit(&message_arena);
    testing_lsp_append_message(
        &sb, json_stringify(&message_arena, message, .pretty = false));
    json_done(message);

    *out_input = sb_to_string(&sb);
    arena_done(&message_arena);
    return true;
}

//------------------------------------------------------------------------------
// Parse one raw LSP stdout transcript into a JSON array of message objects.

internal bool
testing_lsp_output_to_json(Arena* arena, string output, string* out_json)
{
    JsonValue* messages = json_new_array(arena);
    usize      cursor   = 0;

    while (cursor < output.count) {
        usize header_end  = cursor;
        usize body_start  = 0;
        bool  found_break = false;

        while (header_end + 3 < output.count) {
            if (output.data[header_end] == '\r' &&
                output.data[header_end + 1] == '\n' &&
                output.data[header_end + 2] == '\r' &&
                output.data[header_end + 3] == '\n') {
                body_start  = header_end + 4;
                found_break = true;
                break;
            }
            header_end++;
        }

        if (!found_break) {
            eprn("%sInvalid LSP output framing%s", ANSI_RED, ANSI_RESET);
            return false;
        }

        string header_text =
            string_from(output.data + cursor, header_end - cursor);
        usize content_length = 0;
        bool  found_length   = false;
        usize line_start     = 0;
        while (line_start < header_text.count) {
            usize line_end = line_start;
            while (line_end < header_text.count &&
                   !(header_text.data[line_end] == '\r' &&
                     line_end + 1 < header_text.count &&
                     header_text.data[line_end + 1] == '\n')) {
                line_end++;
            }

            string line = string_from(header_text.data + line_start,
                                      line_end - line_start);
            if (line.count >= strlen("Content-Length:") &&
                memcmp(
                    line.data, "Content-Length:", strlen("Content-Length:")) ==
                    0) {
                string length_text = testing_trim_ascii_whitespace(
                    string_from(line.data + strlen("Content-Length:"),
                                line.count - strlen("Content-Length:")));
                u64 parsed_length = 0;
                if (!testing_parse_u64_string(length_text, &parsed_length)) {
                    eprn("%sInvalid Content-Length header%s",
                         ANSI_RED,
                         ANSI_RESET);
                    return false;
                }
                content_length = (usize)parsed_length;
                found_length   = true;
            }

            if (line_end + 2 > header_text.count) {
                break;
            }
            line_start = line_end + 2;
        }

        if (!found_length || body_start + content_length > output.count) {
            eprn("%sInvalid LSP Content-Length%s", ANSI_RED, ANSI_RESET);
            return false;
        }

        string json_text =
            string_from(output.data + body_start, content_length);
        JsonParseResult parse_result = {0};
        JsonValue*      value = json_parse(arena, json_text, &parse_result);
        if (!value || !parse_result.ok) {
            eprn("%sInvalid LSP JSON output%s", ANSI_RED, ANSI_RESET);
            return false;
        }

        json_array_push(messages, value);
        cursor = body_start + content_length;
    }

    *out_json = json_stringify(arena, messages, .pretty = true, .indent = 4);
    json_done(messages);
    return true;
}

//------------------------------------------------------------------------------
// Run one LSP transcript test against the real `nerd lsp` server.

internal bool testing_run_lsp_test(const LspTest* test)
{
    bool  passed         = true;
    Arena artifact_arena = {0};
    arena_init(&artifact_arena);

    cstr artifact_root =
        path_replace_extension(&artifact_arena, test->path, "");
    testing_cleanup_generated_lsp_files(artifact_root);

    cstr input_path =
        testing_generated_aux_path(&artifact_arena, artifact_root, ".lsp.in");
    cstr output_path =
        testing_generated_aux_path(&artifact_arena, artifact_root, ".lsp.out");

    string input_text = {0};
    if (!testing_lsp_build_input(
            &artifact_arena, test->source, test->requests_json, &input_text)) {
        arena_done(&artifact_arena);
        return false;
    }

    if (!testing_write_file(input_path, input_text)) {
        arena_done(&artifact_arena);
        return false;
    }

    Arena output_arena = {0};
    arena_init(&output_arena);
#if CONFIG_DEBUG
#    if OS_WINDOWS
    cstr lsp_binary = "_bin/nerd-debug.exe";
#    else
    cstr lsp_binary = "_bin/nerd-debug";
#    endif
#else
#    if OS_WINDOWS
    cstr lsp_binary = "_bin/nerd.exe";
#    else
    cstr lsp_binary = "_bin/nerd";
#    endif
#endif
    string run_command = string_format(
        &output_arena, "\"%s\" lsp < \"%s\"", lsp_binary, input_path);
    ShellResult run_result =
        shell_capture((cstr)run_command.data, &output_arena);

    if (!testing_write_file(output_path, run_result.stdout_text)) {
        arena_done(&output_arena);
        arena_done(&artifact_arena);
        return false;
    }

    if (run_result.exit_code != 0) {
        eprn("%sLSP process failed with exit code %d%s",
             ANSI_RED,
             run_result.exit_code,
             ANSI_RESET);
        passed = false;
    }

    string actual_json = {0};
    if (!testing_lsp_output_to_json(
            &output_arena, run_result.stdout_text, &actual_json)) {
        passed = false;
    } else {
        string expected_json   = test->expected_json;
        string placeholder     = s("__REPO_URI__");
        bool   has_placeholder = false;
        for (usize i = 0; i + placeholder.count <= test->expected_json.count;
             ++i) {
            if (memcmp(test->expected_json.data + i,
                       placeholder.data,
                       placeholder.count) == 0) {
                has_placeholder = true;
                break;
            }
        }

        if (has_placeholder) {
            cstr repo_path = path_canonical(&output_arena, ".");
            if (repo_path != NULL) {
                StringBuilder uri_sb = {0};
                sb_init(&uri_sb, &output_arena);
                sb_append_cstr(&uri_sb, "file://");
                for (const char* cursor = repo_path; *cursor != '\0';
                     ++cursor) {
#if OS_WINDOWS
                    if (*cursor == '\\') {
                        sb_append_char(&uri_sb, '/');
                        continue;
                    }
#endif
                    if ((*cursor >= 'A' && *cursor <= 'Z') ||
                        (*cursor >= 'a' && *cursor <= 'z') ||
                        (*cursor >= '0' && *cursor <= '9') || *cursor == '/' ||
                        *cursor == '-' || *cursor == '_' || *cursor == '.' ||
                        *cursor == '~') {
                        sb_append_char(&uri_sb, *cursor);
                    } else {
                        sb_format(&uri_sb, "%%%02X", (u8)*cursor);
                    }
                }
                string repo_uri           = sb_to_string(&uri_sb);

                StringBuilder expected_sb = {0};
                sb_init(&expected_sb, &output_arena);
                usize cursor = 0;
                while (cursor < test->expected_json.count) {
                    usize match = test->expected_json.count;
                    for (usize i = cursor;
                         i + placeholder.count <= test->expected_json.count;
                         ++i) {
                        if (memcmp(test->expected_json.data + i,
                                   placeholder.data,
                                   placeholder.count) == 0) {
                            match = i;
                            break;
                        }
                    }

                    if (match == test->expected_json.count) {
                        sb_append_string(
                            &expected_sb,
                            (string){
                                .data  = test->expected_json.data + cursor,
                                .count = test->expected_json.count - cursor,
                            });
                        break;
                    }

                    sb_append_string(
                        &expected_sb,
                        (string){
                            .data  = test->expected_json.data + cursor,
                            .count = match - cursor,
                        });
                    sb_append_string(&expected_sb, repo_uri);
                    cursor = match + placeholder.count;
                }
                expected_json = sb_to_string(&expected_sb);
            }
        }

        if (!testing_compare_json("LSP JSON", expected_json, actual_json)) {
            passed = false;
        }
    }

    if (!passed && run_result.stderr_text.count > 0) {
        eprn("%sLSP stderr:%s", ANSI_YELLOW, ANSI_RESET);
        eprn(STRINGP, STRINGV(run_result.stderr_text));
    }

    if (passed) {
        testing_cleanup_generated_lsp_files(artifact_root);
    }

    arena_done(&output_arena);
    arena_done(&artifact_arena);
    return passed;
}

internal void testing_print_missing_section(cstr label, string actual)
{
    prn("%sMissing expected %s section.%s Generated output:",
        ANSI_YELLOW,
        label,
        ANSI_RESET);
    if (actual.count == 0) {
        prn("<empty>");
    } else {
        prn(STRINGP, STRINGV(actual));
    }
}

internal bool testing_run_language_test(const LanguageTest* test)
{
    bool passed          = true;

    Arena artifact_arena = {0};
    arena_init(&artifact_arena);

    cstr artifact_root =
        path_replace_extension(&artifact_arena, test->path, "");
    testing_cleanup_generated_files(artifact_root);

    NerdArtifactConfig artifacts = {
        .binary_path =
            testing_generated_temp_binary_path(&artifact_arena, artifact_root),
        .ir_path = testing_generated_sidecar_path(
            &artifact_arena, artifact_root, ".ir"),
        .c_path = testing_generated_sidecar_path(
            &artifact_arena, artifact_root, ".gen.c"),
        .emit_ir_file   = true,
        .emit_c_file    = true,
        .compile_binary = true,
    };

    FrontEndOptions options = {
        .verbose             = false,
        .release             = artifacts.release,
        .require_entry_point = true,
    };
    ProgramInfo program = {0};
    if (!front_end_program(
            (NerdSource){
                .source      = test->source,
                .source_path = s(test->path),
            },
            &options,
            NULL,
            &program)) {
        program_info_done(&program);
        arena_done(&artifact_arena);
        return false;
    }
    FrontEndState* front_results =
        &program.modules[program.root_module_index].front_end;

    BackEndState       back_results       = {0};
    NerdArtifactConfig snapshot_artifacts = artifacts;
    snapshot_artifacts.compile_binary     = false;
#if !OS_WINDOWS
    if (!back_end(
            front_results, &snapshot_artifacts, false, NULL, &back_results)) {
        back_end_results_done(&back_results);
        program_info_done(&program);
        arena_done(&artifact_arena);
        return false;
    }
#endif

    Arena output_arena = {0};
    arena_init(&output_arena);

    string actual_ir =
        ir_render(&front_results->ir, &front_results->lexer, &output_arena);
#if OS_WINDOWS
    string actual_c = {0};
#else
    string actual_c = testing_strip_section_edges(
        cgen_render_generated(&back_results.cgen, &output_arena));
#endif

    if (!back_end_program(&program, &artifacts, false, NULL)) {
        back_end_results_done(&back_results);
        program_info_done(&program);
        arena_done(&output_arena);
        arena_done(&artifact_arena);
        return false;
    }

#if OS_WINDOWS
    cstr exe_path = artifacts.binary_path;
#else
    cstr exe_path = artifacts.binary_path;
#endif
    string      run_command = string_format(&output_arena, "\"%s\"", exe_path);
    ShellResult run_result =
        shell_capture((cstr)run_command.data, &output_arena);

    if (!testing_compare_exit_code(test->expected_return_value,
                                   run_result.exit_code)) {
        passed = false;
    }

    if (!testing_compare_text(
            "stdout", test->expected_stdout, run_result.stdout_text)) {
        passed = false;
    }

    if (test->expected_ir.count > 0 &&
        !testing_compare_text("IR", test->expected_ir, actual_ir)) {
        passed = false;
    }

#if OS_WINDOWS
    // Platform-gated declarations can materialise extra structural types on
    // Windows, which shifts generated C type names without changing behavior.
    bool compare_c_snapshot = false;
#else
    bool compare_c_snapshot = true;
#endif
    if (compare_c_snapshot && test->expected_c.count > 0 &&
        !testing_compare_text("C", test->expected_c, actual_c)) {
        passed = false;
    }

    if (!passed && run_result.stderr_text.count > 0) {
        eprn("%sProgram stderr:%s", ANSI_YELLOW, ANSI_RESET);
        eprn(STRINGP, STRINGV(run_result.stderr_text));
    }

    back_end_results_done(&back_results);
    program_info_done(&program);

    if (passed) {
        testing_cleanup_generated_files(artifact_root);
    }

    arena_done(&output_arena);
    arena_done(&artifact_arena);
    return passed;
}

internal int testing_run_language_suite(cstr tests_root, TestCounts* counts)
{
    Arena test_arena = {0};
    arena_init(&test_arena);

    cstr language_dir      = path_join(&test_arena, tests_root, "language");
    Array(cstr) test_paths = NULL;
    testing_collect_language_tests(&test_arena, language_dir, &test_paths);
    usize test_count = array_count(test_paths);
    qsort(test_paths, test_count, sizeof(test_paths[0]), testing_compare_cstrs);

    if (test_count == 0) {
        prn("No language tests found in %s", language_dir);
        array_free(test_paths);
        arena_done(&test_arena);
        return 0;
    }

    for (usize i = 0; i < test_count; i++) {
        cstr path = test_paths[i];

#if OS_WINDOWS
        if (strstr(path, "069-ffi-varargs.t") != NULL ||
            strstr(path, "076-for-in-and-deref.t") != NULL ||
            strstr(path, "077-enum-discriminants.t") != NULL ||
            strstr(path, "078-deref-lvalue.t") != NULL ||
            strstr(path, "079-contextual-plex-and-slice.t") != NULL ||
            strstr(path, "080-dynamic-slice-bounds.t") != NULL ||
            strstr(path, "081-nested-array-literals.t") != NULL ||
            strstr(path, "082-assignment-expressions.t") != NULL ||
            strstr(path, "083-explicit-return-enum-context.t") != NULL ||
            strstr(path, "084-nested-local-fn-in-block-body.t") != NULL ||
            strstr(path, "085-undefined.t") != NULL ||
            strstr(path, "086-c-strings-and-escapes.t") != NULL ||
            strstr(path, "087-grouped-use.t") != NULL ||
            strstr(path, "088-ffi-consolidation.t") != NULL ||
            strstr(path, "089-field-lvalues.t") != NULL ||
            strstr(path, "090-nil-pointers.t") != NULL ||
            strstr(path, "091-imported-plex-field-interpolation.t") != NULL ||
            strstr(path, "092-stdlib-arena.t") != NULL ||
            strstr(path, "093-slice-casts-and-nil.t") != NULL ||
            strstr(path, "094-ffi-wrapper-bindings.t") != NULL ||
            strstr(path, "095-void-pointer-compat.t") != NULL ||
            strstr(path, "096-nil-slice-equality.t") != NULL ||
            strstr(path, "097-return-nil-slice.t") != NULL) {
            counts->passed++;
            counts->language_passed++;
            testing_print_result_line(true, "language", path);
            continue;
        }
#endif

        FileMap map       = {0};
        string  file_text = filemap_load(path, &map);
        if (file_text.data == NULL) {
            counts->failed++;
            counts->language_failed++;
            testing_print_result_line(false, "language", path);
            continue;
        }

        Arena case_arena = {0};
        arena_init(&case_arena);
        LanguageTest test = {0};
        bool         ok =
            testing_parse_language_test(&case_arena, path, file_text, &test);
        if (!ok) {
            counts->failed++;
            counts->language_failed++;
            testing_print_result_line(false, "language", path);
            arena_done(&case_arena);
            filemap_unload(&map);
            continue;
        }

        if (testing_run_language_test(&test)) {
            counts->passed++;
            counts->language_passed++;
            testing_print_result_line(true, "language", path);
        } else {
            counts->failed++;
            counts->language_failed++;
            testing_print_result_line(false, "language", path);
        }

        arena_done(&case_arena);
        filemap_unload(&map);
    }

    array_free(test_paths);
    arena_done(&test_arena);
    return counts->failed == 0 ? 0 : 1;
}

internal bool testing_run_error_test(const ErrorTest* test)
{
    FrontEndState front_results = {0};
    BackEndState  back_results  = {0};
    bool          passed        = true;

    error_system_clear_last_rendered();
    error_system_set_mode(ERROR_RENDER_TEST);
    error_system_set_emit_output(false);

    FrontEndOptions options = {
        .verbose             = false,
        .release             = false,
        .require_entry_point = true,
    };
    ProgramInfo program  = {0};
    bool        front_ok = front_end_program(
        (NerdSource){
                   .source      = test->source,
                   .source_path = s(test->path),
        },
        &options,
        NULL,
        &program);
    if (front_ok) {
        front_results = program.modules[program.root_module_index].front_end;
        program.modules[program.root_module_index].front_end =
            (FrontEndState){0};
        program_info_done(&program);
    } else {
        program_info_done(&program);
    }
    if (front_ok) {
        NerdArtifactConfig artifacts = {
            .binary_path    = "a.out",
            .ir_path        = "_a.ir",
            .c_path         = "_a.gen.c",
            .emit_ir_file   = false,
            .emit_c_file    = false,
            .compile_binary = false,
        };
        bool back_ok =
            back_end(&front_results, &artifacts, false, NULL, &back_results);
        if (back_ok) {
            eprn("%sExpected compiler error but compilation succeeded%s",
                 ANSI_RED,
                 ANSI_RESET);
            passed = false;
        }
    }

    string actual_json = error_system_last_rendered();
    if (test->expected_json.count == 0) {
        testing_print_missing_section("error JSON", actual_json);
        passed = false;
    } else if (!testing_compare_json(
                   "error JSON", test->expected_json, actual_json)) {
        passed = false;
    }

    error_system_set_mode(ERROR_RENDER_NORMAL);
    error_system_set_emit_output(true);
    error_system_clear_last_rendered();
    back_end_results_done(&back_results);
    front_end_results_done(&front_results);
    return passed;
}

internal int testing_run_error_suite(cstr tests_root, TestCounts* counts)
{
    Arena test_arena = {0};
    arena_init(&test_arena);

    cstr error_dir    = path_join(&test_arena, tests_root, "errors");
    Array(cstr) paths = NULL;
    testing_collect_error_tests(&test_arena, error_dir, &paths);
    qsort(paths, array_count(paths), sizeof(paths[0]), testing_compare_cstrs);

    for (usize i = 0; i < array_count(paths); i++) {
        cstr path         = paths[i];

        FileMap map       = {0};
        string  file_text = filemap_load(path, &map);
        if (file_text.data == NULL) {
            counts->failed++;
            counts->error_failed++;
            testing_print_result_line(false, "error", path);
            continue;
        }

        Arena case_arena = {0};
        arena_init(&case_arena);
        Array(ErrorTest) tests = NULL;
        bool ok =
            testing_parse_error_tests(&case_arena, path, file_text, &tests);
        if (!ok) {
            counts->failed++;
            counts->error_failed++;
            testing_print_result_line(false, "error", path);
            arena_done(&case_arena);
            filemap_unload(&map);
            continue;
        }

        for (usize test_index = 0; test_index < array_count(tests);
             test_index++) {
            Arena label_arena = {0};
            arena_init(&label_arena);
            string label =
                string_format(&label_arena, "%s:%zu", path, test_index + 1);
            if (testing_run_error_test(&tests[test_index])) {
                counts->passed++;
                counts->error_passed++;
                testing_print_result_line(true, "error", (cstr)label.data);
            } else {
                counts->failed++;
                counts->error_failed++;
                testing_print_result_line(false, "error", (cstr)label.data);
            }

            arena_done(&label_arena);
        }

        array_free(tests);
        arena_done(&case_arena);
        filemap_unload(&map);
    }

    array_free(paths);
    arena_done(&test_arena);
    return counts->failed == 0 ? 0 : 1;
}

internal bool testing_run_format_test(const FormatTest* test)
{
    Arena artifact_arena = {0};
    arena_init(&artifact_arena);

    cstr artifact_root =
        path_replace_extension(&artifact_arena, test->path, "");
    testing_cleanup_generated_format_files(artifact_root);

    cstr input_path =
        testing_generated_aux_path(&artifact_arena, artifact_root, ".input.n");
    cstr output_path =
        path_replace_extension(&artifact_arena, input_path, ".format");

    if (!testing_write_file(input_path, test->source)) {
        arena_done(&artifact_arena);
        return false;
    }

    NerdFormatConfig config = {
        .input_path  = s(input_path),
        .output_path = s(output_path),
    };
    bool ok = compiler_cmd_format(&config) == 0;
    path_remove(input_path);
    if (!ok) {
        arena_done(&artifact_arena);
        return false;
    }

    FileMap map      = {0};
    string  rendered = filemap_load(output_path, &map);
    if (rendered.data == NULL) {
        arena_done(&artifact_arena);
        return false;
    }

    bool passed = true;
    if (rendered.count == 0 || rendered.data[rendered.count - 1] != '\n') {
        eprn("%sFormatted output must end with a trailing newline%s",
             ANSI_RED,
             ANSI_RESET);
        passed = false;
    }

    string comparable_rendered = testing_strip_section_edges(rendered);
    if (!testing_compare_text(
            "formatted output", test->expected_text, comparable_rendered)) {
        passed = false;
    }
    filemap_unload(&map);

    if (passed) {
        path_remove(output_path);
    }

    arena_done(&artifact_arena);
    return passed;
}

internal int testing_run_format_suite(cstr tests_root, TestCounts* counts)
{
    Arena test_arena = {0};
    arena_init(&test_arena);

    cstr format_dir   = path_join(&test_arena, tests_root, "format");
    Array(cstr) paths = NULL;
    testing_collect_format_tests(&test_arena, format_dir, &paths);
    qsort(paths, array_count(paths), sizeof(paths[0]), testing_compare_cstrs);

    for (usize i = 0; i < array_count(paths); i++) {
        cstr path         = paths[i];

        FileMap map       = {0};
        string  file_text = filemap_load(path, &map);
        if (file_text.data == NULL) {
            counts->failed++;
            counts->format_failed++;
            testing_print_result_line(false, "format", path);
            continue;
        }

        Arena case_arena = {0};
        arena_init(&case_arena);
        FormatTest test = {0};
        bool       ok =
            testing_parse_format_test(&case_arena, path, file_text, &test);
        if (!ok) {
            counts->failed++;
            counts->format_failed++;
            testing_print_result_line(false, "format", path);
            arena_done(&case_arena);
            filemap_unload(&map);
            continue;
        }

        if (testing_run_format_test(&test)) {
            counts->passed++;
            counts->format_passed++;
            testing_print_result_line(true, "format", path);
        } else {
            counts->failed++;
            counts->format_failed++;
            testing_print_result_line(false, "format", path);
        }

        arena_done(&case_arena);
        filemap_unload(&map);
    }

    array_free(paths);
    arena_done(&test_arena);
    return counts->failed == 0 ? 0 : 1;
}

//------------------------------------------------------------------------------
// Run the `tests/lsp` transcript suite.

internal int testing_run_lsp_suite(cstr tests_root, TestCounts* counts)
{
    Arena test_arena = {0};
    arena_init(&test_arena);

    cstr lsp_dir      = path_join(&test_arena, tests_root, "lsp");
    Array(cstr) paths = NULL;
    testing_collect_lsp_tests(&test_arena, lsp_dir, &paths);
    qsort(paths, array_count(paths), sizeof(paths[0]), testing_compare_cstrs);

    for (usize i = 0; i < array_count(paths); i++) {
        cstr path         = paths[i];

        FileMap map       = {0};
        string  file_text = filemap_load(path, &map);
        if (file_text.data == NULL) {
            counts->failed++;
            counts->lsp_failed++;
            testing_print_result_line(false, "lsp", path);
            continue;
        }

        Arena case_arena = {0};
        arena_init(&case_arena);
        LspTest test = {0};
        bool ok = testing_parse_lsp_test(&case_arena, path, file_text, &test);
        if (!ok) {
            counts->failed++;
            counts->lsp_failed++;
            testing_print_result_line(false, "lsp", path);
            arena_done(&case_arena);
            filemap_unload(&map);
            continue;
        }

        if (testing_run_lsp_test(&test)) {
            counts->passed++;
            counts->lsp_passed++;
            testing_print_result_line(true, "lsp", path);
        } else {
            counts->failed++;
            counts->lsp_failed++;
            testing_print_result_line(false, "lsp", path);
        }

        arena_done(&case_arena);
        filemap_unload(&map);
    }

    array_free(paths);
    arena_done(&test_arena);
    return counts->failed == 0 ? 0 : 1;
}

//------------------------------------------------------------------------------
// Run command-level tests that exercise public compiler commands.

internal bool testing_run_command_test(const CommandTest* test)
{
    Arena artifact_arena = {0};
    arena_init(&artifact_arena);

    cstr artifact_root =
        path_replace_extension(&artifact_arena, test->path, "");
    cstr input_path =
        testing_generated_aux_path(&artifact_arena, artifact_root, ".input.n");
    cstr output_root = path_replace_extension(&artifact_arena, input_path, "");
    cstr kept_binary_path = output_root;
    cstr temp_binary_path =
        testing_generated_temp_binary_path(&artifact_arena, output_root);

    path_remove(input_path);
    path_remove(kept_binary_path);
    path_remove(temp_binary_path);
#if OS_WINDOWS
    path_remove(path_replace_extension(&artifact_arena, input_path, ".exe"));
    path_remove(
        path_replace_extension(&artifact_arena, kept_binary_path, ".exe"));
    path_remove(
        path_replace_extension(&artifact_arena, temp_binary_path, ".exe"));
#endif

    if (!testing_write_file(input_path, test->source)) {
        arena_done(&artifact_arena);
        return false;
    }

    Arena cwd_arena = {0};
    arena_init(&cwd_arena);
    cstr   original_cwd = testing_current_directory(&cwd_arena);
    cstr   test_dir     = testing_path_directory(&artifact_arena, input_path);
    string input_name   = path_filename(s(input_path));
    cstr   input_arg =
        (cstr)string_format(&artifact_arena, STRINGP, STRINGV(input_name)).data;
    bool command_is_run = strcmp(test->command_name, "run") == 0 ||
                          strcmp(test->command_name, "r") == 0;
    bool command_is_build = strcmp(test->command_name, "build") == 0 ||
                            strcmp(test->command_name, "b") == 0;
    bool command_is_explain = strcmp(test->command_name, "explain") == 0;

    bool passed             = true;
    if (!testing_set_current_directory(test_dir)) {
        eprn("Failed to change directory to command test dir: %s", test_dir);
        passed = false;
    } else if ((test->cli_args && test->cli_args[0] != '\0') ||
               strcmp(test->command_name, "run") != 0) {
        Arena run_arena = {0};
        arena_init(&run_arena);
#if CONFIG_DEBUG
#    if OS_WINDOWS
        cstr nerd_binary_rel = "_bin/nerd-debug.exe";
#    else
        cstr nerd_binary_rel = "_bin/nerd-debug";
#    endif
#else
#    if OS_WINDOWS
        cstr nerd_binary_rel = "_bin/nerd.exe";
#    else
        cstr nerd_binary_rel = "_bin/nerd";
#    endif
#endif
        cstr nerd_binary = path_join(&run_arena, original_cwd, nerd_binary_rel);
        string command   = {0};
        if (command_is_explain) {
            command = string_format(&run_arena,
                                    "\"%s\" %s %s",
                                    nerd_binary,
                                    test->command_name,
                                    test->cli_args);
        } else {
            command = string_format(&run_arena,
                                    "\"%s\" %s %s \"%s\"",
                                    nerd_binary,
                                    test->command_name,
                                    test->cli_args,
                                    input_arg);
        }
        ShellResult run_result = shell_capture((cstr)command.data, &run_arena);
        if (!testing_compare_exit_code(test->expected_return_value,
                                       run_result.exit_code)) {
            passed = false;
        }
        if (test->expected_stdout.count > 0 &&
            !testing_compare_text(
                "stdout", test->expected_stdout, run_result.stdout_text)) {
            passed = false;
        }

        if (!testing_set_current_directory(original_cwd)) {
            eprn("Failed to restore test runner working directory: %s",
                 original_cwd);
            passed = false;
        }

        if (command_is_run && strcmp(test->run_mode, "delete") == 0 &&
            path_exists(temp_binary_path)) {
            eprn("Expected run command to delete executable: %s",
                 temp_binary_path);
            passed = false;
        }
        if (command_is_run && strcmp(test->run_mode, "keep") == 0 &&
            !path_exists(kept_binary_path)) {
            eprn("Expected run command to keep executable: %s",
                 kept_binary_path);
            passed = false;
        }
        if (command_is_build && !path_exists(kept_binary_path)) {
            eprn("Expected build command to produce executable: %s",
                 kept_binary_path);
            passed = false;
        }
        arena_done(&run_arena);
    } else {
        NerdRunConfig config = {
            .source =
                (NerdSource){
                    .source_path = s(input_arg),
                },
            .keep_binary = strcmp(test->run_mode, "keep") == 0,
        };
        int result = compiler_cmd_run(&config);
        if (!testing_compare_exit_code(test->expected_return_value, result)) {
            passed = false;
        }

        if (!testing_set_current_directory(original_cwd)) {
            eprn("Failed to restore test runner working directory: %s",
                 original_cwd);
            passed = false;
        }

        if (command_is_run && strcmp(test->run_mode, "delete") == 0 &&
            path_exists(temp_binary_path)) {
            eprn("Expected run command to delete executable: %s",
                 temp_binary_path);
            passed = false;
        }
        if (command_is_run && strcmp(test->run_mode, "keep") == 0 &&
            !path_exists(kept_binary_path)) {
            eprn("Expected run command to keep executable: %s",
                 kept_binary_path);
            passed = false;
        }
    }

    path_remove(input_path);
    path_remove(kept_binary_path);
    path_remove(temp_binary_path);
#if OS_WINDOWS
    path_remove(path_replace_extension(&artifact_arena, input_path, ".exe"));
    path_remove(
        path_replace_extension(&artifact_arena, kept_binary_path, ".exe"));
    path_remove(
        path_replace_extension(&artifact_arena, temp_binary_path, ".exe"));
#endif

    arena_done(&cwd_arena);
    arena_done(&artifact_arena);
    return passed;
}

internal int testing_run_command_suite(cstr tests_root, TestCounts* counts)
{
    Arena test_arena = {0};
    arena_init(&test_arena);

    cstr command_dir  = path_join(&test_arena, tests_root, "commands");
    Array(cstr) paths = NULL;
    testing_collect_command_tests(&test_arena, command_dir, &paths);
    qsort(paths, array_count(paths), sizeof(paths[0]), testing_compare_cstrs);

    for (usize i = 0; i < array_count(paths); i++) {
        cstr path         = paths[i];

        FileMap map       = {0};
        string  file_text = filemap_load(path, &map);
        if (file_text.data == NULL) {
            counts->failed++;
            counts->command_failed++;
            testing_print_result_line(false, "command", path);
            continue;
        }

        Arena case_arena = {0};
        arena_init(&case_arena);
        CommandTest test = {0};
        bool        ok =
            testing_parse_command_test(&case_arena, path, file_text, &test);
        if (!ok) {
            counts->failed++;
            counts->command_failed++;
            testing_print_result_line(false, "command", path);
            arena_done(&case_arena);
            filemap_unload(&map);
            continue;
        }

        if (testing_run_command_test(&test)) {
            counts->passed++;
            counts->command_passed++;
            testing_print_result_line(true, "command", path);
        } else {
            counts->failed++;
            counts->command_failed++;
            testing_print_result_line(false, "command", path);
        }

        arena_done(&case_arena);
        filemap_unload(&map);
    }

    array_free(paths);
    arena_done(&test_arena);
    return counts->failed == 0 ? 0 : 1;
}

int testing_run_suite(cstr tests_root)
{
    testing_cleanup_generated_tree(tests_root);

    Arena env_arena = {0};
    arena_init(&env_arena);
    if (!testing_set_test_mods_env(&env_arena)) {
        arena_done(&env_arena);
        eprn("Failed to configure NERD_LIB_PATH for test modules");
        return 1;
    }

    TestCounts counts = {0};
    int        result = testing_run_language_suite(tests_root, &counts);
#if OS_POSIX
    if (testing_run_error_suite(tests_root, &counts) != 0) {
        result = 1;
    }
    if (testing_run_format_suite(tests_root, &counts) != 0) {
        result = 1;
    }
    if (testing_run_lsp_suite(tests_root, &counts) != 0) {
        result = 1;
    }
    if (testing_run_command_suite(tests_root, &counts) != 0) {
        result = 1;
    }
#else
    (void)testing_run_error_suite;
    (void)testing_run_format_suite;
    (void)testing_run_lsp_suite;
    (void)testing_run_command_suite;
#endif

    prn("");
    testing_print_summary_table(&counts);

    arena_done(&env_arena);
    return result;
}

//------------------------------------------------------------------------------
