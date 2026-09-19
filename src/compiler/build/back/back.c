//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#ifndef _WIN32
#    define _POSIX_C_SOURCE 200809L
#endif

#include <compiler/internal.h>
#if OS_POSIX
#    include <sys/stat.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <compiler/build/back/back.h>
#include <compiler/build/back/llvm_runtime.h>
#include <compiler/build/back/llvm_text.h>
#include <compiler/build/front/front.h>
#include <compiler/cgen/cgen.h>
#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>

//------------------------------------------------------------------------------

typedef enum {
    BACK_END_LLVM_TOOL_DIAG_NONE,
    BACK_END_LLVM_TOOL_DIAG_SOURCE_ERROR,
    BACK_END_LLVM_TOOL_DIAG_UNDEFINED_SYMBOL,
    BACK_END_LLVM_TOOL_DIAG_MISSING_FILE,
    BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT,
} BackEndLlvmToolDiagnosticKind;

typedef struct {
    BackEndLlvmToolDiagnosticKind kind;
    string                        tool;
    string                        message;
    string                        path;
    string                        line;
    string                        column;
    string                        source_line;
    string                        caret_line;
    string                        symbol;
    string                        reference;
} BackEndLlvmToolDiagnostic;

internal cstr back_end_cstr(Arena* arena, string text)
{
    char* copy = (char*)arena_alloc(arena, text.count + 1);
    memcpy(copy, text.data, text.count);
    copy[text.count] = '\0';
    return copy;
}

internal void back_end_append_hir_extern_link_flags(StringBuilder* link_flags,
                                                    const ProgramInfo* program,
                                                    cstr separator)
{
    for (u32 module_index = 0; module_index < array_count(program->modules);
         ++module_index) {
        const Hir* hir = &program->modules[module_index].front_end.hir;
        for (u32 i = 0; i < array_count(hir->externs); ++i) {
            string library = hir->externs[i].library;
            if (library.count == 0) {
                continue;
            }
            if (string_eq(library, s("c")) || string_eq(library, s("nrt"))) {
                continue;
            }
#if OS_WINDOWS || OS_MACOS
            if (string_eq(library, s("m"))) {
                continue;
            }
#endif
#if OS_WINDOWS
            // The native linker configuration supplies the Windows CRT. Naming
            // it as an FFI library resolves the DLL for symbol validation, but
            // there is no matching `ucrtbase.lib` import library, and an
            // explicit
            // `-lucrt` clashes with the statically linked `libucrt`. Treat the
            // CRT like `c`: let the configured runtime libraries provide the
            // symbols.
            if (string_eq(library, s("ucrtbase")) ||
                string_eq(library, s("ucrtbase.dll")) ||
                string_eq(library, s("ucrt")) ||
                string_eq(library, s("msvcrt")) ||
                string_eq(library, s("msvcrt.dll"))) {
                continue;
            }
#endif

            bool already_added = false;
            for (u32 previous_module = 0; previous_module <= module_index;
                 ++previous_module) {
                const Hir* previous_hir =
                    &program->modules[previous_module].front_end.hir;
                u32 end = previous_module == module_index
                              ? i
                              : (u32)array_count(previous_hir->externs);
                for (u32 j = 0; j < end; ++j) {
                    if (string_eq(previous_hir->externs[j].library, library)) {
                        already_added = true;
                        break;
                    }
                }
                if (already_added) {
                    break;
                }
            }
            if (!already_added) {
                sb_format(
                    link_flags, "%s-l" STRINGP, separator, STRINGV(library));
            }
        }
    }
}

internal bool back_end_write_text_file(cstr path, string text)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(text.data, 1, text.count, file);
    bool  close_failed = fclose(file) != 0;
    if (written != text.count || close_failed) {
        return error_runtime("Failed to write file: %s", path);
    }
    return true;
}

internal string back_end_tool_output_excerpt(ShellResult result)
{
    string output =
        result.stderr_text.count > 0 ? result.stderr_text : result.stdout_text;
    if (output.count > 2048) {
        output.count = 2048;
    }
    return output;
}

internal usize back_end_cstr_len(cstr text)
{
    usize len = 0;
    while (text[len] != '\0') {
        ++len;
    }
    return len;
}

internal bool back_end_string_starts_with_cstr(string text, cstr prefix)
{
    usize prefix_len = back_end_cstr_len(prefix);
    return text.count >= prefix_len &&
           memcmp(text.data, prefix, prefix_len) == 0;
}

internal bool back_end_string_find_cstr(string text, cstr needle, usize* out)
{
    usize needle_len = back_end_cstr_len(needle);
    if (needle_len == 0 || text.count < needle_len) {
        return false;
    }
    for (usize i = 0; i + needle_len <= text.count; ++i) {
        if (memcmp(text.data + i, needle, needle_len) == 0) {
            if (out != NULL) {
                *out = i;
            }
            return true;
        }
    }
    return false;
}

internal bool back_end_next_line(string text, usize* cursor, string* out)
{
    if (*cursor >= text.count) {
        return false;
    }

    usize start = *cursor;
    usize end   = start;
    while (end < text.count && text.data[end] != '\n') {
        ++end;
    }
    *cursor = end < text.count ? end + 1 : end;
    if (end > start && text.data[end - 1] == '\r') {
        --end;
    }
    *out = string_from(text.data + start, end - start);
    return true;
}

internal string back_end_string_trim_left(string text)
{
    while (text.count > 0 && (text.data[0] == ' ' || text.data[0] == '\t')) {
        ++text.data;
        --text.count;
    }
    return text;
}

internal bool back_end_is_digit(u8 c) { return c >= '0' && c <= '9'; }

internal bool
back_end_parse_llvm_source_error_line(string                     line,
                                      BackEndLlvmToolDiagnostic* out)
{
    cstr  marker     = ": error: ";
    usize marker_len = back_end_cstr_len(marker);
    for (usize path_end = 0; path_end < line.count; ++path_end) {
        if (line.data[path_end] != ':' || path_end + 1 >= line.count ||
            !back_end_is_digit(line.data[path_end + 1])) {
            continue;
        }

        usize line_start = path_end + 1;
        usize line_end   = line_start;
        while (line_end < line.count &&
               back_end_is_digit(line.data[line_end])) {
            ++line_end;
        }
        if (line_end >= line.count || line.data[line_end] != ':' ||
            line_end + 1 >= line.count ||
            !back_end_is_digit(line.data[line_end + 1])) {
            continue;
        }

        usize column_start = line_end + 1;
        usize column_end   = column_start;
        while (column_end < line.count &&
               back_end_is_digit(line.data[column_end])) {
            ++column_end;
        }
        if (column_end + marker_len > line.count ||
            memcmp(line.data + column_end, marker, marker_len) != 0) {
            continue;
        }

        out->kind = BACK_END_LLVM_TOOL_DIAG_SOURCE_ERROR;
        out->tool = s("LLVM");
        out->path = string_from(line.data, path_end);
        out->line = string_from(line.data + line_start, line_end - line_start);
        out->column =
            string_from(line.data + column_start, column_end - column_start);
        out->message = string_from(line.data + column_end + marker_len,
                                   line.count - column_end - marker_len);
        return true;
    }
    return false;
}

internal bool back_end_parse_llvm_source_error(string output,
                                               BackEndLlvmToolDiagnostic* out)
{
    usize  cursor = 0;
    string line   = {0};
    while (back_end_next_line(output, &cursor, &line)) {
        if (!back_end_parse_llvm_source_error_line(line, out)) {
            continue;
        }

        string source_line = {0};
        string caret_line  = {0};
        if (back_end_next_line(output, &cursor, &source_line)) {
            out->source_line = source_line;
            if (back_end_next_line(output, &cursor, &caret_line)) {
                out->caret_line = caret_line;
            }
        }
        return true;
    }
    return false;
}

