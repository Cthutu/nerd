//------------------------------------------------------------------------------
//> use: core intern compiler timing table cli lsp object testing

#include <cli/cli.h>
#include <compiler/compiler.h>
#include <compiler/error/error.h>
#include <lsp/lsp.h>
#include <table/table.h>

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
    //       "name": "benchmark",
    //       "summary": "Run 10000 benchmark iterations",
    //       "params": [
    //         {
    //           "name": "source",
    //           "kind": "positional",
    //           "description": "Source snippet to benchmark",
    //           "required": false
    //         }
    //       ]
    //     },
    //     {
    //       "name": "million",
    //       "summary": "Generate 1,000,000 lines and build once"
    //     },
    //     {
    //       "name": "test",
    //       "summary": "Run the compiler test command"
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

    {
        JsonValue* params = json_new_array(arena);
        JsonValue* flags  = json_new_array(arena);
        json_array_push(params,
                        nerd_cli_make_param(arena,
                                            "source",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Source snippet to compile",
                                            false));
        json_array_push(flags,
                        nerd_cli_make_flag(
                            arena, "ir", NULL, "Write generated IR to a file"));
        json_array_push(
            flags,
            nerd_cli_make_flag(
                arena, "cgen", NULL, "Write generated C to a file"));
        json_array_push(params,
                        nerd_cli_make_param(arena,
                                            "output",
                                            "named",
                                            "output",
                                            "o",
                                            "Output binary path",
                                            false));
        json_array_push(
            commands,
            nerd_cli_make_command(
                arena, "build", "Run one normal build", flags, params));
    }

    {
        JsonValue* params = json_new_array(arena);
        json_array_push(params,
                        nerd_cli_make_param(arena,
                                            "source",
                                            "positional",
                                            NULL,
                                            NULL,
                                            "Source snippet to benchmark",
                                            false));
        json_array_push(commands,
                        nerd_cli_make_command(arena,
                                              "benchmark",
                                              "Run 10000 benchmark iterations",
                                              NULL,
                                              params));
    }

    json_array_push(
        commands,
        nerd_cli_make_command(arena,
                              "million",
                              "Generate 1,000,000 lines and build once",
                              NULL,
                              NULL));
    json_array_push(
        commands,
        nerd_cli_make_command(
            arena, "test", "Run the compiler test command", NULL, NULL));
    json_array_push(
        commands,
        nerd_cli_make_command(arena, "lsp", "Run the LSP server", NULL, NULL));

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
nerd_build_config_from_json(const JsonValue* cli_result)
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
        .emit_ir = nerd_cli_flag_bool(cli_result, "command.flags.ir", false),
        .emit_c  = nerd_cli_flag_bool(cli_result, "command.flags.cgen", false),
        .verbose =
            nerd_cli_flag_bool(cli_result, "global_flags.verbose", false),
    };
}

internal NerdBenchmarkConfig
nerd_benchmark_config_from_json(const JsonValue* cli_result)
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

    return (NerdBenchmarkConfig){
        .source =
            (NerdSource){
                .source      = source_text,
                .source_path = source_path,
            },
    };
}

internal NerdMillionConfig
nerd_million_config_from_json(const JsonValue* cli_result)
{
    UNUSED(cli_result);
    return (NerdMillionConfig){0};
}

internal NerdTestConfig nerd_test_config_from_json(const JsonValue* cli_result)
{
    UNUSED(cli_result);
    return (NerdTestConfig){0};
}

internal int nerd_run_with_cli(int argc, char** argv)
{
    Arena arena = {0};
    arena_init(&arena);
    error_system_init(ERROR_RENDER_NORMAL);

    JsonValue* schema = nerd_cli_schema(&arena);
    CliParser  parser = {0};
    cli_init(&parser, schema);

    JsonValue* cli_result = cli_parse(&parser, &arena, argc, argv);
    JsonValue* command    = json_object_get_cstr(cli_result, "command");
    JsonValue* help       = json_object_get_cstr(cli_result, "help_requested");
    JsonValue* ok         = json_object_get_cstr(cli_result, "ok");

    if (help && help->kind == JSON_BOOL && json_bool(help)) {
        if (argc <= 1) {
            dump_info();
        }

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
        if (argc <= 1) {
            dump_info();
        }
        cli_print_help(&parser);
        if (error && error->kind == JSON_STRING) {
            string error_message = json_string(error);
            json_done(cli_result);
            cli_done(&parser);
            json_done(schema);
            arena_done(&arena);
            error_system_done();
            kill("%.*s", STRINGV(error_message));
        }

        json_done(cli_result);
        cli_done(&parser);
        json_done(schema);
        arena_done(&arena);
        error_system_done();
        kill("CLI parse failed.");
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

    if (string_eq_cstr(name, "build")) {
        NerdBuildConfig config = nerd_build_config_from_json(cli_result);
        result                 = compiler_cmd_build(&config);
    } else if (string_eq_cstr(name, "benchmark")) {
        NerdBenchmarkConfig config =
            nerd_benchmark_config_from_json(cli_result);
        result = compiler_cmd_benchmark(&config);
    } else if (string_eq_cstr(name, "million")) {
        NerdMillionConfig config = nerd_million_config_from_json(cli_result);
        result                   = compiler_cmd_million(&config);
    } else if (string_eq_cstr(name, "test")) {
        NerdTestConfig config = nerd_test_config_from_json(cli_result);
        result                = compiler_cmd_test(&config);
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
