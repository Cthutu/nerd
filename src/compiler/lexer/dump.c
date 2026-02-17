//------------------------------------------------------------------------------
// Lexer state dump (for debugging)
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/lexer/lexer.h>
#include <table/table.h>

//------------------------------------------------------------------------------

string token_kind_to_string(TokenKind kind)
{
    switch (kind) {
    case TK_Integer:
        return string_from_cstr("Integer");
    default:
        return string_from_cstr("Unknown");
    }
}

void lex_dump(const Lexer* lexer)
{
    Table table;
    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "Token", .colour = ANSI_YELLOW},
               (TableColumn){.title = "Offset", .colour = ANSI_CYAN},
               (TableColumn){.title = "Value", .colour = ANSI_GREEN});
    table_init(&table, columns);
    array_free(columns);

    Array(TableCell) cells = 0;
    array_requires_size(cells, 3);

    usize integer_index = 0;

    for (usize i = 0; i < array_count(lexer->tokens); i++) {
        Token token = lexer->tokens[i];
        cells[0]    = table_cell_text(token_kind_to_string(token.kind));
        cells[1]    = table_cell_u32(token.offset);

        switch (token.kind) {
        case TK_Number:
            {
                u64 value = lexer->integers[integer_index++];
                cells[2]  = table_cell_u64(value);
                break;
            }
        default:
            cells[2] = table_cell_text(s("-"));
            break;
        }
        table_add_row(&table, cells);
    }

    table_print(&table, ANSI_FAINT_WHITE, ANSI_BOLD_WHITE, ANSI_RESET);
    table_done(&table);

    array_free(cells);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
