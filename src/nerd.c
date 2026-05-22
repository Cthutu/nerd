//------------------------------------------------------------------------------
//> use: core intern compiler timing table cli lsp object
// clang-format off
//> run: clang -std=c23 -Wall -Wextra -Werror -O2 -c data/nrt.c -o _obj/runtime/nrt.o
// clang-format on

#include <cli/cli.h>
#include <compiler/build/back/back.h>
#include <compiler/build/back/llvm_text.h>
#include <compiler/build/front/front.h>
#include <compiler/compiler.h>
#include <compiler/error/error.h>
#include <lsp/lsp.h>
#include <table/table.h>

#include <errno.h>
#include <stdio.h>

#if OS_WINDOWS
#    include <direct.h>
#else
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif

//------------------------------------------------------------------------------

internal JsonValue* nerd_cli_make_param(Arena* arena,
                                        cstr   name,
                                        cstr   kind,
                                        cstr   long_name,
                                        cstr   short_name,
                                        cstr   description,
                                        bool   required)
{
    JsonValue* param = json_new_object(arena);
    json_object_set_cstr(param, arena, "name", name);
    json_object_set_cstr(param, arena, "kind", kind);
    if (long_name) {
        json_object_set_cstr(param, arena, "long", long_name);
    }
    if (short_name) {
        json_object_set_cstr(param, arena, "short", short_name);
    }
    json_object_set_cstr(param, arena, "description", description);
    json_object_set_bool(param, arena, "required", required);
    return param;
}

internal JsonValue* nerd_cli_make_flag(Arena* arena,
                                       cstr   long_name,
                                       cstr   short_name,
                                       cstr   description)
{
    JsonValue* flag = json_new_object(arena);
    json_object_set_cstr(flag, arena, "long", long_name);
    if (short_name) {
        json_object_set_cstr(flag, arena, "short", short_name);
    }
    json_object_set_cstr(flag, arena, "description", description);
    return flag;
}

internal JsonValue* nerd_cli_make_command(
    Arena* arena, cstr name, cstr summary, JsonValue* flags, JsonValue* params)
{
    JsonValue* command = json_new_object(arena);
    json_object_set_cstr(command, arena, "name", name);
    json_object_set_cstr(command, arena, "summary", summary);
    if (flags) {
        json_object_set_array(command, "flags", flags);
    }
    if (params) {
        json_object_set_array(command, "params", params);
    }
    return command;
}

internal string nerd_cli_param_string(const JsonValue* cli_result,
                                      cstr             path,
                                      string           fallback);

internal bool nerd_write_test_file(cstr path, cstr text)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        return false;
    }
    usize text_len = strlen(text);
    usize written  = fwrite(text, 1, text_len, file);
    bool  close_ok = fclose(file) == 0;
    return written == text_len && close_ok;
}

internal int nerd_internal_module_filemap_test(void)
{
    cstr module_path = "maplockmod.n";
    cstr root_path   = "_internal_maplock_root.n";
    cstr module_text = "pub imported_value :: 0\n";
    cstr root_text   = "maplockmod :: use maplockmod\n"
                       "\n"
                       "main :: fn () -> i32 {\n"
                       "    return maplockmod.imported_value\n"
                       "}\n";

    remove(module_path);
    remove(root_path);
    if (!nerd_write_test_file(module_path, module_text) ||
        !nerd_write_test_file(root_path, root_text)) {
        remove(module_path);
        remove(root_path);
        eprn("Failed to write internal filemap test inputs");
        return 1;
    }

    ProgramInfo     program = {0};
    FrontEndOptions options = {
        .require_entry_point = true,
        .skip_hir_generation = true,
    };
    bool ok = front_end_program(
        (NerdSource){
            .source      = s(root_text),
            .source_path = s(root_path),
        },
        &options,
        NULL,
        &program);
    if (!ok) {
        program_info_done(&program);
        remove(module_path);
        remove(root_path);
        eprn("Failed to analyse internal filemap test program");
        return 1;
    }

    bool overwrite_ok =
        nerd_write_test_file(module_path, "pub imported_value :: 1\n");
    program_info_done(&program);
    remove(module_path);
    remove(root_path);
    if (!overwrite_ok) {
        eprn("Imported module remained locked while ProgramInfo was alive");
        return 1;
    }
    return 0;
}

