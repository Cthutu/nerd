//------------------------------------------------------------------------------
// Command-line parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <cli/cli.h>
#include <table/table.h>

//------------------------------------------------------------------------------

#if CONFIG_DEBUG
internal bool cli_string_is_char(string value) { return value.count == 1; }
#endif

internal cstr cli_param_kind_label(CliParamKind kind)
{
    switch (kind) {
    case CLI_PARAM_NAMED:
        return "named";
    case CLI_PARAM_POSITIONAL:
        return "positional";
    default:
        ASSERT(false, "Invalid CliParamKind");
        return "";
    }
}

#if CONFIG_DEBUG
internal bool cli_string_is_empty(string value) { return value.count == 0; }
#endif

internal string cli_json_required_string(const JsonValue* object, cstr key)
{
    JsonValue* value = json_object_get_cstr(object, key);
    ASSERT(value && value->kind == JSON_STRING,
           "CLI schema field '%s' must be a string",
           key);
    return json_string(value);
}

internal string cli_json_optional_string(const JsonValue* object,
                                         cstr             key,
                                         string           fallback)
{
    JsonValue* value = json_object_get_cstr(object, key);
    if (!value) {
        return fallback;
    }
    ASSERT(value->kind == JSON_STRING,
           "CLI schema field '%s' must be a string",
           key);
    return json_string(value);
}

internal bool
cli_json_optional_bool(const JsonValue* object, cstr key, bool fallback)
{
    JsonValue* value = json_object_get_cstr(object, key);
    if (!value) {
        return fallback;
    }
    ASSERT(
        value->kind == JSON_BOOL, "CLI schema field '%s' must be a bool", key);
    return json_bool(value);
}

internal CliParamKind cli_json_param_kind(const JsonValue* object)
{
    JsonValue* kind = json_object_get_cstr(object, "kind");
    if (!kind) {
        return CLI_PARAM_NAMED;
    }

    ASSERT(kind->kind == JSON_STRING,
           "CLI schema field 'kind' must be a string");
    string kind_string = json_string(kind);
    if (string_eq_cstr(kind_string, "named")) {
        return CLI_PARAM_NAMED;
    }
    if (string_eq_cstr(kind_string, "positional")) {
        return CLI_PARAM_POSITIONAL;
    }

    ASSERT(
        false,
        "CLI schema param kind must be 'named' or 'positional', got " STRINGP,
        STRINGV(kind_string));
    return CLI_PARAM_NAMED;
}

internal CliFlag cli_schema_make_flag(const JsonValue* value)
{
    ASSERT(value && value->kind == JSON_OBJECT,
           "CLI schema flags must be objects");

    CliFlag flag = {
        .short_name  = '\0',
        .long_name   = cli_json_required_string(value, "long"),
        .description = cli_json_optional_string(value, "description", s("")),
    };

    JsonValue* short_name = json_object_get_cstr(value, "short");
    if (short_name) {
        ASSERT(short_name->kind == JSON_STRING,
               "CLI schema field 'short' must be a string");
        string short_string = json_string(short_name);
#if CONFIG_DEBUG
        ASSERT(cli_string_is_char(short_string),
               "CLI schema short option must be a single character");
#endif
        flag.short_name = (char)short_string.data[0];
    }

    return flag;
}

internal CliParam cli_schema_make_param(const JsonValue* value)
{
    ASSERT(value && value->kind == JSON_OBJECT,
           "CLI schema params must be objects");

    CliParam param = {
        .name        = cli_json_required_string(value, "name"),
        .short_name  = '\0',
        .long_name   = cli_json_optional_string(value, "long", s("")),
        .description = cli_json_optional_string(value, "description", s("")),
        .required    = cli_json_optional_bool(value, "required", false),
        .kind        = cli_json_param_kind(value),
    };

    JsonValue* short_name = json_object_get_cstr(value, "short");
    if (short_name) {
        ASSERT(short_name->kind == JSON_STRING,
               "CLI schema field 'short' must be a string");
        string short_string = json_string(short_name);
#if CONFIG_DEBUG
        ASSERT(cli_string_is_char(short_string),
               "CLI schema short option must be a single character");
#endif
        param.short_name = (char)short_string.data[0];
    }

#if CONFIG_DEBUG
    if (param.kind == CLI_PARAM_NAMED) {
        ASSERT(!cli_string_is_empty(param.long_name),
               "Named CLI params must define a long option");
    } else {
        ASSERT(cli_string_is_empty(param.long_name) && param.short_name == '\0',
               "Positional CLI params cannot define short/long option names");
    }
#endif

    return param;
}

