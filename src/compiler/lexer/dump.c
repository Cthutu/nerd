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
               (TableColumn){.title = "Index", .colour = ANSI_MAGENTA},
               (TableColumn){.title = "Token", .colour = ANSI_YELLOW},
               (TableColumn){.title = "Offset", .colour = ANSI_CYAN},
               (TableColumn){.title = "Value", .colour = ANSI_GREEN});
    table_init(&table, columns);
    table_set_title(&table, "Lexer Tokens (4 bytes each)");
    array_free(columns);
    table_reserve_rows(&table, array_count(lexer->tokens));

    usize integer_index = 0;

    for (usize i = 0; i < array_count(lexer->tokens); i++) {
        Token     token = lexer->tokens[i];
        TableCell row[4];
        row[0] = table_cell_u32((u32)i);
        row[1] = table_cell_string(token_kind_to_string(token.kind));
        row[2] = table_cell_u32(token.offset);

        switch (token.kind) {
        case TK_Integer:
            {
                u64 value = lexer->integers[integer_index++];
                row[3]    = table_cell_u64(value);
                break;
            }
        default:
            row[3] = table_cell_string(s("-"));
            break;
        }
        table_add_row(&table, row);
    }

    table_print(&table);
    table_done(&table);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
