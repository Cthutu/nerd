//------------------------------------------------------------------------------
// Command-line parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <cli/cli.h>
#include <table/table.h>

//------------------------------------------------------------------------------

internal bool cli_string_is_char(string value)
{
    return value.count == 1;
}

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

internal bool cli_string_is_empty(string value)
{
    return value.count == 0;
}

internal string cli_json_required_string(const JsonValue* object, cstr key)
{
    JsonValue* value = json_object_get_cstr(object, key);
    ASSERT(value && value->kind == JSON_STRING,
           "CLI schema field '%s' must be a string",
           key);
    return json_string(value);
}

internal string
cli_json_optional_string(const JsonValue* object, cstr key, string fallback)
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

internal bool cli_json_optional_bool(const JsonValue* object,
                                     cstr             key,
                                     bool             fallback)
{
    JsonValue* value = json_object_get_cstr(object, key);
    if (!value) {
        return fallback;
    }
    ASSERT(value->kind == JSON_BOOL,
           "CLI schema field '%s' must be a bool",
           key);
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

    ASSERT(false,
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
        ASSERT(cli_string_is_char(short_string),
               "CLI schema short option must be a single character");
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
        ASSERT(cli_string_is_char(short_string),
               "CLI schema short option must be a single character");
        param.short_name = (char)short_string.data[0];
    }

    if (param.kind == CLI_PARAM_NAMED) {
        ASSERT(!cli_string_is_empty(param.long_name),
               "Named CLI params must define a long option");
    } else {
        ASSERT(cli_string_is_empty(param.long_name) && param.short_name == '\0',
               "Positional CLI params cannot define short/long option names");
    }

    return param;
}

internal void cli_schema_load_flags(Array(CliFlag)* out_flags,
                                    const JsonValue* flags_value)
{
    if (!flags_value) {
        return;
    }

    ASSERT(flags_value->kind == JSON_ARRAY, "CLI schema 'flags' must be an array");
    for (usize i = 0; i < array_count(flags_value->array.values); i++) {
        array_push(*out_flags, cli_schema_make_flag(flags_value->array.values[i]));
    }
}

internal void cli_schema_load_params(Array(CliParam)* out_params,
                                     const JsonValue* params_value)
{
    if (!params_value) {
        return;
    }

    ASSERT(params_value->kind == JSON_ARRAY, "CLI schema 'params' must be an array");
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
        if (params[i].kind == CLI_PARAM_NAMED && params[i].short_name == short_name) {
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
        CliFlag* flag = &flags[i];
        ASSERT(!cli_string_is_empty(flag->long_name),
               "CLI flags must define a long option name");

        for (usize j = i + 1; j < array_count(flags); j++) {
            CliFlag* other = &flags[j];
            ASSERT(!string_eq(flag->long_name, other->long_name),
                   "Duplicate long flag --" STRINGP,
                   STRINGV(flag->long_name));

            ASSERT(flag->short_name == '\0' || other->short_name == '\0' ||
                       flag->short_name != other->short_name,
                   "Duplicate short flag -%c",
                   flag->short_name);
        }
    }
}