internal void cli_schema_load_flags(Array(CliFlag) * out_flags,
                                    const JsonValue* flags_value)
{
    if (!flags_value) {
        return;
    }

    ASSERT(flags_value->kind == JSON_ARRAY,
           "CLI schema 'flags' must be an array");
    for (usize i = 0; i < array_count(flags_value->array.values); i++) {
        array_push(*out_flags,
                   cli_schema_make_flag(flags_value->array.values[i]));
    }
}

internal void cli_schema_load_params(Array(CliParam) * out_params,
                                     const JsonValue* params_value)
{
    if (!params_value) {
        return;
    }

    ASSERT(params_value->kind == JSON_ARRAY,
           "CLI schema 'params' must be an array");
    for (usize i = 0; i < array_count(params_value->array.values); i++) {
        array_push(*out_params,
                   cli_schema_make_param(params_value->array.values[i]));
    }
}

internal CliFlag* cli_find_short_flag(Array(CliFlag) flags, char short_name)
{
    for (usize i = 0; i < array_count(flags); i++) {
        if (flags[i].short_name == short_name) {
            return &flags[i];
        }
    }
    return NULL;
}

internal CliParam* cli_find_short_param(Array(CliParam) params, char short_name)
{
    for (usize i = 0; i < array_count(params); i++) {
        if (params[i].kind == CLI_PARAM_NAMED &&
            params[i].short_name == short_name) {
            return &params[i];
        }
    }
    return NULL;
}

internal CliParam* cli_next_positional_param(const CliCommand* command,
                                             usize*            io_position)
{
    usize positional_index = *io_position;
    for (usize i = 0; i < array_count(command->params); i++) {
        CliParam* param = &command->params[i];
        if (param->kind != CLI_PARAM_POSITIONAL) {
            continue;
        }

        if (positional_index == 0) {
            *io_position += 1;
            return param;
        }

        positional_index--;
    }

    return NULL;
}

internal usize cli_count_positionals(const CliCommand* command)
{
    usize count = 0;
    for (usize i = 0; i < array_count(command->params); i++) {
        if (command->params[i].kind == CLI_PARAM_POSITIONAL) {
            count++;
        }
    }
    return count;
}

internal isize cli_find_command_index(const CliParser* parser, cstr name)
{
    for (usize i = 0; i < array_count(parser->commands); i++) {
        if (string_eq_cstr(parser->commands[i].name, name)) {
            return (isize)i;
        }
    }
    return -1;
}

internal void cli_validate_unique_flags(Array(CliFlag) flags)
{
    for (usize i = 0; i < array_count(flags); i++) {
        ASSERT(flags[i].long_name.count != 0,
               "CLI flags must define a long option name");

        for (usize j = i + 1; j < array_count(flags); j++) {
            ASSERT(!string_eq(flags[i].long_name, flags[j].long_name),
                   "Duplicate long flag --" STRINGP,
                   STRINGV(flags[i].long_name));

            ASSERT(flags[i].short_name == '\0' || flags[j].short_name == '\0' ||
                       flags[i].short_name != flags[j].short_name,
                   "Duplicate short flag -%c",
                   flags[i].short_name);
        }
    }
}

internal void cli_validate_unique_params(Array(CliParam) params)
{
#if CONFIG_DEBUG
    usize positional_count = 0;
#endif

    for (usize i = 0; i < array_count(params); i++) {
        CliParam* param = &params[i];
        ASSERT(param->name.count != 0, "CLI params must define a name");

#if CONFIG_DEBUG
        if (param->kind == CLI_PARAM_POSITIONAL) {
            positional_count++;
        }
#endif

        for (usize j = i + 1; j < array_count(params); j++) {
            CliParam* other = &params[j];
            ASSERT(!string_eq(param->name, other->name),
                   "Duplicate CLI param name " STRINGP,
                   STRINGV(param->name));

            if (param->kind == CLI_PARAM_NAMED &&
                other->kind == CLI_PARAM_NAMED) {
                ASSERT(!string_eq(param->long_name, other->long_name),
                       "Duplicate long option --" STRINGP,
                       STRINGV(param->long_name));
                ASSERT(param->short_name == '\0' || other->short_name == '\0' ||
                           param->short_name != other->short_name,
                       "Duplicate short option -%c",
                       param->short_name);
            }
        }
    }

#if CONFIG_DEBUG
    ASSERT(positional_count <= array_count(params),
           "Invalid positional param count");
#endif
}

