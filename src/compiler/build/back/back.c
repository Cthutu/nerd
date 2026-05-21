//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>
#if OS_POSIX
#    include <sys/stat.h>
#endif
#include <stdio.h>

#include <compiler/build/back/back.h>
#include <compiler/build/back/llvm_runtime.h>
#include <compiler/build/back/llvm_text.h>
#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>

//------------------------------------------------------------------------------

typedef enum {
    BACK_END_LLVM_TOOL_DIAG_NONE,
    BACK_END_LLVM_TOOL_DIAG_SOURCE_ERROR,
    BACK_END_LLVM_TOOL_DIAG_UNDEFINED_SYMBOL,
    BACK_END_LLVM_TOOL_DIAG_MISSING_FILE,
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
                                                    const ProgramInfo* program)
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
                sb_format(link_flags, " -l" STRINGP, STRINGV(library));
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
        out->tool = s("clang");
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
    return false;
}

internal bool back_end_report_llvm_tool_failure(Arena*      arena,
                                                ShellResult result,
                                                string      command,
                                                cstr        combined_llvm_path,
                                                cstr        runtime_object_path)
{
    string output                        = back_end_tool_output_excerpt(result);

    BackEndLlvmToolDiagnostic diagnostic = {0};
    if (!back_end_parse_llvm_tool_output(output, &diagnostic)) {
        return error_ice(
            "Failed to parse LLVM tool output.\n"
            "The compiler fell back to the raw tool output so this format can "
            "be taught to the parser.\n"
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
        .binary_path     = "a.out",
        .hir_path        = "_a.hir",
        .llvm_path       = "_a.ll",
        .emit_hir_file   = false,
        .emit_llvm_file  = false,
        .emit_executable = true,
        .release         = false,
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
                                              cstr combined_llvm_path,
                                              cstr runtime_object_path)
{
    if (combined_llvm_path != NULL) {
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
    Array(cstr) llvm_paths;
    Array(string) module_llvms;
    Array(u32) init_module_indices;
} BackEndLlvmModules;

internal void back_end_llvm_modules_done(BackEndLlvmModules* modules)
{
    array_free(modules->llvm_paths);
    array_free(modules->module_llvms);
    array_free(modules->init_module_indices);
    *modules = (BackEndLlvmModules){0};
}

internal bool back_end_render_llvm_modules(Arena*                    arena,
                                           const ProgramInfo*        program,
                                           const NerdArtifactConfig* artifacts,
                                           BackEndLlvmModules*       out)
{
    u32 module_count = (u32)array_count(program->modules);
    array_requires_capacity(out->module_llvms, module_count);
    array_requires_capacity(out->init_module_indices, module_count);
    if (artifacts->emit_llvm_file) {
        array_requires_capacity(out->llvm_paths, module_count);
    }

    for (u32 i = 0; i < module_count; ++i) {
        const FrontEndState* front_end   = &program->modules[i].front_end;
        string               module_llvm = llvm_render_hir(
            &front_end->hir, &front_end->lexer, &front_end->sema, arena);
        array_push(out->module_llvms, module_llvm);
        if (artifacts->emit_llvm_file) {
            cstr llvm_path = back_end_module_llvm_path(arena, artifacts, i);
            if (!back_end_write_text_file(llvm_path, module_llvm)) {
                return false;
            }
            array_push(out->llvm_paths, llvm_path);
        }
        if (back_end_llvm_runtime_hir_has_globals(&front_end->hir)) {
            array_push(out->init_module_indices, i);
        }
    }
    return true;
}

internal bool back_end_link_combined_llvm(Arena*                    arena,
                                          const ProgramInfo*        program,
                                          const NerdArtifactConfig* artifacts,
                                          cstr combined_llvm_path,
                                          cstr runtime_object_path)
{
    string opt_flags       = artifacts->release ? s("-O2") : s("-g -O0");
    string subsystem_flags = s("");
#if OS_WINDOWS
    if (program->windowed) {
        subsystem_flags = s(" -Wl,/SUBSYSTEM:WINDOWS");
    }
#endif
    StringBuilder link_flags = {0};
    sb_init(&link_flags, arena);
    back_end_append_hir_extern_link_flags(&link_flags, program);
    StringBuilder command_builder = {0};
    sb_init(&command_builder, arena);
    sb_format(&command_builder,
              "clang -Wno-override-module " STRINGP " -o \"%s\" \"%s\" \"%s\"",
              STRINGV(opt_flags),
              artifacts->binary_path,
              combined_llvm_path,
              runtime_object_path);
    sb_append_string(&command_builder, subsystem_flags);
    sb_append_string(&command_builder, sb_to_string(&link_flags));
    string      command = sb_to_string(&command_builder);
    ShellResult result  = shell_capture(back_end_cstr(arena, command), arena);
    if (result.exit_code != 0) {
        return back_end_report_llvm_tool_failure(
            arena, result, command, combined_llvm_path, runtime_object_path);
    }

#if OS_POSIX
    if (chmod(artifacts->binary_path, 0755) != 0) {
        return error_runtime("Failed to make %s executable",
                             artifacts->binary_path);
    }
#endif

    return true;
}

internal bool back_end_emit_llvm_artifacts(const ProgramInfo*        program,
                                           const NerdArtifactConfig* artifacts,
                                           Timing*                   timing)
{
    if (!artifacts->emit_executable && !artifacts->emit_llvm_file) {
        return true;
    }
    if (program->root_module_index >= array_count(program->modules)) {
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
        back_end_cleanup_llvm_artifacts(modules.llvm_paths, false, NULL, NULL);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    back_end_timing_end(timing, COMPILER_PHASE_LLVM_RENDER, timing_start);
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_RENDER, memory_before);
    if (!artifacts->emit_executable) {
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return true;
    }

    cstr combined_llvm_path = back_end_cstr(
        &arena, string_format(&arena, "%s.link.ll", artifacts->binary_path));
    cstr runtime_object_path = back_end_cstr(
        &arena, string_format(&arena, "%s.nrt.o", artifacts->binary_path));

    const FrontEndState* root =
        &program->modules[program->root_module_index].front_end;
    bool root_main_returns_void =
        back_end_llvm_runtime_root_main_returns_void(root);
    string runtime_epilogue = back_end_llvm_runtime_epilogue(
        root_main_returns_void, program->windowed && OS_WINDOWS);
    string init_ll =
        back_end_llvm_runtime_render_init(&arena, modules.init_module_indices);
    memory_before        = compiler_memory_profile_begin();
    timing_start         = back_end_timing_begin(timing);
    string combined_llvm = back_end_llvm_text_build_combined(
        &arena, modules.module_llvms, runtime_epilogue, init_ll);
    back_end_timing_end(timing, COMPILER_PHASE_LLVM_COMBINE, timing_start);
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LLVM_COMBINE, memory_before);
    timing_start = back_end_timing_begin(timing);
    if (!back_end_write_text_file(combined_llvm_path, combined_llvm)) {
        back_end_timing_end(timing, COMPILER_PHASE_LLVM_WRITE, timing_start);
        back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                        !artifacts->emit_llvm_file,
                                        combined_llvm_path,
                                        NULL);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    back_end_timing_end(timing, COMPILER_PHASE_LLVM_WRITE, timing_start);
    memory_before = compiler_memory_profile_begin();
    timing_start  = back_end_timing_begin(timing);
    if (!back_end_llvm_runtime_write_object(runtime_object_path)) {
        back_end_timing_end(
            timing, COMPILER_PHASE_RUNTIME_OBJECT, timing_start);
        compiler_memory_profile_end(COMPILER_STAGE_BACK_END,
                                    COMPILER_PHASE_RUNTIME_OBJECT,
                                    memory_before);
        back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                        !artifacts->emit_llvm_file,
                                        combined_llvm_path,
                                        runtime_object_path);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    back_end_timing_end(timing, COMPILER_PHASE_RUNTIME_OBJECT, timing_start);
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_RUNTIME_OBJECT, memory_before);

    memory_before = compiler_memory_profile_begin();
    timing_start  = back_end_timing_begin(timing);
    if (!back_end_link_combined_llvm(&arena,
                                     program,
                                     artifacts,
                                     combined_llvm_path,
                                     runtime_object_path)) {
        back_end_timing_end(timing, COMPILER_PHASE_LINK, timing_start);
        compiler_memory_profile_end(
            COMPILER_STAGE_BACK_END, COMPILER_PHASE_LINK, memory_before);
        back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                        !artifacts->emit_llvm_file,
                                        combined_llvm_path,
                                        runtime_object_path);
        back_end_llvm_modules_done(&modules);
        arena_done(&arena);
        return false;
    }
    back_end_timing_end(timing, COMPILER_PHASE_LINK, timing_start);
    compiler_memory_profile_end(
        COMPILER_STAGE_BACK_END, COMPILER_PHASE_LINK, memory_before);

    back_end_cleanup_llvm_artifacts(modules.llvm_paths,
                                    !artifacts->emit_llvm_file,
                                    combined_llvm_path,
                                    runtime_object_path);

    back_end_llvm_modules_done(&modules);
    arena_done(&arena);
    return true;
}

bool back_end_program(const ProgramInfo*        program,
                      const NerdArtifactConfig* artifacts,
                      bool                      verbose,
                      Timing*                   timing)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
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
    return back_end_emit_llvm_artifacts(program, artifacts, timing);
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

    prn("llvm-tool-output ok");
    return true;
}

//------------------------------------------------------------------------------