internal bool nerd_make_test_dir(cstr path)
{
#if OS_WINDOWS
    return _mkdir(path) == 0 || errno == EEXIST;
#else
    return mkdir(path, 0777) == 0 || errno == EEXIST;
#endif
}

internal bool nerd_remove_test_dir(cstr path)
{
#if OS_WINDOWS
    return _rmdir(path) == 0;
#else
    return rmdir(path) == 0;
#endif
}

internal int nerd_internal_module_cwd_root_test(void)
{
    cstr module_path = "_internal_cwdmod.n";
    cstr root_dir    = "_internal_cwd_root";
    cstr root_path   = "_internal_cwd_root/root.n";
    cstr module_text = "pub imported_value :: 0\n";
    cstr root_text   = "cwdmod :: use _internal_cwdmod\n"
                       "\n"
                       "main :: fn () -> i32 {\n"
                       "    return cwdmod.imported_value\n"
                       "}\n";

    remove(module_path);
    remove(root_path);
    nerd_remove_test_dir(root_dir);
    if (!nerd_make_test_dir(root_dir) ||
        !nerd_write_test_file(module_path, module_text) ||
        !nerd_write_test_file(root_path, root_text)) {
        remove(module_path);
        remove(root_path);
        nerd_remove_test_dir(root_dir);
        eprn("Failed to write internal cwd module root test inputs");
        return 1;
    }

    ProgramInfo     program = {0};
    FrontEndOptions options = {
        .require_entry_point = true,
        .skip_hir_generation = true,
    };
    bool ok = front_end_program(
        (NerdSource){
            .source      = s(root_text),
            .source_path = s(root_path),
        },
        &options,
        NULL,
        &program);
    program_info_done(&program);
    remove(module_path);
    remove(root_path);
    nerd_remove_test_dir(root_dir);
    if (!ok) {
        eprn("Failed to resolve module from current working directory");
        return 1;
    }
    return 0;
}

internal int nerd_internal_test(const JsonValue* cli_result)
{
    string name =
        nerd_cli_param_string(cli_result, "command.params.name", (string){0});
    if (string_eq_cstr(name, "llvm-text")) {
        return back_end_llvm_text_self_test() ? 0 : 1;
    }
    if (string_eq_cstr(name, "llvm-tool-output")) {
        return back_end_llvm_tool_output_self_test() ? 0 : 1;
    }
    if (string_eq_cstr(name, "module-filemap-release")) {
        return nerd_internal_module_filemap_test();
    }
    if (string_eq_cstr(name, "module-cwd-root")) {
        return nerd_internal_module_cwd_root_test();
    }

    eprn("Unknown internal test: " STRINGP, STRINGV(name));
    return 1;
}