internal bool
back_end_parse_lld_undefined_symbol(string                     output,
                                    BackEndLlvmToolDiagnostic* out)
{
    cstr   marker     = "undefined symbol: ";
    usize  marker_len = back_end_cstr_len(marker);
    usize  cursor     = 0;
    string line       = {0};
    while (back_end_next_line(output, &cursor, &line)) {
        usize marker_offset = 0;
        if (!back_end_string_find_cstr(line, marker, &marker_offset)) {
            continue;
        }

        out->kind = BACK_END_LLVM_TOOL_DIAG_UNDEFINED_SYMBOL;
        out->tool =
            string_from(line.data, marker_offset > 0 ? marker_offset : 0);
        out->message     = s("undefined symbol");
        out->symbol      = string_from(line.data + marker_offset + marker_len,
                                       line.count - marker_offset - marker_len);

        string reference = {0};
        while (back_end_next_line(output, &cursor, &reference)) {
            reference = back_end_string_trim_left(reference);
            if (reference.count == 0 ||
                back_end_string_starts_with_cstr(reference, ">>>")) {
                continue;
            }
            out->reference = reference;
            break;
        }
        return true;
    }
    return false;
}

internal bool back_end_parse_lld_missing_file(string                     output,
                                              BackEndLlvmToolDiagnostic* out)
{
    cstr   prefix        = "lld-link: error: could not open '";
    cstr   suffix        = "': no such file or directory";
    usize  prefix_len    = back_end_cstr_len(prefix);
    usize  suffix_offset = 0;
    usize  cursor        = 0;
    string line          = {0};
    while (back_end_next_line(output, &cursor, &line)) {
        if (!back_end_string_starts_with_cstr(line, prefix) ||
            !back_end_string_find_cstr(line, suffix, &suffix_offset) ||
            suffix_offset < prefix_len) {
            continue;
        }

        out->kind    = BACK_END_LLVM_TOOL_DIAG_MISSING_FILE;
        out->tool    = s("lld-link");
        out->message = s("could not open file");
        out->path =
            string_from(line.data + prefix_len, suffix_offset - prefix_len);
        return true;
    }
    return false;
}

internal bool back_end_parse_ld_missing_library(string output,
                                                BackEndLlvmToolDiagnostic* out)
{
    cstr   prefix        = "/usr/bin/ld: cannot find ";
    cstr   suffix        = ": No such file or directory";
    usize  prefix_len    = back_end_cstr_len(prefix);
    usize  suffix_offset = 0;
    usize  cursor        = 0;
    string line          = {0};
    while (back_end_next_line(output, &cursor, &line)) {
        if (!back_end_string_starts_with_cstr(line, prefix) ||
            !back_end_string_find_cstr(line, suffix, &suffix_offset) ||
            suffix_offset < prefix_len) {
            continue;
        }

        out->kind    = BACK_END_LLVM_TOOL_DIAG_MISSING_FILE;
        out->tool    = s("ld");
        out->message = s("could not find library");
        out->path =
            string_from(line.data + prefix_len, suffix_offset - prefix_len);
        return true;
    }
    return false;
}

internal bool back_end_parse_lld_locked_output(string output,
                                               BackEndLlvmToolDiagnostic* out)
{
    cstr   prefix            = "lld-link: error: could not open '";
    cstr   permission_suffix = "': Permission denied";
    cstr   access_suffix     = "': Access is denied";
    usize  prefix_len        = back_end_cstr_len(prefix);
    usize  permission_offset = 0;
    usize  access_offset     = 0;
    usize  cursor            = 0;
    string line              = {0};
    while (back_end_next_line(output, &cursor, &line)) {
        bool permission_denied =
            back_end_string_find_cstr(
                line, permission_suffix, &permission_offset) &&
            permission_offset >= prefix_len;
        bool access_denied =
            back_end_string_find_cstr(line, access_suffix, &access_offset) &&
            access_offset >= prefix_len;
        if (!back_end_string_starts_with_cstr(line, prefix) ||
            (!permission_denied && !access_denied)) {
            continue;
        }

        usize suffix_offset =
            permission_denied ? permission_offset : access_offset;
        out->kind    = BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT;
        out->tool    = s("lld-link");
        out->message = s("could not open output file");
        out->path =
            string_from(line.data + prefix_len, suffix_offset - prefix_len);
        return true;
    }
    return false;
}

internal bool
back_end_parse_lld_failed_write_output(string                     output,
                                       BackEndLlvmToolDiagnostic* out)
{
    cstr   prefix        = "lld-link: error: failed to write output '";
    cstr   suffix        = "': permission denied";
    usize  prefix_len    = back_end_cstr_len(prefix);
    usize  suffix_offset = 0;
    usize  cursor        = 0;
    string line          = {0};
    while (back_end_next_line(output, &cursor, &line)) {
        if (!back_end_string_starts_with_cstr(line, prefix) ||
            !back_end_string_find_cstr(line, suffix, &suffix_offset) ||
            suffix_offset < prefix_len) {
            continue;
        }

        out->kind    = BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT;
        out->tool    = s("lld-link");
        out->message = s("failed to write output file");
        out->path =
            string_from(line.data + prefix_len, suffix_offset - prefix_len);
        return true;
    }
    return false;
}

internal bool back_end_parse_clang_locked_output(string output,
                                                 BackEndLlvmToolDiagnostic* out)
{
    cstr   prefix        = "clang: error: unable to open output file '";
    cstr   suffix        = "': Permission denied";
    usize  prefix_len    = back_end_cstr_len(prefix);
    usize  suffix_offset = 0;
    usize  cursor        = 0;
    string line          = {0};
    while (back_end_next_line(output, &cursor, &line)) {
        if (!back_end_string_starts_with_cstr(line, prefix) ||
            !back_end_string_find_cstr(line, suffix, &suffix_offset) ||
            suffix_offset < prefix_len) {
            continue;
        }

        out->kind    = BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT;
        out->tool    = s("clang");
        out->message = s("could not open output file");
        out->path =
            string_from(line.data + prefix_len, suffix_offset - prefix_len);
        return true;
    }
    return false;
}

internal bool back_end_parse_llvm_tool_output(string                     output,
                                              BackEndLlvmToolDiagnostic* out)
{
    *out = (BackEndLlvmToolDiagnostic){0};
    if (back_end_parse_llvm_source_error(output, out)) {
        return true;
    }
    if (back_end_parse_lld_undefined_symbol(output, out)) {
        return true;
    }
    if (back_end_parse_lld_missing_file(output, out)) {
        return true;
    }
    if (back_end_parse_ld_missing_library(output, out)) {
        return true;
    }
    if (back_end_parse_lld_locked_output(output, out)) {
        return true;
    }
    if (back_end_parse_lld_failed_write_output(output, out)) {
        return true;
    }
    if (back_end_parse_clang_locked_output(output, out)) {
        return true;
    }
    return false;
}

