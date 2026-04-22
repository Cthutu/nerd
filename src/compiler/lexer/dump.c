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
    case TK_EOF:
        return string_from_cstr("EOF");
    case TK_Integer:
        return string_from_cstr("Integer");
    case TK_String:
        return string_from_cstr("String");
    case TK_Symbol:
        return string_from_cstr("Symbol");
    case TK_Plus:
        return string_from_cstr("Plus `+`");
    case TK_Minus:
        return string_from_cstr("Minus `-`");
    case TK_Star:
        return string_from_cstr("Star `*`");
    case TK_Slash:
        return string_from_cstr("Slash `/`");
    case TK_Percent:
        return string_from_cstr("Percent `%`");
    case TK_LParen:
        return string_from_cstr("LeftParen `(`");
    case TK_RParen:
        return string_from_cstr("RightParen `)`");
    case TK_LBrace:
        return string_from_cstr("LeftBrace `{`");
    case TK_RBrace:
        return string_from_cstr("RightBrace `}`");
    case TK_Colon:
        return string_from_cstr("Colon `:`");
    case TK_Bang:
        return string_from_cstr("Bang `!`");
    case TK_FatArrow:
        return string_from_cstr("FatArrow `=>`");
    case TK_ThinArrow:
        return string_from_cstr("ThinArrow `->`");
    case TK_fn:
        return string_from_cstr("Keyword `fn`");
    case TK_return:
        return string_from_cstr("Keyword `return`");
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
    table_init(&table, columns, .title = "Lexer Tokens (4 bytes each)");
    array_free(columns);
    table_reserve_rows(&table, array_count(lexer->tokens));

    usize integer_index = 0;
    usize string_index  = 0;
    usize symbol_index  = 0;

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
        case TK_String:
            row[3] = table_cell_string(lexer->strings[string_index++]);
            break;
        case TK_Symbol:
            {
                string symbol =
                    lex_symbol(lexer, lexer->symbol_handles[symbol_index++]);
                row[3] = table_cell_string(symbol);
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