internal JsonValue* nerd_cli_schema(Arena* arena)
{
    JsonValue* schema   = json_new_object(arena);
    JsonValue* commands = json_new_array(arena);
    JsonValue* flags    = json_new_array(arena);

    // Schema used by the `nerd` executable:
    // {
    //   "program": "nerd",
    //   "summary": "Nerd compiler playground",
    //   "commands": [
    //     {
    //       "name": "build",
    //       "summary": "Run one normal build",
    //       "params": [
    //         {
    //           "name": "source",
    //           "kind": "positional",
    //           "description": "Source snippet to compile",
    //           "required": false
    //         }
    //       ]
    //     },
    //     {
    //       "name": "check",
    //       "summary": "Check one program without code generation"
    //     },
    //     {
    //       "name": "test",
    //       "summary": "Run the compiler test command"
    //     },
    //     {
    //       "name": "format",
    //       "summary": "Format one source file"
    //     },
    //     {
    //       "name": "run",
    //       "summary": "Build and run one program"
    //     },
    //     {
    //       "name": "lsp",
    //       "summary": "Run the LSP server"
    //     }
    //   ]
    // }

    json_object_set_cstr(schema, arena, "program", "nerd");
    json_object_set_cstr(schema, arena, "summary", "Nerd compiler playground");
    json_object_set_array(schema, "flags", flags);
    json_object_set_array(schema, "commands", commands);

    json_array_push(
        flags,
        nerd_cli_make_flag(
            arena, "verbose", "v", "Enable verbose debug dump output"));
    json_array_push(
        flags,
        nerd_cli_make_flag(
            arena, "timing", NULL, "Print compiler timing information"));

    {
        JsonValue* build_params = json_new_array(arena);
        JsonValue* build_flags  = json_new_array(arena);
        json_array_push(build_params,
                        nerd_cli_make_param(arena,
                                            "source",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Source snippet to compile",
                                            false));
        json_array_push(
            build_flags,
            nerd_cli_make_flag(
                arena, "hir", NULL, "Write generated HIR to a file"));
        json_array_push(
            build_flags,
            nerd_cli_make_flag(
                arena, "llvm", NULL, "Write generated LLVM IR to a file"));
        json_array_push(
            build_flags,
            nerd_cli_make_flag(arena, "release", "r", "Build release binary"));
        json_array_push(build_params,
                        nerd_cli_make_param(arena,
                                            "output",
                                            "named",
                                            "output",
                                            "o",
                                            "Output binary path",
                                            false));
        json_array_push(commands,
                        nerd_cli_make_command(arena,
                                              "build",
                                              "Run one normal build",
                                              build_flags,
                                              build_params));
        json_array_push(
            commands,
            nerd_cli_make_command(
                arena, "b", "Alias for build", build_flags, build_params));
    }
    {
        JsonValue* run_params = json_new_array(arena);
        JsonValue* run_flags  = json_new_array(arena);
        json_array_push(run_params,
                        nerd_cli_make_param(arena,
                                            "input",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Source file to build and run. "
                                            "Defaults to main.n, then "
                                            "<folder>.n",
                                            false));
        json_array_push(
            run_flags,
            nerd_cli_make_flag(
                arena, "hir", NULL, "Write generated HIR to a file"));
        json_array_push(
            run_flags,
            nerd_cli_make_flag(
                arena, "llvm", NULL, "Write generated LLVM IR to a file"));
        json_array_push(
            run_flags,
            nerd_cli_make_flag(arena, "release", "r", "Build release binary"));
        json_array_push(
            run_flags,
            nerd_cli_make_flag(arena,
                               "keep",
                               "k",
                               "Keep the generated executable after running"));
        json_array_push(run_params,
                        nerd_cli_make_param(arena,
                                            "output",
                                            "named",
                                            "output",
                                            "o",
                                            "Output binary path",
                                            false));
        json_array_push(commands,
                        nerd_cli_make_command(arena,
                                              "run",
                                              "Build and run one program",
                                              run_flags,
                                              run_params));
        json_array_push(
            commands,
            nerd_cli_make_command(
                arena, "r", "Alias for run", run_flags, run_params));
    }
    {
        JsonValue* check_params = json_new_array(arena);
        JsonValue* check_flags  = json_new_array(arena);
        json_array_push(
            check_flags,
            nerd_cli_make_flag(arena, "release", "r", "Check release build"));
        json_array_push(check_params,
                        nerd_cli_make_param(arena,
                                            "input",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Source file to check",
                                            true));
        json_array_push(commands,
                        nerd_cli_make_command(arena,
                                              "check",
                                              "Check one program without code "
                                              "generation",
                                              check_flags,
                                              check_params));
    }
    {
        JsonValue* flags  = json_new_array(arena);
        JsonValue* params = json_new_array(arena);
        json_array_push(
            flags,
            nerd_cli_make_flag(arena,
                               "list",
                               NULL,
                               "List discovered source tests without running"));
        json_array_push(
            flags,
            nerd_cli_make_flag(
                arena, "verbose", "v", "List source tests as they run"));
        json_array_push(params,
                        nerd_cli_make_param(arena,
                                            "input",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Root source file to test",
                                            true));
        json_array_push(
            params,
            nerd_cli_make_param(arena,
                                "filter",
                                "named",
                                "filter",
                                NULL,
                                "Run only test names containing text",
                                false));
        json_array_push(
            commands,
            nerd_cli_make_command(arena,
                                  "test",
                                  "Run source tests in a Nerd module",
                                  flags,
                                  params));
        json_array_push(
            commands,
            nerd_cli_make_command(arena, "t", "Alias for test", flags, params));
    }
    {
        JsonValue* flags  = json_new_array(arena);
        JsonValue* params = json_new_array(arena);
        json_array_push(
            flags,
            nerd_cli_make_flag(
                arena, "stdout", NULL, "Write formatted output to stdout"));
        json_array_push(params,
                        nerd_cli_make_param(arena,
                                            "input",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Source file to format",
                                            true));
        json_array_push(params,
                        nerd_cli_make_param(arena,
                                            "output",
                                            "named",
                                            "output",
                                            "o",
                                            "Formatted output path",
                                            false));
        json_array_push(
            commands,
            nerd_cli_make_command(
                arena, "format", "Format one source file", flags, params));
        json_array_push(commands,
                        nerd_cli_make_command(
                            arena, "f", "Alias for format", flags, params));
    }
    json_array_push(
        commands,
        nerd_cli_make_command(arena, "lsp", "Run the LSP server", NULL, NULL));
    {
        JsonValue* internal_test_params = json_new_array(arena);
        json_array_push(internal_test_params,
                        nerd_cli_make_param(arena,
                                            "name",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Internal test name",
                                            true));
        json_array_push(commands,
                        nerd_cli_make_command(arena,
                                              "internal-test",
                                              "Run one internal compiler test",
                                              NULL,
                                              internal_test_params));
    }

    return schema;
}

