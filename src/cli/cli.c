//------------------------------------------------------------------------------
// Command-line parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <cli/cli.h>
#include <table/table.h>

internal CliFlag* cli_find_long_flag(CliParser* parser, cstr long_name)
{
    for (usize i = 0; i < array_count(parser->flags); i++) {
        if (strcmp(parser->flags[i].long_name, long_name) == 0) {
            return &parser->flags[i];
        }
    }
    return NULL;
}

internal CliFlag* cli_find_short_flag(CliParser* parser, char short_name)
{
    for (usize i = 0; i < array_count(parser->flags); i++) {
        if (parser->flags[i].short_name == short_name) {
            return &parser->flags[i];
        }
    }
    return NULL;
}

void cli_init(CliParser* parser, cstr program_name, cstr summary)
{
    *parser              = (CliParser){0};
    parser->program_name = program_name;
    parser->summary      = summary;
}

void cli_done(CliParser* parser)
{
    array_free(parser->flags);
    *parser = (CliParser){0};
}

void cli_add_flag(CliParser* parser,
                  char       short_name,
                  cstr       long_name,
                  cstr       description,
                  bool*      out_value)
{
    ASSERT(long_name != NULL && long_name[0] != '\0', "Flag must have long name");
    ASSERT(out_value != NULL, "Flag output pointer cannot be NULL");

    if (short_name != '\0') {
        ASSERT(cli_find_short_flag(parser, short_name) == NULL,
               "Duplicate short flag -%c",
               short_name);
    }
    ASSERT(cli_find_long_flag(parser, long_name) == NULL,
           "Duplicate long flag --%s",
           long_name);

    *out_value = false;
    array_push(parser->flags,
               (CliFlag){.short_name  = short_name,
                         .long_name   = long_name,
                         .description = description ? description : "",
                         .out_value   = out_value});
}

CliParseResult cli_parse(CliParser* parser, int argc, char** argv)
{
    CliParseResult result = {0};

    for (int i = 1; i < argc; i++) {
        cstr arg = argv[i];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            result.help_requested = true;
            continue;
        }

        if (arg[0] == '-' && arg[1] == '-') {
            cstr name = arg + 2;
            CliFlag* flag = cli_find_long_flag(parser, name);
            if (!flag) {
                kill("Unknown flag '%s'. Use --help to list options.", arg);
            }
            *flag->out_value = true;
            continue;
        }

        if (arg[0] == '-' && arg[1] != '\0' && arg[2] == '\0') {
            char     name = arg[1];
            CliFlag* flag = cli_find_short_flag(parser, name);
            if (!flag) {
                kill("Unknown flag '%s'. Use --help to list options.", arg);
            }
            *flag->out_value = true;
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
    prn("  %s [options]", program_name);

    if (parser->summary && parser->summary[0] != '\0') {
        prn("");
    }

    prn("%sOptions%s", ANSI_BOLD_YELLOW, ANSI_RESET);

    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "Option", .colour = ANSI_BOLD_CYAN},
               (TableColumn){.title = "Description", .colour = ANSI_BOLD_GREEN});

    Table table = {0};
    table_init(&table, columns);
    array_free(columns);
    table_reserve_rows(&table, array_count(parser->flags) + 1);

    Arena arena = {0};
    arena_init(&arena);

    {
        TableCell row[] = {table_cell_text(s("-h, --help")),
                           table_cell_text(s("Show this help message"))};
        table_add_row(&table, row);
    }

    for (usize i = 0; i < array_count(parser->flags); i++) {
        const CliFlag* flag       = &parser->flags[i];
        string         option_str = {0};
        if (flag->short_name != '\0') {
            option_str = string_format(
                &arena, "-%c, --%s", flag->short_name, flag->long_name);
        } else {
            option_str = string_format(&arena, "--%s", flag->long_name);
        }

        TableCell row[] = {table_cell_text(option_str),
                           table_cell_text(s(flag->description))};
        table_add_row(&table, row);
    }

    table_print(&table, ANSI_FAINT_WHITE, ANSI_BOLD_WHITE, ANSI_RESET);
    table_done(&table);
    arena_done(&arena);
}

//------------------------------------------------------------------------------