internal void cli_validate_command(const CliCommand* command)
{
    cli_validate_unique_flags(command->flags);
    cli_validate_unique_params(command->params);
}

internal void cli_validate_parser(const CliParser* parser)
{
    cli_validate_unique_flags(parser->root_flags);
    cli_validate_unique_params(parser->root_params);

    for (usize i = 0; i < array_count(parser->commands); i++) {
        CliCommand* command = &parser->commands[i];
#if CONFIG_DEBUG
        ASSERT(!cli_string_is_empty(command->name),
               "CLI commands must define a name");
#endif

        for (usize j = i + 1; j < array_count(parser->commands); j++) {
            ASSERT(!string_eq(command->name, parser->commands[j].name),
                   "Duplicate command " STRINGP,
                   STRINGV(command->name));
        }

        cli_validate_command(command);
    }
}

internal void
cli_set_json_bool(JsonValue* object, Arena* arena, string key, bool value)
{
    string key_copy = string_format(arena, STRINGP, STRINGV(key));
    json_object_set_bool(object, arena, (cstr)key_copy.data, value);
}

internal void
cli_set_json_string(JsonValue* object, Arena* arena, string key, string value)
{
    string key_copy = string_format(arena, STRINGP, STRINGV(key));
    json_object_set_string(object, arena, (cstr)key_copy.data, value);
}

internal void cli_add_named_option_row(Table* table,
                                       Arena* arena,
                                       char   short_name,
                                       string long_name,
                                       string suffix,
                                       string description)
{
    string option_str = {0};
    if (short_name != '\0') {
        if (suffix.count > 0) {
            option_str = string_format(arena,
                                       "-%c, --" STRINGP " " STRINGP,
                                       short_name,
                                       STRINGV(long_name),
                                       STRINGV(suffix));
        } else {
            option_str = string_format(
                arena, "-%c, --" STRINGP, short_name, STRINGV(long_name));
        }
    } else if (suffix.count > 0) {
        option_str = string_format(arena,
                                   "--" STRINGP " " STRINGP,
                                   STRINGV(long_name),
                                   STRINGV(suffix));
    } else {
        option_str = string_format(arena, "--" STRINGP, STRINGV(long_name));
    }

    TableCell row[] = {table_cell_string(option_str),
                       table_cell_string(description)};
    table_add_row(table, row);
}

internal void cli_print_named_options(Array(CliFlag) flags,
                                      Array(CliParam) params,
                                      cstr title)
{
    Array(TableColumn) columns = NULL;
    array_push(
        columns,
        (TableColumn){.title = "Option", .colour = ANSI_BOLD_CYAN},
        (TableColumn){.title = "Description", .colour = ANSI_BOLD_GREEN});

    Table table = {0};
    table_init(&table, columns, .title = title);
    array_free(columns);
    table_reserve_rows(&table, array_count(flags) + array_count(params) + 1);

    Arena arena = {0};
    arena_init(&arena);

    {
        TableCell row[] = {table_cell_string(s("-h, --help")),
                           table_cell_string(s("Show this help message"))};
        table_add_row(&table, row);
    }

    for (usize i = 0; i < array_count(flags); i++) {
        CliFlag* flag = &flags[i];
        cli_add_named_option_row(&table,
                                 &arena,
                                 flag->short_name,
                                 flag->long_name,
                                 s(""),
                                 flag->description);
    }

    for (usize i = 0; i < array_count(params); i++) {
        CliParam* param = &params[i];
        if (param->kind != CLI_PARAM_NAMED) {
            continue;
        }

        string suffix =
            string_format(&arena, "<" STRINGP ">", STRINGV(param->name));
        cli_add_named_option_row(&table,
                                 &arena,
                                 param->short_name,
                                 param->long_name,
                                 suffix,
                                 param->description);
    }

    table_print(&table);
    table_done(&table);
    arena_done(&arena);
}

