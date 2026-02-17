//------------------------------------------------------------------------------
// Command-line parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <cli/cli.h>
#include <table/table.h>

internal CliFlag* cli_find_long_flag(Array(CliFlag) flags, cstr long_name)
{
    for (usize i = 0; i < array_count(flags); i++) {
        if (strcmp(flags[i].long_name, long_name) == 0) {
            return &flags[i];
        }
    }
    return NULL;
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

internal isize cli_find_command_index(const CliParser* parser, cstr name)
{
    for (usize i = 0; i < array_count(parser->commands); i++) {
        if (strcmp(parser->commands[i].name, name) == 0) {
            return (isize)i;
        }
    }
    return -1;
}

internal void cli_add_flag(Array(CliFlag)* flags,
                           char            short_name,
                           cstr            long_name,
                           cstr            description,
                           bool*           out_value)
{
    ASSERT(long_name != NULL && long_name[0] != '\0',
           "Flag must have long name");
    ASSERT(out_value != NULL, "Flag output pointer cannot be NULL");

    if (short_name != '\0') {
        ASSERT(cli_find_short_flag(*flags, short_name) == NULL,
               "Duplicate short flag -%c",
               short_name);
    }
    ASSERT(cli_find_long_flag(*flags, long_name) == NULL,
           "Duplicate long flag --%s",
           long_name);

    *out_value = false;
    array_push(*flags,
               (CliFlag){.short_name  = short_name,
                         .long_name   = long_name,
                         .description = description ? description : "",
                         .out_value   = out_value});
}

internal void cli_print_flags_table(Array(CliFlag) flags, cstr title)
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