internal bool back_end_report_llvm_tool_failure(Arena*      arena,
                                                ShellResult result,
                                                string      command,
                                                cstr        combined_llvm_path,
                                                cstr        runtime_object_path)
{
    string output = back_end_tool_output_excerpt(result);
    if (runtime_object_path == NULL) {
        runtime_object_path = "(none)";
    }

    BackEndLlvmToolDiagnostic diagnostic = {0};
    if (!back_end_parse_llvm_tool_output(output, &diagnostic)) {
        return error_runtime("LLVM tool failed; ensure the LLVM tools and host "
                             "SDK are installed.\n"
                             "Exit code: %d\n"
                             "Command: " STRINGP "\n"
                             "Generated LLVM: %s\n"
                             "Runtime object: %s\n"
                             "Tool output:\n" STRINGP,
                             result.exit_code,
                             STRINGV(command),
                             combined_llvm_path,
                             runtime_object_path,
                             STRINGV(output));
    }

    (void)arena;
    if (diagnostic.kind == BACK_END_LLVM_TOOL_DIAG_SOURCE_ERROR) {
        return error_runtime(
            "LLVM tool reported an error while compiling generated IR "
            "(exit code %d)\n"
            "Message: " STRINGP "\n"
            "Location: " STRINGP ":" STRINGP ":" STRINGP "\n"
            "Generated LLVM: %s\n"
            "Runtime object: %s\n"
            "Command: " STRINGP "\n"
            "Source:\n" STRINGP "\n" STRINGP,
            result.exit_code,
            STRINGV(diagnostic.message),
            STRINGV(diagnostic.path),
            STRINGV(diagnostic.line),
            STRINGV(diagnostic.column),
            combined_llvm_path,
            runtime_object_path,
            STRINGV(command),
            STRINGV(diagnostic.source_line),
            STRINGV(diagnostic.caret_line));
    }

    if (diagnostic.kind == BACK_END_LLVM_TOOL_DIAG_UNDEFINED_SYMBOL) {
        return error_runtime(
            "LLVM linker reported an undefined symbol (exit code %d)\n"
            "Symbol: " STRINGP "\n"
            "Referenced by: " STRINGP "\n"
            "Generated LLVM: %s\n"
            "Runtime object: %s\n"
            "Command: " STRINGP,
            result.exit_code,
            STRINGV(diagnostic.symbol),
            STRINGV(diagnostic.reference),
            combined_llvm_path,
            runtime_object_path,
            STRINGV(command));
    }

    if (diagnostic.kind == BACK_END_LLVM_TOOL_DIAG_MISSING_FILE) {
        return error_runtime(
            "LLVM linker could not open a required file (exit code %d)\n"
            "File: " STRINGP "\n"
            "Generated LLVM: %s\n"
            "Runtime object: %s\n"
            "Command: " STRINGP,
            result.exit_code,
            STRINGV(diagnostic.path),
            combined_llvm_path,
            runtime_object_path,
            STRINGV(command));
    }

    if (diagnostic.kind == BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT) {
        return error_runtime(
            "Could not write the output executable (exit code %d)\n"
            "File: " STRINGP "\n"
            "The existing executable is probably still running or locked by "
            "another process. Stop that process, or build to a different "
            "output path with `-o`.\n"
            "Generated LLVM: %s\n"
            "Runtime object: %s\n"
            "Command: " STRINGP,
            result.exit_code,
            STRINGV(diagnostic.path),
            combined_llvm_path,
            runtime_object_path,
            STRINGV(command));
    }

    return error_ice("Unhandled LLVM tool diagnostic kind: %u",
                     diagnostic.kind);
}

internal TimePoint back_end_timing_begin(Timing* timing)
{
    return timing != NULL ? time_now() : 0;
}

internal void back_end_timing_end(Timing* timing, cstr phase, TimePoint start)
{
    if (timing == NULL) {
        return;
    }
    timing_add(timing,
               COMPILER_STAGE_BACK_END,
               phase,
               time_elapsed(start, time_now()));
}

internal NerdArtifactConfig compiler_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .binary_path         = "a.out",
        .hir_path            = "_a.hir",
        .llvm_path           = "_a.ll",
        .emit_hir_file       = false,
        .emit_llvm_file      = false,
        .output_kind         = NERD_BUILD_OUTPUT_Executable,
        .require_entry_point = true,
        .release             = false,
    };
}

internal cstr back_end_module_llvm_path(Arena*                    arena,
                                        const NerdArtifactConfig* artifacts,
                                        u32                       module_index)
{
    if (module_index == 0) {
        return artifacts->llvm_path;
    }
    return back_end_cstr(
        arena,
        string_format(
            arena, "%s.m%u.ll", artifacts->binary_path, module_index));
}

internal void back_end_cleanup_llvm_artifacts(Array(cstr) llvm_paths,
                                              bool remove_llvm_paths,
                                              bool remove_combined_llvm_path,
                                              cstr combined_llvm_path,
                                              cstr runtime_object_path)
{
    if (remove_combined_llvm_path && combined_llvm_path != NULL) {
        path_remove(combined_llvm_path);
    }
    if (runtime_object_path != NULL) {
        path_remove(runtime_object_path);
    }
    if (remove_llvm_paths) {
        for (u32 i = 0; i < array_count(llvm_paths); ++i) {
            path_remove(llvm_paths[i]);
        }
    }
}

typedef struct {
    Arena  arena;
    string llvm;
} BackEndLlvmModuleResult;

typedef struct {
    Array(BackEndLlvmModuleResult) results;
    Array(cstr) llvm_paths;
    Array(string) module_llvms;
    Array(u32) init_module_indices;
} BackEndLlvmModules;

internal void back_end_llvm_render_results_done(BackEndLlvmModules* modules)
{
    for (usize i = 0; i < array_count(modules->results); ++i) {
        if (modules->results[i].arena.data != NULL) {
            arena_done(&modules->results[i].arena);
        }
    }
    array_free(modules->results);
    array_free(modules->module_llvms);
}

internal void back_end_llvm_modules_done(BackEndLlvmModules* modules)
{
    back_end_llvm_render_results_done(modules);
    array_free(modules->llvm_paths);
    array_free(modules->init_module_indices);
    *modules = (BackEndLlvmModules){0};
}

internal bool back_end_llvm_module_defines_init(string module_llvm,
                                                u32    module_index)
{
    char needle[64] = {0};
    snprintf(needle, sizeof(needle), "define void @m%u.init()", module_index);
    usize needle_count = strlen(needle);
    if (needle_count == 0 || module_llvm.count < needle_count) {
        return false;
    }
    for (usize i = 0; i + needle_count <= module_llvm.count; ++i) {
        if (memcmp(module_llvm.data + i, needle, needle_count) == 0) {
            return true;
        }
    }
    return false;
}

internal bool back_end_render_llvm_modules(Arena*                    arena,
                                           const ProgramInfo*        program,
                                           const NerdArtifactConfig* artifacts,
                                           BackEndLlvmModules*       out)
{
    u32 module_count = (u32)array_count(program->modules);
    // Freeze result slots before rendering. Each arena owns only its module's
    // output, while paths and combined text belong to the coordinator arena.
    array_requires_capacity(out->results, module_count);
    for (u32 i = 0; i < module_count; ++i) {
        array_push(out->results, ((BackEndLlvmModuleResult){0}));
    }
    array_requires_capacity(out->module_llvms, module_count);
    array_requires_capacity(out->init_module_indices, module_count);
    if (artifacts->emit_llvm_file) {
        array_requires_capacity(out->llvm_paths, module_count);
    }

    const char* debug_sidecars = getenv("NERD_DEBUG_LLVM_SIDECARS");
    bool        emit_debug_sidecars =
        debug_sidecars != NULL && strcmp(debug_sidecars, "1") == 0;

    for (u32 i = 0; i < module_count; ++i) {
        const FrontEndState*     front_end = &program->modules[i].front_end;
        BackEndLlvmModuleResult* result    = &out->results[i];
        TimingProbe              probe     = timing_probe_begin();
        arena_init(&result->arena);
        string module_llvm = llvm_render_hir(&front_end->hir,
                                             &front_end->lexer,
                                             &front_end->sema,
                                             &result->arena,
                                             !artifacts->release,
                                             artifacts->output_kind !=
                                                 NERD_BUILD_OUTPUT_Executable);
        timing_probe_end(probe,
                         COMPILER_STAGE_BACK_END,
                         COMPILER_PHASE_LLVM_RENDER,
                         front_end->lexer.source.source_path,
                         true,
                         module_llvm.count);
        result->llvm = module_llvm;
        array_push(out->module_llvms, result->llvm);
        if (artifacts->emit_llvm_file) {
            string sidecar_llvm = module_llvm;
            if (!artifacts->release && !emit_debug_sidecars) {
                TimingProbe sidecar_probe = timing_probe_begin();
                sidecar_llvm              = llvm_render_hir(
                    &front_end->hir,
                    &front_end->lexer,
                    &front_end->sema,
                    &result->arena,
                    false,
                    artifacts->output_kind != NERD_BUILD_OUTPUT_Executable);
                timing_probe_end(sidecar_probe,
                                 COMPILER_STAGE_BACK_END,
                                 "render LLVM sidecar",
                                 front_end->lexer.source.source_path,
                                 true,
                                 sidecar_llvm.count);
            }
            cstr llvm_path = back_end_module_llvm_path(arena, artifacts, i);
            if (!back_end_write_text_file(llvm_path, sidecar_llvm)) {
                return false;
            }
            array_push(out->llvm_paths, llvm_path);
        } else {
            cstr llvm_path = back_end_module_llvm_path(arena, artifacts, i);
            nerd_side_file_register_cleanup(artifacts->side_files, llvm_path);
        }
        if (back_end_llvm_module_defines_init(module_llvm, i)) {
            array_push(out->init_module_indices, i);
        }
    }
    return true;
}