internal void nerd_print_args_table(int argc, char** argv)
{
    Table args_table           = {0};
    Array(TableColumn) columns = 0;
    array_push(columns,
               (TableColumn){.title = "Index", .colour = ANSI_CYAN},
               (TableColumn){.title = "Argument", .colour = ANSI_GREEN});
    table_init(&args_table, columns, .title = "Arguments");
    array_free(columns);

    for (int i = 1; i < argc; i++) {
        TableCell row[2] = {
            table_cell_i32(i),
            table_cell_string(s(argv[i])),
        };
        table_add_row(&args_table, row);
    }

    table_print(&args_table);
    table_done(&args_table);
}

internal isize nerd_find_command_index(const CliParser* parser,
                                       string           command_name)
{
    for (usize i = 0; i < array_count(parser->commands); i++) {
        if (string_eq(parser->commands[i].name, command_name)) {
            return (isize)i;
        }
    }
    return -1;
}

internal string nerd_cli_param_string(const JsonValue* cli_result,
                                      cstr             path,
                                      string           fallback)
{
    JsonValue* value = json_get_cstr(cli_result, path);
    if (!value) {
        return fallback;
    }

    ASSERT(value->kind == JSON_STRING,
           "Expected CLI parse result field '%s' to be a string",
           path);
    return json_string(value);
}

internal bool
nerd_cli_flag_bool(const JsonValue* cli_result, cstr path, bool fallback)
{
    JsonValue* value = json_get_cstr(cli_result, path);
    if (!value) {
        return fallback;
    }

    ASSERT(value->kind == JSON_BOOL,
           "Expected CLI parse result field '%s' to be a bool",
           path);
    return json_bool(value);
}

