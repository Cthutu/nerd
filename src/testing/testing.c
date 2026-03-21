//------------------------------------------------------------------------------
// Test runner
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <testing/testing.h>

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
#include <testing/diff.h>

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
    usize passed;
    usize failed;
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
    string sections[5]   = {0};
    usize  cursor        = 0;
    usize  section_count = 0;

    while (section_count < 5 &&
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

internal int testing_compare_cstrs(const void* left, const void* right)
{
    cstr a = *(const cstr*)left;
    cstr b = *(const cstr*)right;
    return strcmp(a, b);
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

internal void testing_cleanup_generated_files(cstr artifact_root)
{
    Arena arena = {0};
    arena_init(&arena);

    cstr ir_path  = path_replace_extension(&arena, artifact_root, ".ir");
    cstr c_path   = path_replace_extension(&arena, artifact_root, ".c");
    cstr exe_path = path_replace_extension(&arena, artifact_root, ".out");

    path_remove(ir_path);
    path_remove(c_path);
    path_remove(exe_path);
#if OS_WINDOWS
    path_remove(path_replace_extension(&arena, artifact_root, ".exe"));
#endif

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
        } else if (path_has_extension(s(child_path), ".ir") ||
                   path_has_extension(s(child_path), ".c") ||
                   path_has_extension(s(child_path), ".out")) {
            path_remove(child_path);
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
    string actual_text = string_format(&arena, "%d", actual_exit_code);
    bool   matches =
        testing_compare_text("return value", expected_text, actual_text);
    arena_done(&arena);
    return matches;
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
            path_replace_extension(&artifact_arena, artifact_root, ".out"),
        .ir_path =
            path_replace_extension(&artifact_arena, artifact_root, ".ir"),
        .c_path = path_replace_extension(&artifact_arena, artifact_root, ".c"),
        .emit_ir_file   = true,
        .emit_c_file    = true,
        .compile_binary = true,
    };

    FrontEndState front_results = {0};
    if (!front_end(
            (NerdSource){
                .source      = test->source,
                .source_path = s(test->path),
            },
            NULL,
            &front_results)) {
        front_end_results_done(&front_results);
        arena_done(&artifact_arena);
        return false;
    }

    BackEndState back_results = {0};
    if (!back_end(&front_results, &artifacts, NULL, &back_results)) {
        back_end_results_done(&back_results);
        front_end_results_done(&front_results);
        arena_done(&artifact_arena);
        return false;
    }

    Arena output_arena = {0};
    arena_init(&output_arena);

    string actual_ir = ir_render(&front_results.ir, &output_arena);
    string actual_c  = testing_strip_section_edges(
        cgen_render(&back_results.cgen, &output_arena));

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

    if (test->expected_ir.count == 0) {
        testing_print_missing_section("IR", actual_ir);
        passed = false;
    } else if (!testing_compare_text("IR", test->expected_ir, actual_ir)) {
        passed = false;
    }

    if (test->expected_c.count == 0) {
        testing_print_missing_section("C", actual_c);
        passed = false;
    } else if (!testing_compare_text("C", test->expected_c, actual_c)) {
        passed = false;
    }

    if (!passed && run_result.stderr_text.count > 0) {
        eprn("%sProgram stderr:%s", ANSI_YELLOW, ANSI_RESET);
        eprn(STRINGP, STRINGV(run_result.stderr_text));
    }

    back_end_results_done(&back_results);
    front_end_results_done(&front_results);

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
    qsort(test_paths,
          array_count(test_paths),
          sizeof(test_paths[0]),
          testing_compare_cstrs);

    if (array_count(test_paths) == 0) {
        prn("No language tests found in %s", language_dir);
        array_free(test_paths);
        arena_done(&test_arena);
        return 0;
    }

    for (usize i = 0; i < array_count(test_paths); i++) {
        cstr path = test_paths[i];
        prn("%s[language]%s %s", ANSI_CYAN, ANSI_RESET, path);

        FileMap map       = {0};
        string  file_text = filemap_load(path, &map);
        if (file_text.data == NULL) {
            counts->failed++;
            continue;
        }

        Arena case_arena = {0};
        arena_init(&case_arena);
        LanguageTest test = {0};
        bool         ok =
            testing_parse_language_test(&case_arena, path, file_text, &test);
        if (!ok) {
            counts->failed++;
            arena_done(&case_arena);
            filemap_unload(&map);
            continue;
        }

        if (testing_run_language_test(&test)) {
            counts->passed++;
            prn("%sPASS%s %s", ANSI_GREEN, ANSI_RESET, path);
        } else {
            counts->failed++;
            eprn("%sFAIL%s %s", ANSI_RED, ANSI_RESET, path);
        }

        arena_done(&case_arena);
        filemap_unload(&map);
    }

    array_free(test_paths);
    arena_done(&test_arena);
    return counts->failed == 0 ? 0 : 1;
}

int testing_run_suite(cstr tests_root)
{
    testing_cleanup_generated_tree(tests_root);

    TestCounts counts = {0};
    int        result = testing_run_language_suite(tests_root, &counts);

    prn("");
    prn("Test summary: %s%zu passed%s, %s%zu failed%s",
        ANSI_GREEN,
        counts.passed,
        ANSI_RESET,
        counts.failed == 0 ? ANSI_GREEN : ANSI_RED,
        counts.failed,
        ANSI_RESET);

    return result;
}

//------------------------------------------------------------------------------