// Direct LLVM toolchain. The host SDK supplies CRT objects and system
// libraries; Nerd never uses a C compiler driver to discover or invoke these
// tools.
internal bool
back_end_run_tool(Arena* arena, string command, cstr ir, cstr runtime)
{
    TimingProbe probe  = timing_probe_begin();
    ShellResult result = shell_capture(back_end_cstr(arena, command), arena);
    usize       end    = 0;
    while (end < command.count && command.data[end] != ' ') {
        ++end;
    }
    // All command names here are literal LLVM tool names.
    char  tool[32] = {0};
    usize count    = end < sizeof(tool) - 1 ? end : sizeof(tool) - 1;
    memcpy(tool, command.data, count);
    timing_probe_end(
        probe, "tool", tool, (string){0}, result.exit_code == 0, 0);
    return result.exit_code == 0 || back_end_report_llvm_tool_failure(
                                        arena, result, command, ir, runtime);
}

internal bool back_end_compile_object(Arena*                    arena,
                                      const NerdArtifactConfig* artifacts,
                                      cstr combined_llvm_path,
                                      cstr object_path)
{
    cstr input     = combined_llvm_path;
    cstr optimized = NULL;
    if (artifacts->release) {
        optimized = back_end_cstr(
            arena, string_format(arena, "%s.opt.bc", object_path));
        nerd_side_file_register_cleanup(artifacts->side_files, optimized);
        string command =
            string_format(arena,
                          "opt \"-passes=default<O2>\" -o \"%s\" \"%s\"",
                          optimized,
                          input);
        if (!back_end_run_tool(arena, command, input, NULL)) {
            return false;
        }
        input = optimized;
    }
    string command = string_format(
        arena,
        "llc -filetype=obj -relocation-model=pic -O%s -o \"%s\" \"%s\"",
        artifacts->release ? "2" : "0",
        object_path,
        input);
    if (!back_end_run_tool(arena, command, combined_llvm_path, NULL)) {
        return false;
    }
    if (optimized) {
        path_remove(optimized);
    }
    return true;
}

#if OS_LINUX
// Common native glibc/musl layouts. A nonstandard SDK can supply its library
// directory explicitly, without depending on GCC or Clang discovery commands.
internal cstr back_end_linux_crt_dir(Arena* arena)
{
    cstr configured = getenv("NERD_CRT_DIR");
    if (configured && configured[0]) {
        return path_exists(path_join(arena, configured, "crti.o")) ? configured
                                                                   : NULL;
    }
    static cstr dirs[] = {
#    if ARCH_X86_64
        "/usr/lib/x86_64-linux-gnu",
        "/usr/lib64",
#    elif ARCH_ARM64
        "/usr/lib/aarch64-linux-gnu",
        "/usr/lib64",
#    endif
        "/usr/lib",
        "/lib"};
    for (u32 i = 0; i < sizeof(dirs) / sizeof(dirs[0]); ++i) {
        if (path_exists(path_join(arena, dirs[i], "crti.o"))) {
            return dirs[i];
        }
    }
    return NULL;
}

internal cstr back_end_linux_loader(void)
{
    cstr configured = getenv("NERD_DYNAMIC_LINKER");
    if (configured && configured[0]) {
        return configured;
    }
    static cstr paths[] = {
#    if ARCH_X86_64
        "/lib64/ld-linux-x86-64.so.2",
        "/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2",
        "/lib/ld-musl-x86_64.so.1",
#    elif ARCH_ARM64
        "/lib/ld-linux-aarch64.so.1",
        "/lib/aarch64-linux-gnu/ld-linux-aarch64.so.1",
        "/lib/ld-musl-aarch64.so.1",
#    endif
        NULL};
    for (u32 i = 0; paths[i]; ++i) {
        if (path_exists(paths[i])) {
            return paths[i];
        }
    }
    return NULL;
}
#endif

internal bool back_end_link_native(Arena*                    arena,
                                   const ProgramInfo*        program,
                                   const NerdArtifactConfig* artifacts,
                                   cstr                      ir,
                                   cstr                      runtime,
                                   bool                      shared)
{
    cstr object = back_end_cstr(
        arena, string_format(arena, "%s.obj.o", artifacts->binary_path));
    nerd_side_file_register_cleanup(artifacts->side_files, object);
    if (!back_end_compile_object(arena, artifacts, ir, object)) {
        return false;
    }
    StringBuilder command = {0};
#if OS_WINDOWS
    StringBuilder libraries = {0};
    sb_init(&libraries, arena);
    back_end_append_hir_extern_link_flags(&libraries, program, " ");
    string flags = sb_to_string(&libraries);
    sb_init(&command, arena);
    sb_format(&command,
              "lld-link /nologo /out:\"%s\" %s %s /subsystem:%s ",
              artifacts->binary_path,
              shared ? "/dll" : "",
              artifacts->release ? "" : "/debug:dwarf",
              program->windowed ? "windows" : "console");
    sb_format(&command,
              "\"%s\" \"%s\" /defaultlib:libcmt /defaultlib:libvcruntime "
              "/defaultlib:libucrt /defaultlib:oldnames /defaultlib:kernel32",
              object,
              runtime);
    // lld-link reads SDK/VC library paths from the developer environment's LIB.
    // Translate the existing deduplicated library list to COFF import names.
    for (usize i = 0; i + 3 <= flags.count;) {
        if (flags.data[i] != ' ' || flags.data[i + 1] != '-' ||
            flags.data[i + 2] != 'l') {
            ++i;
            continue;
        }
        usize start = i + 3;
        i           = start;
        while (i < flags.count && flags.data[i] != ' ') {
            ++i;
        }
        sb_format(
            &command, " \"%.*s.lib\"", (int)(i - start), flags.data + start);
    }
#elif OS_LINUX
    cstr crt = back_end_linux_crt_dir(arena);
    if (!crt) {
        return error_runtime("Cannot find native CRT objects; install the libc "
                             "development files or set NERD_CRT_DIR");
    }
    cstr start = path_join(arena, crt, "Scrt1.o");
    cstr init  = path_join(arena, crt, "crti.o");
    cstr fini  = path_join(arena, crt, "crtn.o");
    sb_init(&command, arena);
    sb_format(&command,
              "ld.lld %s -o \"%s\" -L\"%s\" -L/usr/lib -L/lib",
              shared ? "-shared" : "-pie",
              artifacts->binary_path,
              crt);
#    if ARCH_X86_64
    sb_append_cstr(&command,
                   " -L/usr/lib/x86_64-linux-gnu -L/lib/x86_64-linux-gnu "
                   "-L/usr/lib64 -L/lib64");
#    elif ARCH_ARM64
    sb_append_cstr(&command,
                   " -L/usr/lib/aarch64-linux-gnu -L/lib/aarch64-linux-gnu "
                   "-L/usr/lib64 -L/lib64");
#    endif
    if (!shared) {
        cstr loader = back_end_linux_loader();
        if (!loader) {
            return error_runtime(
                "Cannot find native dynamic loader; set NERD_DYNAMIC_LINKER");
        }
        sb_format(&command, " --dynamic-linker \"%s\" \"%s\"", loader, start);
    }
    sb_format(&command, " \"%s\" \"%s\" \"%s\"", init, object, runtime);
    back_end_append_hir_extern_link_flags(&command, program, " ");
    sb_format(&command, " -lc -lm -l:libgcc_s.so.1 \"%s\"", fini);
#elif OS_MACOS
    ShellResult sdk =
        shell_capture("xcrun --sdk macosx --show-sdk-path", arena);
    if (sdk.exit_code != 0) {
        return error_runtime("Cannot locate macOS SDK with xcrun");
    }
    string sdk_path = sdk.stdout_text;
    while (sdk_path.count && (sdk_path.data[sdk_path.count - 1] == '\n' ||
                              sdk_path.data[sdk_path.count - 1] == '\r')) {
        --sdk_path.count;
    }
    sb_init(&command, arena);
    sb_format(&command,
              "ld64.lld -arch %s -platform_version macos 11.0 11.0 -syslibroot "
              "\"" STRINGP "\" %s -o \"%s\" \"%s\" \"%s\" -lSystem",
              ARCH_ARM64 ? "arm64" : "x86_64",
              STRINGV(sdk_path),
              shared ? "-dylib" : "",
              artifacts->binary_path,
              object,
              runtime);
    back_end_append_hir_extern_link_flags(&command, program, " ");
#else
    return error_runtime("Direct LLVM linking is not configured for this host");
#endif
    if (!back_end_run_tool(arena, sb_to_string(&command), ir, runtime)) {
        return false;
    }
    path_remove(object);
#if OS_POSIX
    if (!shared && chmod(artifacts->binary_path, 0755) != 0) {
        return error_runtime("Failed to make %s executable",
                             artifacts->binary_path);
    }
#endif
    return true;
}