internal void cli_print_positionals(Array(CliParam) params, cstr title)
{
    Array(TableColumn) columns = NULL;
    array_push(
        columns,
        (TableColumn){.title = "Parameter", .colour = ANSI_BOLD_CYAN},
        (TableColumn){.title = "Kind", .colour = ANSI_BOLD_YELLOW},
        (TableColumn){.title = "Description", .colour = ANSI_BOLD_GREEN});

    Table table = {0};
    table_init(&table, columns, .title = title);
    array_free(columns);

    bool has_rows = false;
    for (usize i = 0; i < array_count(params); i++) {
        CliParam* param = &params[i];
        if (param->kind != CLI_PARAM_POSITIONAL) {
            continue;
        }

        string    kind  = param->required ? s("required") : s("optional");
        TableCell row[] = {table_cell_string(param->name),
                           table_cell_string(kind),
                           table_cell_string(param->description)};
        table_add_row(&table, row);
        has_rows = true;
    }

    if (has_rows) {
        table_print(&table);
    }
    table_done(&table);
}

internal string cli_command_usage(const CliParser*  parser,
                                  Arena*            arena,
                                  const CliCommand* command)
{
    string usage = string_format(arena, STRINGP, STRINGV(parser->program_name));

    if (array_count(parser->root_flags) > 0 ||
        array_count(parser->root_params) > 0) {
        usage =
            string_format(arena, STRINGP " [global options]", STRINGV(usage));
    }

    usage = string_format(
        arena, STRINGP " " STRINGP, STRINGV(usage), STRINGV(command->name));

    if (array_count(command->flags) > 0 || array_count(command->params) > 0) {
        usage = string_format(arena, STRINGP " [options]", STRINGV(usage));
    }

    for (usize i = 0; i < array_count(command->params); i++) {
        CliParam* param = &command->params[i];
        if (param->kind != CLI_PARAM_POSITIONAL) {
            continue;
        }

        if (param->required) {
            usage = string_format(arena,
                                  STRINGP " <" STRINGP ">",
                                  STRINGV(usage),
                                  STRINGV(param->name));
        } else {
            usage = string_format(arena,
                                  STRINGP " [" STRINGP "]",
                                  STRINGV(usage),
                                  STRINGV(param->name));
        }
    }

    return usage;
}

internal JsonValue*
cli_parse_error(const CliParser* parser, Arena* arena, cstr format, ...)
{
    UNUSED(parser);

    va_list args;
    va_start(args, format);
    string message = string_formatv(arena, format, args);
    va_end(args);

    JsonValue* result = json_new_object(arena);
    json_object_set_bool(result, arena, "ok", false);
    json_object_set_bool(result, arena, "help_requested", false);
    json_object_set_string(result, arena, "error", message);
    return result;
}

void cli_init(CliParser* parser, const JsonValue* schema)
{
    ASSERT(parser != NULL, "CliParser cannot be NULL");
    ASSERT(schema && schema->kind == JSON_OBJECT,
           "CLI schema must be a JSON object");

    *parser              = (CliParser){0};

    parser->program_name = cli_json_required_string(schema, "program");
    parser->summary      = cli_json_optional_string(schema, "summary", s(""));

    cli_schema_load_flags(&parser->root_flags,
                          json_object_get_cstr(schema, "flags"));
    cli_schema_load_params(&parser->root_params,
                           json_object_get_cstr(schema, "params"));

    JsonValue* commands = json_object_get_cstr(schema, "commands");
    ASSERT(commands && commands->kind == JSON_ARRAY,
           "CLI schema field 'commands' must be an array");

    for (usize i = 0; i < array_count(commands->array.values); i++) {
        JsonValue* value = commands->array.values[i];
        ASSERT(value && value->kind == JSON_OBJECT,
               "CLI schema commands must be objects");

        CliCommand command = {
            .name    = cli_json_required_string(value, "name"),
            .summary = cli_json_optional_string(value, "summary", s("")),
            .flags   = NULL,
            .params  = NULL,
        };

        cli_schema_load_flags(&command.flags,
                              json_object_get_cstr(value, "flags"));
        cli_schema_load_params(&command.params,
                               json_object_get_cstr(value, "params"));
        array_push(parser->commands, command);
    }

    cli_validate_parser(parser);
}