internal NerdBuildConfig
nerd_build_config_from_json(const JsonValue* cli_result, Array(string) keywords)
{
    Arena* source_arena = &temp_arena;
    string source_arg =
        nerd_cli_param_string(cli_result, "command.params.source", s("42"));
    string source_path     = {0};
    string source_text     = source_arg;

    char source_cstr[4096] = {0};
    ASSERT(source_arg.count < sizeof(source_cstr),
           "Source path too long for CLI handling");
    memcpy(source_cstr, source_arg.data, source_arg.count);
    source_cstr[source_arg.count] = '\0';

    if (path_exists(source_cstr) && !path_is_directory(source_cstr)) {
        FileMap map       = {0};
        string  file_text = filemap_load(source_cstr, &map);
        ASSERT(file_text.data != NULL,
               "Failed to load source file: %s",
               source_cstr);

        u8* copy = (u8*)arena_alloc(source_arena, file_text.count);
        memcpy(copy, file_text.data, file_text.count);
        source_text = string_from(copy, file_text.count);
        source_path = string_format(source_arena, "%s", source_cstr);
        filemap_unload(&map);
    }

    return (NerdBuildConfig){
        .source =
            (NerdSource){
                .source      = source_text,
                .source_path = source_path,
            },
        .output_path = nerd_cli_param_string(
            cli_result, "command.params.output", (string){0}),
        .emit_hir = nerd_cli_flag_bool(cli_result, "command.flags.hir", false),
        .emit_llvm =
            nerd_cli_flag_bool(cli_result, "command.flags.llvm", false),
        .release =
            nerd_cli_flag_bool(cli_result, "command.flags.release", false),
        .verbose =
            nerd_cli_flag_bool(cli_result, "global_flags.verbose", false),
        .timing = nerd_cli_flag_bool(cli_result, "global_flags.timing", false),
        .keywords = keywords,
    };
}

internal NerdCheckConfig
nerd_check_config_from_json(const JsonValue* cli_result, Array(string) keywords)
{
    return (NerdCheckConfig){
        .source =
            (NerdSource){
                .source_path = nerd_cli_param_string(
                    cli_result, "command.params.input", (string){0}),
            },
        .release =
            nerd_cli_flag_bool(cli_result, "command.flags.release", false),
        .verbose =
            nerd_cli_flag_bool(cli_result, "global_flags.verbose", false),
        .timing = nerd_cli_flag_bool(cli_result, "global_flags.timing", false),
        .keywords = keywords,
    };
}

internal NerdTestConfig nerd_test_config_from_json(const JsonValue* cli_result,
                                                   Array(string) keywords)
{
    return (NerdTestConfig){
        .input_path = nerd_cli_param_string(
            cli_result, "command.params.input", (string){0}),
        .filter = nerd_cli_param_string(
            cli_result, "command.params.filter", (string){0}),
        .list = nerd_cli_flag_bool(cli_result, "command.flags.list", false),
        .list_results =
            nerd_cli_flag_bool(cli_result, "command.flags.verbose", false),
        .verbose =
            nerd_cli_flag_bool(cli_result, "global_flags.verbose", false),
        .keywords = keywords,
    };
}

internal NerdFormatConfig
nerd_format_config_from_json(const JsonValue* cli_result)
{
    return (NerdFormatConfig){
        .input_path = nerd_cli_param_string(
            cli_result, "command.params.input", (string){0}),
        .output_path = nerd_cli_param_string(
            cli_result, "command.params.output", (string){0}),
        .write_stdout =
            nerd_cli_flag_bool(cli_result, "command.flags.stdout", false),
    };
}