internal bool back_end_link_combined_llvm(Arena*                    arena,
                                          const ProgramInfo*        program,
                                          const NerdArtifactConfig* artifacts,
                                          cstr                      ir,
                                          cstr                      runtime)
{
    return back_end_link_native(arena, program, artifacts, ir, runtime, false);
}

internal bool back_end_link_shared_library(Arena*                    arena,
                                           const ProgramInfo*        program,
                                           const NerdArtifactConfig* artifacts,
                                           cstr                      ir,
                                           cstr                      runtime)
{
    return back_end_link_native(arena, program, artifacts, ir, runtime, true);
}

internal bool back_end_archive_static_library(Arena* arena,
                                              cstr   library_path,
                                              cstr   object_path,
                                              cstr   runtime_object_path)
{
#if OS_WINDOWS
    string command = string_format(arena,
                                   "llvm-lib /NOLOGO /OUT:\"%s\" \"%s\" \"%s\"",
                                   library_path,
                                   object_path,
                                   runtime_object_path);
#else
    string command = string_format(arena,
                                   "llvm-ar rcs \"%s\" \"%s\" \"%s\"",
                                   library_path,
                                   object_path,
                                   runtime_object_path);
#endif
    return back_end_run_tool(
        arena, command, "(object archive)", runtime_object_path);
}

// Probe versions first so doctor reports every missing tool in one invocation.
// Then exercise the actual release pipeline and host CRT in an isolated
// directory.
bool back_end_doctor(void)
{
    Arena arena = {0};
    arena_init(&arena);
    bool        ok      = true;
    static cstr tools[] = {
        "opt",
        "llc",
#if OS_WINDOWS
        "lld-link",
        "llvm-lib",
#elif OS_MACOS
        "ld64.lld",
        "llvm-ar",
#else
        "ld.lld",
        "llvm-ar",
#endif
    };
    prn("Nerd binary toolchain");
    for (u32 i = 0; i < sizeof(tools) / sizeof(tools[0]); ++i) {
        cstr   option  = strcmp(tools[i], "llvm-lib") == 0 ? "/?" : "--version";
        string command = string_format(&arena, "%s %s", tools[i], option);
        ShellResult result =
            shell_capture(back_end_cstr(&arena, command), &arena);
        bool found = result.exit_code == 0;
        prn("[%s] %s%s",
            found ? "OK" : "ERROR",
            tools[i],
            found ? ""
                  : " — unavailable or failed; install LLVM with this tool on "
                    "PATH");
        ok = found && ok;
    }
#if OS_WINDOWS
    cstr libraries = getenv("LIB");
    bool sdk_ok    = libraries && libraries[0];
    prn("[%s] Windows SDK / VC runtime libraries%s",
        sdk_ok ? "OK" : "ERROR",
        sdk_ok ? " (LIB configured; link probe will validate)"
               : " — run in a Visual Studio developer environment");
    ok = sdk_ok && ok;
#elif OS_LINUX
    cstr crt    = back_end_linux_crt_dir(&arena);
    bool crt_ok = crt && path_exists(path_join(&arena, crt, "Scrt1.o")) &&
                  path_exists(path_join(&arena, crt, "crtn.o"));
    prn("[%s] libc startup objects%s",
        crt_ok ? "OK" : "ERROR",
        crt_ok ? "" : " — install libc development files or set NERD_CRT_DIR");
    bool loader_ok =
        back_end_linux_loader() && path_exists(back_end_linux_loader());
    prn("[%s] dynamic loader%s",
        loader_ok ? "OK" : "ERROR",
        loader_ok ? "" : " — set NERD_DYNAMIC_LINKER to the host loader");
    ok = crt_ok && loader_ok && ok;
#elif OS_MACOS
    ShellResult sdk =
        shell_capture("xcrun --sdk macosx --show-sdk-path", &arena);
    prn("[%s] macOS SDK%s",
        sdk.exit_code == 0 ? "OK" : "ERROR",
        sdk.exit_code == 0 ? "" : " — install the Apple command-line SDK");
    ok = sdk.exit_code == 0 && ok;
#endif
    if (ok) {
        char directory[4096] = {0};
#if OS_WINDOWS
        char temp[4096] = {0};
        if (!GetTempPathA(sizeof(temp), temp) ||
            !GetTempFileNameA(temp, "nrd", 0, directory)) {
            ok = false;
        } else {
            DeleteFileA(directory);
            ok = CreateDirectoryA(directory, NULL) != 0;
        }
#else
        cstr temp  = getenv("TMPDIR");
        int  count = snprintf(directory,
                              sizeof(directory),
                              "%s/nerd-doctor-XXXXXX",
                              temp && temp[0] ? temp : "/tmp");
        ok         = count > 0 && (usize)count < sizeof(directory) &&
                     mkdtemp(directory) != NULL;
#endif
        if (!ok) {
            prn("[ERROR] Cannot create a temporary directory for the link "
                "probe");
        } else {
            NerdSideFileRegistry files = {0};
            nerd_side_file_registry_init(&files);
            cstr ir      = path_join(&arena, directory, "probe.ll");
            cstr runtime = path_join(&arena, directory, "probe.nrt.o");
            cstr binary  = path_join(&arena, directory, "probe.exe");
            nerd_side_file_register_cleanup(&files, ir);
            nerd_side_file_register_cleanup(&files, runtime);
            nerd_side_file_register_cleanup(&files, binary);
            NerdArtifactConfig artifacts = {
                .binary_path = binary,
                .release     = true,
                .side_files  = &files,
                .output_kind = NERD_BUILD_OUTPUT_Executable,
            };
            ProgramInfo program = {0};
            ok                  = back_end_write_text_file(
                                      ir, s("define i32 @main() { ret i32 0 }\n")) &&
                                  back_end_llvm_runtime_write_pic_object(runtime) &&
                                  back_end_link_native(
                                      &arena, &program, &artifacts, ir, runtime, false);
            if (ok) {
                string      command = string_format(&arena, "\"%s\"", binary);
                ShellResult result =
                    shell_capture(back_end_cstr(&arena, command), &arena);
                ok = result.exit_code == 0;
            }
            prn("[%s] LLVM optimisation, object generation, host runtime link "
                "and execution",
                ok ? "OK" : "ERROR");
            nerd_side_file_cleanup_registered(&files);
            nerd_side_file_registry_done(&files);
#if OS_WINDOWS
            RemoveDirectoryA(directory);
#else
            rmdir(directory);
#endif
        }
    }
    prn("%s",
        ok ? "Toolchain ready. C emission does not require these tools."
           : "Toolchain incomplete. Fix the errors above and run nerd doctor "
             "again.");
    arena_done(&arena);
    return ok;
}