void cli_done(CliParser* parser)
{
    array_free(parser->root_flags);
    array_free(parser->root_params);

    for (usize i = 0; i < array_count(parser->commands); i++) {
        array_free(parser->commands[i].flags);
        array_free(parser->commands[i].params);
    }

    array_free(parser->commands);
    *parser = (CliParser){0};
}

JsonValue*
cli_parse(const CliParser* parser, Arena* arena, int argc, char** argv)
{
    JsonValue* result        = json_new_object(arena);
    JsonValue* global_flags  = json_new_object(arena);
    JsonValue* global_params = json_new_object(arena);
    JsonValue* positionals   = json_new_array(arena);

    json_object_set_bool(result, arena, "ok", true);
    json_object_set_bool(result, arena, "help_requested", false);
    json_object_set_object(result, "global_flags", global_flags);
    json_object_set_object(result, "global_params", global_params);
    json_object_set_array(result, "positionals", positionals);

    JsonValue*        command_result   = NULL;
    const CliCommand* current_command  = NULL;
    usize             positional_index = 0;

    for (int i = 1; i < argc; i++) {
        cstr arg = argv[i];

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            json_object_set_bool(result, arena, "help_requested", true);
            continue;
        }

        if (arg[0] == '-' && arg[1] == '-') {
            cstr   option       = arg + 2;
            cstr   option_value = NULL;
            string option_name  = s(option);
            cstr   equals       = strchr(option, '=');
            if (equals) {
                option_name =
                    string_from((u8*)option, (usize)(equals - option));
                option_value = equals + 1;
            }

            CliFlag* root_flag = NULL;
            for (usize flag_index = 0;
                 flag_index < array_count(parser->root_flags);
                 flag_index++) {
                if (string_eq(parser->root_flags[flag_index].long_name,
                              option_name)) {
                    root_flag = &parser->root_flags[flag_index];
                    break;
                }
            }
            if (root_flag) {
                cli_set_json_bool(
                    global_flags, arena, root_flag->long_name, true);
                continue;
            }

            CliParam* root_param = NULL;
            for (usize param_index = 0;
                 param_index < array_count(parser->root_params);
                 param_index++) {
                if (parser->root_params[param_index].kind == CLI_PARAM_NAMED &&
                    string_eq(parser->root_params[param_index].long_name,
                              option_name)) {
                    root_param = &parser->root_params[param_index];
                    break;
                }
            }
            if (root_param) {
                if (!option_value) {
                    if (i + 1 >= argc) {
                        json_done(result);
                        return cli_parse_error(
                            parser,
                            arena,
                            "Missing value for option '--" STRINGP "'.",
                            STRINGV(option_name));
                    }
                    option_value = argv[++i];
                }
                cli_set_json_string(
                    global_params, arena, root_param->name, s(option_value));
                continue;
            }

            if (!current_command) {
                json_done(result);
                return cli_parse_error(parser,
                                       arena,
                                       "Unknown option '--" STRINGP
                                       "'. Use --help to list options.",
                                       STRINGV(option_name));
            }

            CliFlag* command_flag = NULL;
            for (usize flag_index = 0;
                 flag_index < array_count(current_command->flags);
                 flag_index++) {
                if (string_eq(current_command->flags[flag_index].long_name,
                              option_name)) {
                    command_flag = &current_command->flags[flag_index];
                    break;
                }
            }
            if (command_flag) {
                cli_set_json_bool(json_object_get_cstr(command_result, "flags"),
                                  arena,
                                  command_flag->long_name,
                                  true);
                continue;
            }

            CliParam* command_param = NULL;
            for (usize param_index = 0;
                 param_index < array_count(current_command->params);
                 param_index++) {
                if (current_command->params[param_index].kind ==
                        CLI_PARAM_NAMED &&
                    string_eq(current_command->params[param_index].long_name,
                              option_name)) {
                    command_param = &current_command->params[param_index];
                    break;
                }
            }
            if (command_param) {
                if (!option_value) {
                    if (i + 1 >= argc) {
                        json_done(result);
                        return cli_parse_error(
                            parser,
                            arena,
                            "Missing value for option '--" STRINGP "'.",
                            STRINGV(option_name));
                    }
                    option_value = argv[++i];
                }
                cli_set_json_string(
                    json_object_get_cstr(command_result, "params"),
                    arena,
                    command_param->name,
                    s(option_value));
                continue;
            }

            json_done(result);
            return cli_parse_error(parser,
                                   arena,
                                   "Unknown option '--" STRINGP
                                   "'. Use --help to list options.",
                                   STRINGV(option_name));
        }

        if (arg[0] == '-' && arg[1] != '\0' && arg[2] == '\0') {
            char option = arg[1];

            CliFlag* root_flag =
                cli_find_short_flag(parser->root_flags, option);
            if (root_flag) {
                cli_set_json_bool(
                    global_flags, arena, root_flag->long_name, true);
                continue;
            }

            CliParam* root_param =
                cli_find_short_param(parser->root_params, option);
            if (root_param) {
                if (i + 1 >= argc) {
                    json_done(result);
                    return cli_parse_error(parser,
                                           arena,
                                           "Missing value for option '-%c'.",
                                           option);
                }
                cli_set_json_string(
                    global_params, arena, root_param->name, s(argv[++i]));
                continue;
            }

            if (!current_command) {
                json_done(result);
                return cli_parse_error(
                    parser,
                    arena,
                    "Unknown option '-%c'. Use --help to list options.",
                    option);
            }

            CliFlag* command_flag =
                cli_find_short_flag(current_command->flags, option);
            if (command_flag) {
                cli_set_json_bool(json_object_get_cstr(command_result, "flags"),
                                  arena,
                                  command_flag->long_name,
                                  true);
                continue;
            }

            CliParam* command_param =
                cli_find_short_param(current_command->params, option);
            if (command_param) {
                if (i + 1 >= argc) {
                    json_done(result);
                    return cli_parse_error(parser,
                                           arena,
                                           "Missing value for option '-%c'.",
                                           option);
                }
                cli_set_json_string(
                    json_object_get_cstr(command_result, "params"),
                    arena,
                    command_param->name,
                    s(argv[++i]));
                continue;
            }

            json_done(result);
            return cli_parse_error(
                parser,
                arena,
                "Unknown option '-%c'. Use --help to list options.",
                option);
        }

        isize command_index = cli_find_command_index(parser, arg);
        if (command_index >= 0) {
            if (current_command) {
                json_done(result);
                return cli_parse_error(
                    parser,
                    arena,
                    "Only one command can be used at a time. Got '%s'.",
                    arg);
            }

            current_command  = &parser->commands[command_index];
            positional_index = 0;

            command_result   = json_new_object(arena);
            json_object_set_string(
                command_result, arena, "name", current_command->name);
            json_object_set_object(
                command_result, "flags", json_new_object(arena));
            json_object_set_object(
                command_result, "params", json_new_object(arena));
            json_object_set_array(
                command_result, "positionals", json_new_array(arena));
            json_object_set_object(result, "command", command_result);
            continue;
        }

        if (!current_command) {
            json_done(result);
            return cli_parse_error(
                parser,
                arena,
                "Unknown argument '%s'. Use --help to list options.",
                arg);
        }

        CliParam* positional =
            cli_next_positional_param(current_command, &positional_index);
        if (!positional) {
            json_done(result);
            return cli_parse_error(
                parser,
                arena,
                "Too many positional arguments for command " STRINGP ".",
                STRINGV(current_command->name));
        }

        JsonValue* command_params =
            json_object_get_cstr(command_result, "params");
        JsonValue* command_positionals =
            json_object_get_cstr(command_result, "positionals");
        cli_set_json_string(command_params, arena, positional->name, s(arg));
        json_array_push(command_positionals, json_new_string(arena, s(arg)));
        json_array_push(positionals, json_new_string(arena, s(arg)));
    }

    if (!current_command) {
        JsonValue* help_requested =
            json_object_get_cstr(result, "help_requested");
        if (help_requested && help_requested->kind == JSON_BOOL &&
            json_bool(help_requested)) {
            return result;
        }

        json_done(result);
        return cli_parse_error(
            parser, arena, "Missing command. Use --help to list commands.");
    }

    for (usize i = 0; i < array_count(current_command->params); i++) {
        CliParam*  param = &current_command->params[i];
        JsonValue* value = json_object_get(
            json_object_get_cstr(command_result, "params"), param->name);
        if (param->required && !value) {
            json_done(result);
            return cli_parse_error(parser,
                                   arena,
                                   "Missing required %s parameter " STRINGP
                                   " for command " STRINGP ".",
                                   cli_param_kind_label(param->kind),
                                   STRINGV(param->name),
                                   STRINGV(current_command->name));
        }
    }

    for (usize i = 0; i < array_count(parser->root_params); i++) {
        CliParam*  param = &parser->root_params[i];
        JsonValue* value = json_object_get(global_params, param->name);
        if (param->required && !value) {
            json_done(result);
            return cli_parse_error(parser,
                                   arena,
                                   "Missing required global parameter " STRINGP
                                   ".",
                                   STRINGV(param->name));
        }
    }

    return result;
}