internal void cli_validate_unique_params(Array(CliParam) params)
{
    usize positional_count = 0;

    for (usize i = 0; i < array_count(params); i++) {
        CliParam* param = &params[i];
        ASSERT(!cli_string_is_empty(param->name),
               "CLI params must define a name");

        if (param->kind == CLI_PARAM_POSITIONAL) {
            positional_count++;
        }

        for (usize j = i + 1; j < array_count(params); j++) {
            CliParam* other = &params[j];
            ASSERT(!string_eq(param->name, other->name),
                   "Duplicate CLI param name " STRINGP,
                   STRINGV(param->name));

            if (param->kind == CLI_PARAM_NAMED && other->kind == CLI_PARAM_NAMED) {
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

    ASSERT(positional_count <= array_count(params),
           "Invalid positional param count");
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
        ASSERT(!cli_string_is_empty(command->name),
               "CLI commands must define a name");

        for (usize j = i + 1; j < array_count(parser->commands); j++) {
            ASSERT(!string_eq(command->name, parser->commands[j].name),
                   "Duplicate command " STRINGP,
                   STRINGV(command->name));
        }

        cli_validate_command(command);
    }
}

internal void cli_set_json_bool(JsonValue* object,
                                Arena*     arena,
                                string     key,
                                bool       value)
{
    string key_copy = string_format(arena, STRINGP, STRINGV(key));
    json_object_set_bool(object, arena, (cstr)key_copy.data, value);
}

internal void cli_set_json_string(JsonValue* object,
                                  Arena*     arena,
                                  string     key,
                                  string     value)
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
            option_str = string_format(
                arena, "-%c, --" STRINGP " " STRINGP, short_name, STRINGV(long_name), STRINGV(suffix));
        } else {
            option_str = string_format(
                arena, "-%c, --" STRINGP, short_name, STRINGV(long_name));
        }
    } else if (suffix.count > 0) {
        option_str = string_format(
            arena, "--" STRINGP " " STRINGP, STRINGV(long_name), STRINGV(suffix));
    } else {
        option_str = string_format(arena, "--" STRINGP, STRINGV(long_name));
    }

    TableCell row[] = {table_cell_string(option_str), table_cell_string(description)};
    table_add_row(table, row);
}

internal void cli_print_named_options(Array(CliFlag) flags,
                                      Array(CliParam) params,
                                      cstr            title)
{
    Array(TableColumn) columns = NULL;
    array_push(
        columns,
        (TableColumn){.title = "Option", .colour = ANSI_BOLD_CYAN},
        (TableColumn){.title = "Description", .colour = ANSI_BOLD_GREEN});

    Table table = {0};
    table_init(&table, columns, .title = title);
    array_free(columns);
    table_reserve_rows(&table, array_count(flags) + 1);

    Arena arena = {0};
    arena_init(&arena);

    {
        TableCell row[] = {table_cell_string(s("-h, --help")),
                           table_cell_string(s("Show this help message"))};
        table_add_row(&table, row);
    }

    for (usize i = 0; i < array_count(flags); i++) {
        const CliFlag* flag       = &flags[i];
        string         option_str = {0};
        if (flag->short_name != '\0') {
            option_str = string_format(
                &arena, "-%c, --%s", flag->short_name, flag->long_name);
        } else {
            option_str = string_format(&arena, "--%s", flag->long_name);
        }

        TableCell row[] = {table_cell_string(option_str),
                           table_cell_string(s(flag->description))};
        table_add_row(&table, row);
    }

    table_print(&table);
    table_done(&table);
    arena_done(&arena);
}

void cli_init(CliParser* parser, cstr program_name, cstr summary)
{
    *parser              = (CliParser){0};
    parser->program_name = program_name;
    parser->summary      = summary;
}

void cli_done(CliParser* parser)
{
    array_free(parser->root_flags);

    for (usize i = 0; i < array_count(parser->commands); i++) {
        array_free(parser->commands[i].flags);
    }
    array_free(parser->commands);
    *parser = (CliParser){0};
}

void cli_add_root_flag(CliParser* parser,
                       char       short_name,
                       cstr       long_name,
                       cstr       description,
                       bool*      out_value)
{
    cli_add_flag(
        &parser->root_flags, short_name, long_name, description, out_value);
}

usize cli_add_command(CliParser* parser, cstr name, cstr summary)
{
    ASSERT(name != NULL && name[0] != '\0', "Command must have a name");
    ASSERT(cli_find_command_index(parser, name) == -1,
           "Duplicate command '%s'",
           name);

    usize index = array_count(parser->commands);
    array_push(parser->commands,
               (CliCommand){.name    = name,
                            .summary = summary ? summary : "",
                            .flags   = NULL});
    return index;
}