// Aggregate C calling conventions are not yet modelled by Nerd's backend.
// Pointer parameters may refer to opaque data; callable values must themselves
// have signatures that the host C ABI can represent.
internal bool back_end_c_export_type(const Sema* sema, u32 index, u32 depth)
{
    if (index >= array_count(sema->types) || depth > 64) {
        return false;
    }
    const SemaType* type = &sema->types[index];
    switch (type->kind) {
    case STK_Void:
    case STK_Bool:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_Isize:
    case STK_Usize:
    case STK_F32:
    case STK_F64:
        return true;
    case STK_Pointer:
        if (type->first_param_type >= array_count(sema->types)) {
            return false;
        }
        return sema->types[type->first_param_type].kind != STK_Function ||
               back_end_c_export_type(sema, type->first_param_type, depth + 1);
    case STK_Function:
        if (!back_end_c_export_type(sema, type->return_type, depth + 1)) {
            return false;
        }
        for (u32 i = 0; i < type->param_count; ++i) {
            if (!back_end_c_export_type(
                    sema,
                    sema->type_param_types[type->first_param_type + i],
                    depth + 1)) {
                return false;
            }
        }
        return true;
    default:
        return false;
    }
}

internal bool back_end_validate_c_exports(const ProgramInfo* program)
{
    const FrontEndState* front =
        &program->modules[program->root_module_index].front_end;
    const Sema*       sema   = &front->sema;
    const ModuleInfo* module = &program->modules[program->root_module_index];
    for (u32 i = 0; i < array_count(module->export_decl_indices); ++i) {
        u32 index = module->export_decl_indices[i];
        if (index >= array_count(sema->decls)) {
            continue;
        }
        const SemaDecl* decl = &sema->decls[index];
        string          name = lex_symbol(&front->lexer, decl->symbol_handle);
        if (decl->kind == SK_GenericFunction ||
            decl->kind == SK_CompoundFunction) {
            return error_runtime(
                "C export `%.*s` needs one concrete function signature; export "
                "a concrete function binding instead",
                (int)name.count,
                name.data);
        }
        if (decl->type_index >= array_count(sema->types) ||
            sema->types[decl->type_index].kind != STK_Function) {
            continue;
        }
        const HirBinding* exported_binding = NULL;
        for (u32 b = 0; b < array_count(front->hir.bindings); ++b) {
            if (front->hir.bindings[b].symbol_handle == decl->symbol_handle) {
                exported_binding = &front->hir.bindings[b];
            }
        }
        if (exported_binding == NULL ||
            (exported_binding->kind != HIR_BINDING_Function &&
             exported_binding->kind != HIR_BINDING_Import)) {
            return error_runtime("C export `%.*s` needs a concrete function "
                                 "declaration or function binding",
                                 (int)name.count,
                                 name.data);
        }
        bool foreign =
            exported_binding->kind == HIR_BINDING_Function
                ? front->hir.functions[exported_binding->target_index].kind ==
                      HIR_FUNCTION_Ffi
                : front->hir.imports[exported_binding->target_index]
                          .ffi_symbol_handle != U32_MAX;
        if (foreign &&
            (sema->types[decl->type_index].flags & STF_FunctionVarargs)) {
            return error_runtime(
                "C export `%.*s` cannot forward an imported variadic tail; "
                "write a receiving Nerd function and use a va_list API",
                (int)name.count,
                name.data);
        }
        for (u32 m = 0; m < array_count(program->modules); ++m) {
            const FrontEndState* other = &program->modules[m].front_end;
            for (u32 e = 0; e < array_count(other->hir.externs); ++e) {
                if (string_eq(
                        name,
                        lex_symbol(&other->lexer,
                                   other->hir.externs[e].symbol_handle))) {
                    return error_runtime(
                        "C export `%.*s` collides with an imported foreign "
                        "symbol; choose a distinct public binding name",
                        (int)name.count,
                        name.data);
                }
            }
        }
        if (!back_end_c_export_type(sema, decl->type_index, 0)) {
            return error_runtime(
                "C export `%.*s` has an unsupported ABI signature; use scalar "
                "values, pointers, and compatible function pointers",
                (int)name.count,
                name.data);
        }
        if (name.count >= 4 && memcmp(name.data, "nrt_", 4) == 0) {
            return error_runtime(
                "C export `%.*s` uses the reserved nrt_ runtime namespace",
                (int)name.count,
                name.data);
        }
    }
    return true;
}