void cli_print_help(const CliParser* parser)
{
    prn("%s" STRINGP "%s",
        ANSI_BOLD_CYAN,
        STRINGV(parser->program_name),
        ANSI_RESET);
    if (parser->summary.count > 0) {
        prn("%s" STRINGP "%s",
            ANSI_FAINT_WHITE,
            STRINGV(parser->summary),
            ANSI_RESET);
    }

    prn("");
    prn("%sUsage%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    prn("  " STRINGP " [global options] <command> [command options]",
        STRINGV(parser->program_name));

    prn("");
    prn("%sCommands%s", ANSI_BOLD_YELLOW, ANSI_RESET);

    Array(TableColumn) command_columns = NULL;
    array_push(
        command_columns,
        (TableColumn){.title = "Command", .colour = ANSI_BOLD_CYAN},
        (TableColumn){.title = "Description", .colour = ANSI_BOLD_GREEN});

    Table commands_table = {0};
    table_init(&commands_table, command_columns, .title = "Commands");
    array_free(command_columns);
    table_reserve_rows(&commands_table, array_count(parser->commands));

    for (usize i = 0; i < array_count(parser->commands); i++) {
        const CliCommand* command = &parser->commands[i];
        TableCell         row[]   = {table_cell_string(command->name),
                                     table_cell_string(command->summary)};
        table_add_row(&commands_table, row);
    }

    table_print(&commands_table);
    table_done(&commands_table);

    prn("");
    prn("%sGlobal Options%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    cli_print_named_options(
        parser->root_flags, parser->root_params, "Global Options");
}

