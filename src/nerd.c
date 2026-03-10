//------------------------------------------------------------------------------
//> use: core intern compiler timing table cli

#include <cli/cli.h>
#include <compiler/compiler.h>
#include <table/table.h>

//------------------------------------------------------------------------------

internal NerdConfig parse_config(int argc, char** argv)
{
    //
    // SHow a table of all the arguments
    //

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

    //
    // Process command line arguments
    //

    NerdConfig config = {0};
    CliParser  parser = {0};
    cli_init(&parser, "nerd", "Nerd compiler playground");

    usize build_command =
        cli_add_command(&parser, "build", "Run one normal build");
    usize benchmark_command =
        cli_add_command(&parser, "benchmark", "Run 10000 benchmark iterations");
    usize million_command = cli_add_command(
        &parser, "million", "Generate 1,000,000 lines and build once");

    CliParseResult parse_result = cli_parse(&parser, argc, argv);
    if (parse_result.help_requested) {
        if (parse_result.command_index >= 0) {
            cli_print_command_help(&parser, (usize)parse_result.command_index);
        } else {
            cli_print_help(&parser);
        }
        cli_done(&parser);
        exit(0);
    }

    if (parse_result.command_index < 0) {
        cli_print_help(&parser);
        cli_done(&parser);
        kill("Missing command. Use --help to list commands.");
    }

    //
    // Handle the commands
    //

    usize command_index = (usize)parse_result.command_index;
    if (command_index == build_command) {
        config.command = NERD_COMMAND_BUILD;
    } else if (command_index == benchmark_command) {
        config.command = NERD_COMMAND_BENCHMARK;
    } else if (command_index == million_command) {
        config.command = NERD_COMMAND_MILLION;
    } else {
        cli_done(&parser);
        kill("Invalid command index: %zd", parse_result.command_index);
    }

    cli_done(&parser);
    return config;
}

//------------------------------------------------------------------------------
// Entry point for the test.

int run(int argc, char** argv)
{
    dump_info();

    NerdConfig config = parse_config(argc, argv);
    config.source     = s("42");

    switch (config.command) {
    case NERD_COMMAND_BUILD:
        prn("Command: build");
        return compiler_cmd_build(&config);
    case NERD_COMMAND_BENCHMARK:
        prn("Command: benchmark");
        return compiler_cmd_benchmark(&config);
    case NERD_COMMAND_MILLION:
        prn("Command: million");
        return compiler_cmd_million(&config);
    default:
        kill("Unhandled command");
    }

    return 1;
}