internal bool back_end_emit_llvm_artifacts(const ProgramInfo*        program,
                                           const NerdArtifactConfig* artifacts,
                                           Timing*                   timing)
{
    bool emit_binary = artifacts->output_kind != NERD_BUILD_OUTPUT_Executable ||
                       artifacts->require_entry_point;
    if (!emit_binary && !artifacts->emit_llvm_file) {
        return true;
    }
    if (program->root_module_index >= array_count(program->modules)) {
        return false;
    }

    if (artifacts->output_kind != NERD_BUILD_OUTPUT_Executable &&
        !back_end_validate_c_exports(program)) {
        return false;
    }

    Arena arena = {0};
    arena_init(&arena);

    BackEndLlvmModules modules       = {0};
    MemoryStats        memory_before = compiler_memory_profile_begin();
    TimePoint          timing_start  = back_end_timing_begin(timing);
    if (!back_end_render_llvm_modules(&arena, program, artifacts, &modules)) {
        back_end_timing_end(timing, COMPILER_PHASE_LLVM_RENDER, timing_start);
        compiler_memory_profile_end(
            COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_RENDER, memory_before);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    back_end_timing_end(timing, COMPILER_PHASE_LLVM_RENDER, timing_start);
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_RENDER, memory_before);
    if (!emit_binary) {
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return true;
    }

    cstr combined_llvm_path = back_end_cstr(
        &arena, string_format(&arena, "%s.link.ll", artifacts->binary_path));
    cstr runtime_object_path =
        artifacts->output_kind == NERD_BUILD_OUTPUT_Object
            ? NULL
            : back_end_cstr(
                  &arena,
                  string_format(&arena, "%s.nrt.o", artifacts->binary_path));
    const char* keep_combined_llvm = getenv("NERD_DEBUG_KEEP_LINK_LLVM");
    if (keep_combined_llvm == NULL || strcmp(keep_combined_llvm, "1") != 0) {
        nerd_side_file_register_cleanup(artifacts->side_files,
                                        combined_llvm_path);
    }
    nerd_side_file_register_cleanup(artifacts->side_files, runtime_object_path);

    string runtime_epilogue = s("");
    string init_ll          = s("");
    if (artifacts->output_kind == NERD_BUILD_OUTPUT_Executable) {
        const FrontEndState* root =
            &program->modules[program->root_module_index].front_end;
        BackEndRootMainInfo main_info =
            back_end_llvm_runtime_root_main_info(root);
        BackEndCoreLifecycleInfo core_lifecycle =
            back_end_llvm_runtime_core_lifecycle_info(program);
        runtime_epilogue = back_end_llvm_runtime_epilogue(
            &arena, main_info, core_lifecycle, program->windowed && OS_WINDOWS);
        init_ll = back_end_llvm_runtime_render_init(
            &arena, modules.init_module_indices);
    }
    memory_before             = compiler_memory_profile_begin();
    timing_start              = back_end_timing_begin(timing);
    TimingProbe combine_probe = timing_probe_begin();
    string      combined_llvm = back_end_llvm_text_build_combined(
        &arena, modules.module_llvms, runtime_epilogue, init_ll);
    timing_probe_end(combine_probe,
                     COMPILER_STAGE_BACK_END,
                     COMPILER_PHASE_LLVM_COMBINE,
                     (string){0},
                     true,
                     combined_llvm.count);
    back_end_timing_end(timing, COMPILER_PHASE_LLVM_COMBINE, timing_start);
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_COMBINE, memory_before);
    // Combining copies all module text. Release task results before invoking
    // LLVM tools; no borrowed module views may survive this point.
    back_end_llvm_render_results_done(&modules);
    timing_start = back_end_timing_begin(timing);
    if (!back_end_write_text_file(combined_llvm_path, combined_llvm)) {
        back_end_timing_end(timing, COMPILER_PHASE_LLVM_WRITE, timing_start);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    back_end_timing_end(timing, COMPILER_PHASE_LLVM_WRITE, timing_start);
    if (runtime_object_path != NULL) {
        memory_before = compiler_memory_profile_begin();
        timing_start  = back_end_timing_begin(timing);
        bool wrote_runtime =
            (artifacts->output_kind == NERD_BUILD_OUTPUT_SharedLibrary ||
             artifacts->output_kind == NERD_BUILD_OUTPUT_Executable)
                ? back_end_llvm_runtime_write_pic_object(runtime_object_path)
                : back_end_llvm_runtime_write_object(runtime_object_path);
        if (!wrote_runtime) {
            back_end_timing_end(
                timing, COMPILER_PHASE_RUNTIME_OBJECT, timing_start);
            compiler_memory_profile_end(COMPILER_STAGE_BACK_END,
                                        COMPILER_PHASE_RUNTIME_OBJECT,
                                        memory_before);
            back_end_llvm_modules_done(&modules);
            arena_done(&arena);
            return false;
        }
        back_end_timing_end(
            timing, COMPILER_PHASE_RUNTIME_OBJECT, timing_start);
        compiler_memory_profile_end(COMPILER_STAGE_BACK_END,
                                    COMPILER_PHASE_RUNTIME_OBJECT,
                                    memory_before);
    }

    memory_before    = compiler_memory_profile_begin();
    timing_start     = back_end_timing_begin(timing);
    bool ok          = false;
    cstr object_path = artifacts->binary_path;
    cstr phase       = COMPILER_PHASE_LINK;
    if (artifacts->output_kind == NERD_BUILD_OUTPUT_Executable) {
        ok = back_end_link_combined_llvm(&arena,
                                         program,
                                         artifacts,
                                         combined_llvm_path,
                                         runtime_object_path);
    } else if (artifacts->output_kind == NERD_BUILD_OUTPUT_Object) {
        phase = COMPILER_PHASE_OBJECT;
        ok    = back_end_compile_object(
            &arena, artifacts, combined_llvm_path, artifacts->binary_path);
    } else if (artifacts->output_kind == NERD_BUILD_OUTPUT_StaticLibrary) {
        phase       = COMPILER_PHASE_ARCHIVE;
        object_path = back_end_cstr(
            &arena, string_format(&arena, "%s.obj.o", artifacts->binary_path));
        nerd_side_file_register_cleanup(artifacts->side_files, object_path);
        ok = back_end_compile_object(
                 &arena, artifacts, combined_llvm_path, object_path) &&
             back_end_archive_static_library(&arena,
                                             artifacts->binary_path,
                                             object_path,
                                             runtime_object_path);
    } else if (artifacts->output_kind == NERD_BUILD_OUTPUT_SharedLibrary) {
        phase = COMPILER_PHASE_SHARED_LIBRARY;
        ok    = back_end_link_shared_library(&arena,
                                             program,
                                             artifacts,
                                             combined_llvm_path,
                                             runtime_object_path);
    }
    if (!ok) {
        back_end_timing_end(timing, phase, timing_start);
        compiler_memory_profile_end(
            COMPILER_STAGE_BACK_END, phase, memory_before);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    back_end_timing_end(timing, phase, timing_start);
    compiler_memory_profile_end(COMPILER_STAGE_BACK_END, phase, memory_before);

    bool keep_combined_llvm_file =
        keep_combined_llvm != NULL && strcmp(keep_combined_llvm, "1") == 0;
    back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                    !artifacts->emit_llvm_file,
                                    !artifacts->emit_llvm_file &&
                                        !keep_combined_llvm_file,
                                    combined_llvm_path,
                                    runtime_object_path);

    back_end_llvm_modules_done(&modules);
    arena_done(&arena);
    return true;
}

// Keep stdout suitable for command substitution: space-separated arguments on
// one line.
internal bool back_end_print_c_options(const ProgramInfo*        program,
                                       const NerdArtifactConfig* artifacts)
{
    Arena arena = {0};
    arena_init(&arena);
    StringBuilder flags = {0};
    sb_init(&flags, &arena);
    sb_append_cstr(&flags, "-std=gnu11 ");
    sb_append_cstr(&flags, artifacts->release ? "-O2 -DNDEBUG" : "-g -O0");
    bool link = artifacts->output_kind == NERD_BUILD_OUTPUT_Executable ||
                artifacts->output_kind == NERD_BUILD_OUTPUT_SharedLibrary;
    if (!link) {
        sb_append_cstr(&flags, " -c");
    }
    if (artifacts->output_kind == NERD_BUILD_OUTPUT_SharedLibrary) {
#if OS_MACOS
        sb_append_cstr(&flags, " -dynamiclib");
#else
        sb_append_cstr(&flags, " -shared");
#endif
#if OS_POSIX
        sb_append_cstr(&flags, " -fPIC");
#endif
    }
#if OS_WINDOWS
    if (artifacts->output_kind == NERD_BUILD_OUTPUT_Executable &&
        program->windowed) {
        sb_append_cstr(&flags, " -Wl,/SUBSYSTEM:WINDOWS");
    }
#endif
    if (link) {
        back_end_append_hir_extern_link_flags(&flags, program, " ");
    }
    string output = sb_to_string(&flags);
    bool   ok = fwrite(output.data, 1, output.count, stdout) == output.count &&
                fputc('\n', stdout) != EOF;
    arena_done(&arena);
    return ok;
}