void cli_print_command_help(const CliParser* parser, usize command_index)
{
    ASSERT(command_index < array_count(parser->commands),
           "Invalid command index: %zu",
           command_index);

    const CliCommand* command = &parser->commands[command_index];

    prn("%s" STRINGP " " STRINGP "%s",
        ANSI_BOLD_CYAN,
        STRINGV(parser->program_name),
        STRINGV(command->name),
        ANSI_RESET);
    if (command->summary.count > 0) {
        prn("%s" STRINGP "%s",
            ANSI_FAINT_WHITE,
            STRINGV(command->summary),
            ANSI_RESET);
    }

    Arena arena = {0};
    arena_init(&arena);

    prn("");
    prn("%sUsage%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    string usage = cli_command_usage(parser, &arena, command);
    prn("  " STRINGP, STRINGV(usage));

    prn("");
    prn("%sCommand Options%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    cli_print_named_options(command->flags, command->params, "Command Options");

    if (cli_count_positionals(command) > 0) {
        prn("");
        prn("%sPositional Parameters%s", ANSI_BOLD_YELLOW, ANSI_RESET);
        cli_print_positionals(command->params, "Positional Parameters");
    }

    if (array_count(parser->root_flags) > 0 ||
        array_count(parser->root_params) > 0) {
        prn("");
        prn("%sGlobal Options%s", ANSI_BOLD_YELLOW, ANSI_RESET);
        cli_print_named_options(
            parser->root_flags, parser->root_params, "Global Options");
    }

    arena_done(&arena);
}

//------------------------------------------------------------------------------