internal NerdRunConfig nerd_run_config_from_json(const JsonValue* cli_result,
                                                 Array(string) keywords)
{
    Arena* arena = &temp_arena;
    string input =
        nerd_cli_param_string(cli_result, "command.params.input", (string){0});

    if (input.count == 0) {
        cstr cwd = path_canonical(arena, ".");
        if (cwd == NULL) {
            error_runtime("Failed to resolve current working directory");
        } else {
            cstr main_path = path_join(arena, cwd, "main.n");
            if (path_exists(main_path) && !path_is_directory(main_path)) {
                input = s(main_path);
            } else {
                string folder          = path_filename(s(cwd));

                StringBuilder filename = {0};
                sb_init(&filename, arena);
                sb_append_string(&filename, folder);
                sb_append_cstr(&filename, ".n");
                sb_append_null(&filename);

                cstr folder_path =
                    path_join(arena, cwd, (cstr)sb_to_string(&filename).data);
                if (path_exists(folder_path) &&
                    !path_is_directory(folder_path)) {
                    input = s(folder_path);
                } else {
                    error_runtime(
                        "No source file specified and neither `%s` nor `%s` "
                        "exists",
                        main_path,
                        folder_path);
                }
            }
        }
    }

    return (NerdRunConfig){
        .source =
            (NerdSource){
                .source_path = input,
            },
        .output_path = nerd_cli_param_string(
            cli_result, "command.params.output", (string){0}),
        .emit_hir = nerd_cli_flag_bool(cli_result, "command.flags.hir", false),
        .emit_llvm =
            nerd_cli_flag_bool(cli_result, "command.flags.llvm", false),
        .keep_binary =
            nerd_cli_flag_bool(cli_result, "command.flags.keep", false),
        .release =
            nerd_cli_flag_bool(cli_result, "command.flags.release", false),
        .verbose =
            nerd_cli_flag_bool(cli_result, "global_flags.verbose", false),
        .timing = nerd_cli_flag_bool(cli_result, "global_flags.timing", false),
        .keywords = keywords,
    };
}

internal bool nerd_cli_extract_keywords(Arena*  arena,
                                        int     argc,
                                        char**  argv,
                                        int*    out_argc,
                                        char*** out_argv,
                                        Array(string) * out_keywords)
{
    char** filtered =
        (char**)arena_alloc(arena, (usize)argc * sizeof(filtered[0]));
    int filtered_count = 0;

    for (int i = 0; i < argc; ++i) {
        if (i > 0 && strncmp(argv[i], "-k:", 3) == 0) {
            if (argv[i][3] == '\0') {
                eprn("Expected a keyword after -k:");
                return false;
            }
            array_push(*out_keywords, string_format(arena, "%s", argv[i] + 3));
            continue;
        }
        if (i > 0 && strncmp(argv[i], "-D", 2) == 0) {
            if (argv[i][2] == '\0') {
                eprn("Expected a keyword after -D");
                return false;
            }
            array_push(*out_keywords, string_format(arena, "%s", argv[i] + 2));
            continue;
        }
        filtered[filtered_count++] = argv[i];
    }

    *out_argc = filtered_count;
    *out_argv = filtered;
    return true;
}

