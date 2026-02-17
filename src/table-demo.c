//------------------------------------------------------------------------------
// Table rendering demo
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core table

#include <table/table.h>

//------------------------------------------------------------------------------

int run(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    Table table;
    Array(TableColumn) columns = NULL;
    array_push(columns,
               ((TableColumn){.title = "Stage", .colour = ANSI_CYAN}),
               ((TableColumn){.title = "Token", .colour = ANSI_YELLOW}),
               ((TableColumn){.title = "Count", .colour = ANSI_GREEN}),
               ((TableColumn){.title = "Bytes", .colour = ANSI_BLUE}),
               ((TableColumn){.title = "Delta", .colour = ANSI_MAGENTA}));

    table_init(&table, columns);
    array_free(columns);
    table_reserve_rows(&table, 8);

    {
        TableCell row[] = {
            table_cell_string(s("Lex")),
            table_cell_string(s("Ident@0")),
            table_cell_u32(42),
            table_cell_u64(1536),
            table_cell_i32(+3),
        };
        table_add_row(&table, row);
    }

    {
        TableCell row[] = {
            table_cell_string(s("Lex")),
            table_cell_string(s("Number@126")),
            table_cell_u32(17),
            table_cell_u64(768),
            table_cell_i32(-2),
        };
        table_add_row(&table, row);
    }

    {
        TableCell row[] = {
            table_cell_string(s("Parse")),
            table_cell_string(s("FnDecl@204")),
            table_cell_u32(9),
            table_cell_u64(4096),
            table_cell_i32(+1),
        };
        cstr colours[] = {ANSI_BOLD_CYAN,
                          ANSI_BOLD_YELLOW,
                          ANSI_BOLD_GREEN,
                          ANSI_BOLD_BLUE,
                          ANSI_BOLD_MAGENTA};
        table_add_row(&table, row, .colours = colours, .divider_before = true);
    }

    {
        TableCell row[] = {
            table_cell_string(s("Typecheck")),
            table_cell_empty(),
            table_cell_u32(9),
            table_cell_u64(5120),
            table_cell_i32(0),
        };
        table_add_row(&table, row);
    }

    {
        TableCell row[] = {
            table_cell_string(s("Codegen")),
            table_cell_string(s("Mov@812")),
            table_cell_u32(128),
            table_cell_u64(12288),
            table_cell_i32(+6),
        };
        cstr colours[] = {"", ANSI_RED, "", "", ANSI_GREEN};
        table_add_row(&table, row, .colours = colours);
    }

    prn("%sCompiler Pipeline Summary%s", ANSI_BOLD_WHITE, ANSI_RESET);
    table_print(&table, ANSI_FAINT_WHITE, ANSI_BOLD_WHITE, ANSI_RESET);
    table_done(&table);

    return 0;
}