bool back_end_program(ProgramInfo*              program,
                      const NerdArtifactConfig* artifacts,
                      bool                      verbose,
                      Timing*                   timing)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    if ((artifacts->emit_c_file || artifacts->print_c_options) &&
        artifacts->output_kind != NERD_BUILD_OUTPUT_Executable &&
        !back_end_validate_c_exports(program)) {
        return false;
    }
    if (artifacts->print_c_options && !artifacts->emit_c_file) {
        return back_end_print_c_options(program, artifacts);
    }

    if (artifacts->emit_hir_file &&
        program->root_module_index < array_count(program->modules)) {
        const FrontEndState* root =
            &program->modules[program->root_module_index].front_end;
        if (!hir_save(
                &root->hir, &root->lexer, &root->sema, artifacts->hir_path)) {
            return false;
        }
    }

    (void)verbose;
    if (artifacts->emit_c_file) {
        MemoryStats memory_before = compiler_memory_profile_begin();
        TimePoint   start         = back_end_timing_begin(timing);
        TimingProbe probe         = timing_probe_begin();
        bool        ok            = cgen_save_program(program, artifacts);
        timing_probe_end(probe,
                         COMPILER_STAGE_BACK_END,
                         COMPILER_PHASE_C_RENDER,
                         (string){0},
                         ok,
                         0);
        back_end_timing_end(timing, COMPILER_PHASE_C_RENDER, start);
        compiler_memory_profile_end(
            COMPILER_STAGE_BACK_END, COMPILER_PHASE_C_RENDER, memory_before);
        return ok && (!artifacts->print_c_options ||
                      back_end_print_c_options(program, artifacts));
    }
    Map function_names = {0};
    llvm_index_function_names(program, &function_names);
    program->llvm_function_name_counts = &function_names;
    bool ok = back_end_emit_llvm_artifacts(program, artifacts, timing);
    program->llvm_function_name_counts = NULL;
    map_done(&function_names);
    return ok;
}

bool back_end_llvm_tool_output_self_test(void)
{
    string clang_output = s(
        "example.link.ll:816:40: error: use of undefined value '%on.next.11'\n"
        "  816 |   br i1 %t30, label %on.body.10, label %on.next.11\n"
        "      |                                        ^\n"
        "1 error generated.\n");
    BackEndLlvmToolDiagnostic diagnostic = {0};
    if (!back_end_parse_llvm_tool_output(clang_output, &diagnostic) ||
        diagnostic.kind != BACK_END_LLVM_TOOL_DIAG_SOURCE_ERROR ||
        !string_eq_cstr(diagnostic.path, "example.link.ll") ||
        !string_eq_cstr(diagnostic.line, "816") ||
        !string_eq_cstr(diagnostic.column, "40") ||
        !string_eq_cstr(diagnostic.message,
                        "use of undefined value '%on.next.11'")) {
        eprn("Failed to parse clang LLVM source diagnostic");
        return false;
    }

    string lld_output =
        s("lld-link: error: undefined symbol: $GetStdHandle\n"
          ">>> referenced by\n"
          "C:\\Users\\matt\\AppData\\Local\\Temp\\quill.o:(m1.fn.5)\n"
          "clang: error: linker command failed with exit code 1\n");
    diagnostic = (BackEndLlvmToolDiagnostic){0};
    if (!back_end_parse_llvm_tool_output(lld_output, &diagnostic) ||
        diagnostic.kind != BACK_END_LLVM_TOOL_DIAG_UNDEFINED_SYMBOL ||
        !string_eq_cstr(diagnostic.symbol, "$GetStdHandle") ||
        !string_eq_cstr(diagnostic.reference,
                        "C:\\Users\\matt\\AppData\\Local\\Temp\\quill.o:"
                        "(m1.fn.5)")) {
        eprn("Failed to parse lld undefined-symbol diagnostic");
        return false;
    }

    string lld_missing_file_output =
        s("lld-link: error: could not open "
          "'nerd_missing_link_cleanup_test_library.lib': no such file or "
          "directory\n"
          "clang: error: linker command failed with exit code 1 "
          "(use -v to see invocation)\n");
    diagnostic = (BackEndLlvmToolDiagnostic){0};
    if (!back_end_parse_llvm_tool_output(lld_missing_file_output,
                                         &diagnostic) ||
        diagnostic.kind != BACK_END_LLVM_TOOL_DIAG_MISSING_FILE ||
        !string_eq_cstr(diagnostic.path,
                        "nerd_missing_link_cleanup_test_library.lib")) {
        eprn("Failed to parse lld missing-file diagnostic");
        return false;
    }

    string lld_locked_output =
        s("lld-link: error: could not open "
          "'C:\\Users\\matt\\nerd\\examples\\dungeon\\_dungeon.out.exe': "
          "Permission denied\n"
          "clang: error: linker command failed with exit code 1 "
          "(use -v to see invocation)\n");
    diagnostic = (BackEndLlvmToolDiagnostic){0};
    if (!back_end_parse_llvm_tool_output(lld_locked_output, &diagnostic) ||
        diagnostic.kind != BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT ||
        !string_eq_cstr(diagnostic.path,
                        "C:\\Users\\matt\\nerd\\examples\\dungeon\\_dungeon."
                        "out.exe")) {
        eprn("Failed to parse lld locked-output diagnostic");
        return false;
    }

    string lld_failed_write_output =
        s("lld-link: error: failed to write output "
          "'C:\\Users\\matt\\nerd\\examples\\matrix\\_matrix.out': "
          "permission denied\n"
          "clang: error: unable to remove file: permission denied\n"
          "clang: error: linker command failed with exit code 1 "
          "(use -v to see invocation)\n");
    diagnostic = (BackEndLlvmToolDiagnostic){0};
    if (!back_end_parse_llvm_tool_output(lld_failed_write_output,
                                         &diagnostic) ||
        diagnostic.kind != BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT ||
        !string_eq_cstr(diagnostic.path,
                        "C:\\Users\\matt\\nerd\\examples\\matrix\\_matrix."
                        "out")) {
        eprn("Failed to parse lld failed-write diagnostic");
        return false;
    }

    string clang_locked_output =
        s("clang: error: unable to open output file "
          "'C:\\Users\\matt\\nerd\\examples\\dungeon\\_dungeon.out.exe': "
          "Permission denied\n");
    diagnostic = (BackEndLlvmToolDiagnostic){0};
    if (!back_end_parse_llvm_tool_output(clang_locked_output, &diagnostic) ||
        diagnostic.kind != BACK_END_LLVM_TOOL_DIAG_LOCKED_OUTPUT ||
        !string_eq_cstr(diagnostic.path,
                        "C:\\Users\\matt\\nerd\\examples\\dungeon\\_dungeon."
                        "out.exe")) {
        eprn("Failed to parse clang locked-output diagnostic");
        return false;
    }

    prn("llvm-tool-output ok");
    return true;
}

//------------------------------------------------------------------------------

bool back_end_llvm_result_lifetime_self_test(void)
{
    BackEndLlvmModules modules = {0};
    for (u32 i = 0; i < 5; ++i) {
        array_push(modules.results, ((BackEndLlvmModuleResult){0}));
    }
    // Only two slots started: exercise the same cleanup as an early failure.
    for (u32 i = 0; i < 2; ++i) {
        BackEndLlvmModuleResult* result = &modules.results[i];
        arena_init(&result->arena);
        result->llvm = string_format(&result->arena, "declare i32 @f%u()\n", i);
        array_push(modules.module_llvms, result->llvm);
    }
    Arena combined_arena = {0};
    arena_init(&combined_arena);
    string combined = back_end_llvm_text_build_combined(
        &combined_arena, modules.module_llvms, (string){0}, (string){0});
    u8* expected = arena_alloc(&combined_arena, combined.count);
    memcpy(expected, combined.data, combined.count);
    MemoryStats before = mem_stats_snapshot();
    back_end_llvm_render_results_done(&modules);
    MemoryStats after = mem_stats_snapshot();
    bool ok = after.arena_done_count - before.arena_done_count == 2 &&
              array_count(modules.results) == 0 &&
              array_count(modules.module_llvms) == 0 && combined.count != 0 &&
              memcmp(expected, combined.data, combined.count) == 0;
    back_end_llvm_modules_done(&modules);
    ok = ok && mem_stats_snapshot().arena_done_count == after.arena_done_count;
    arena_done(&combined_arena);
    if (ok) {
        prn("llvm-result-lifetime ok");
    }
    return ok;
}