internal int nerd_run_with_cli(int argc, char** argv)
{
    Arena arena = {0};
    arena_init(&arena);
    error_system_init(getenv("NERD_ERROR_RENDER_TEST") != NULL
                          ? ERROR_RENDER_TEST
                          : ERROR_RENDER_NORMAL);

    JsonValue* schema = nerd_cli_schema(&arena);
    CliParser  parser = {0};
    cli_init(&parser, schema);

    int    cli_argc            = argc;
    char** cli_argv            = argv;
    Array(string) cli_keywords = NULL;
    if (!nerd_cli_extract_keywords(
            &arena, argc, argv, &cli_argc, &cli_argv, &cli_keywords)) {
        cli_done(&parser);
        json_done(schema);
        arena_done(&arena);
        error_system_done();
        return 1;
    }

    JsonValue* cli_result = cli_parse(&parser, &arena, cli_argc, cli_argv);
    JsonValue* command    = json_object_get_cstr(cli_result, "command");
    JsonValue* help       = json_object_get_cstr(cli_result, "help_requested");
    JsonValue* ok         = json_object_get_cstr(cli_result, "ok");

    if (help && help->kind == JSON_BOOL && json_bool(help)) {
        if (command && command->kind == JSON_OBJECT) {
            JsonValue* command_name = json_object_get_cstr(command, "name");
            if (command_name && command_name->kind == JSON_STRING) {
                isize command_index =
                    nerd_find_command_index(&parser, json_string(command_name));
                if (command_index >= 0) {
                    cli_print_command_help(&parser, (usize)command_index);
                } else {
                    cli_print_help(&parser);
                }
            } else {
                cli_print_help(&parser);
            }
        } else {
            cli_print_help(&parser);
        }

        json_done(cli_result);
        cli_done(&parser);
        json_done(schema);
        arena_done(&arena);
        error_system_done();
        return 0;
    }

    if (!ok || ok->kind != JSON_BOOL || !json_bool(ok)) {
        JsonValue* error = json_object_get_cstr(cli_result, "error");
        cli_print_help(&parser);
        if (error && error->kind == JSON_STRING) {
            string error_message = json_string(error);
            eprn("%.*s", STRINGV(error_message));
            json_done(cli_result);
            cli_done(&parser);
            json_done(schema);
            arena_done(&arena);
            error_system_done();
            return 1;
        }

        if (argc <= 1) {
            dump_info();
        }

        json_done(cli_result);
        cli_done(&parser);
        json_done(schema);
        arena_done(&arena);
        error_system_done();
        eprn("CLI parse failed.");
        return 1;
    }

    ASSERT(command && command->kind == JSON_OBJECT,
           "CLI parse result must contain a command object");

    JsonValue* command_name = json_object_get_cstr(command, "name");
    ASSERT(command_name && command_name->kind == JSON_STRING,
           "CLI parse result command must contain a string name");

    string name   = json_string(command_name);
    int    result = 1;
    bool   verbose =
        nerd_cli_flag_bool(cli_result, "global_flags.verbose", false);

    if (verbose && !string_eq_cstr(name, "lsp")) {
        nerd_print_args_table(argc, argv);
    }

    if (string_eq_cstr(name, "build") || string_eq_cstr(name, "b")) {
        NerdBuildConfig config =
            nerd_build_config_from_json(cli_result, cli_keywords);
        result = compiler_cmd_build(&config);
    } else if (string_eq_cstr(name, "check")) {
        NerdCheckConfig config =
            nerd_check_config_from_json(cli_result, cli_keywords);
        result = compiler_cmd_check(&config);
    } else if (string_eq_cstr(name, "run") || string_eq_cstr(name, "r")) {
        NerdRunConfig config =
            nerd_run_config_from_json(cli_result, cli_keywords);
        result = config.source.source_path.count == 0
                     ? 1
                     : compiler_cmd_run(&config);
    } else if (string_eq_cstr(name, "test") || string_eq_cstr(name, "t")) {
        NerdTestConfig config =
            nerd_test_config_from_json(cli_result, cli_keywords);
        result = compiler_cmd_test(&config);
    } else if (string_eq_cstr(name, "format") || string_eq_cstr(name, "f")) {
        NerdFormatConfig config = nerd_format_config_from_json(cli_result);
        result                  = compiler_cmd_format(&config);
    } else if (string_eq_cstr(name, "internal-test")) {
        result = nerd_internal_test(cli_result);
    } else if (string_eq_cstr(name, "lsp")) {
        lsp_log("Launching nerd lsp");
        result = lsp_run();
    } else {
        json_done(cli_result);
        cli_done(&parser);
        json_done(schema);
        arena_done(&arena);
        error_system_done();
        kill("Unhandled command " STRINGP, STRINGV(name));
    }

    json_done(cli_result);
    cli_done(&parser);
    json_done(schema);
    arena_done(&arena);
    error_system_done();
    return result;
}

//------------------------------------------------------------------------------
// Entry point for the test.

int run(int argc, char** argv) { return nerd_run_with_cli(argc, argv); }