void cli_add_command_flag(CliParser* parser,
                          usize      command_index,
                          char       short_name,
                          cstr       long_name,
                          cstr       description,
                          bool*      out_value)
{
    ASSERT(command_index < array_count(parser->commands),
           "Invalid command index: %zu",
           command_index);

    CliCommand* command = &parser->commands[command_index];
    cli_add_flag(
        &command->flags, short_name, long_name, description, out_value);
}

CliParseResult cli_parse(CliParser* parser, int argc, char** argv)
{
    CliParseResult result = {.help_requested = false, .command_index = -1};

    for (int i = 1; i < argc; i++) {
        cstr arg = argv[i];

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            result.help_requested = true;
            continue;
        }

        if (arg[0] == '-' && arg[1] == '-') {
            cstr name = arg + 2;

            CliFlag* flag = NULL;
            if (result.command_index >= 0) {
                CliCommand* command = &parser->commands[result.command_index];
                flag                = cli_find_long_flag(command->flags, name);
            }
            if (!flag) {
                flag = cli_find_long_flag(parser->root_flags, name);
            }
            if (!flag) {
                kill("Unknown flag '%s'. Use --help to list options.", arg);
            }

            *flag->out_value = true;
            continue;
        }

        if (arg[0] == '-' && arg[1] != '\0' && arg[2] == '\0') {
            char name = arg[1];

            CliFlag* flag = NULL;
            if (result.command_index >= 0) {
                CliCommand* command = &parser->commands[result.command_index];
                flag                = cli_find_short_flag(command->flags, name);
            }
            if (!flag) {
                flag = cli_find_short_flag(parser->root_flags, name);
            }
            if (!flag) {
                kill("Unknown flag '%s'. Use --help to list options.", arg);
            }

            *flag->out_value = true;
            continue;
        }

        isize command_index = cli_find_command_index(parser, arg);
        if (command_index >= 0) {
            if (result.command_index >= 0) {
                kill("Only one command can be used at a time. Got '%s'.", arg);
            }
            result.command_index = command_index;
            continue;
        }

        kill("Unknown argument '%s'. Use --help to list options.", arg);
    }

    return result;
}

void cli_print_help(const CliParser* parser)
{
    cstr program_name = parser->program_name ? parser->program_name : "program";
    prn("%s%s%s", ANSI_BOLD_CYAN, program_name, ANSI_RESET);
    if (parser->summary && parser->summary[0] != '\0') {
        prn("%s%s%s", ANSI_FAINT_WHITE, parser->summary, ANSI_RESET);
    }

    prn("");
    prn("%sUsage%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    prn("  %s [global options] <command> [command options]", program_name);

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
        TableCell         row[]   = {table_cell_string(s(command->name)),
                                     table_cell_string(s(command->summary))};
        table_add_row(&commands_table, row);
    }

    table_print(&commands_table);
    table_done(&commands_table);

    prn("");
    prn("%sGlobal Options%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    cli_print_flags_table(parser->root_flags, "Global Options");
}

void cli_print_command_help(const CliParser* parser, usize command_index)
{
    ASSERT(command_index < array_count(parser->commands),
           "Invalid command index: %zu",
           command_index);

    const CliCommand* command = &parser->commands[command_index];
    cstr              program_name = parser->program_name ? parser->program_name
                                                           : "program";

    prn("%s%s %s%s", ANSI_BOLD_CYAN, program_name, command->name, ANSI_RESET);
    if (command->summary && command->summary[0] != '\0') {
        prn("%s%s%s", ANSI_FAINT_WHITE, command->summary, ANSI_RESET);
    }

    prn("");
    prn("%sUsage%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    prn("  %s %s [options]", program_name, command->name);

    prn("");
    prn("%sCommand Options%s", ANSI_BOLD_YELLOW, ANSI_RESET);
    cli_print_flags_table(command->flags, "Command Options");

    if (array_count(parser->root_flags) > 0) {
        prn("");
        prn("%sGlobal Options%s", ANSI_BOLD_YELLOW, ANSI_RESET);
        cli_print_flags_table(parser->root_flags, "Global Options");
    }
}

//------------------------------------------------------------------------------
