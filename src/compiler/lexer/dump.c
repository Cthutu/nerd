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
    case TK_Float:
        return string_from_cstr("Float");
    case TK_String:
        return string_from_cstr("String");
    case TK_CString:
        return string_from_cstr("CString");
    case TK_InterpolatedStringStart:
        return string_from_cstr("InterpolatedStringStart `$\"`");
    case TK_InterpolatedStringEnd:
        return string_from_cstr("InterpolatedStringEnd `\"`");
    case TK_Symbol:
        return string_from_cstr("Symbol");
    case TK_Plus:
        return string_from_cstr("Plus `+`");
    case TK_PlusEqual:
        return string_from_cstr("PlusEqual `+=`");
    case TK_Minus:
        return string_from_cstr("Minus `-`");
    case TK_MinusEqual:
        return string_from_cstr("MinusEqual `-=`");
    case TK_Star:
        return string_from_cstr("Star `*`");
    case TK_StarEqual:
        return string_from_cstr("StarEqual `*=`");
    case TK_Slash:
        return string_from_cstr("Slash `/`");
    case TK_SlashEqual:
        return string_from_cstr("SlashEqual `/=`");
    case TK_Percent:
        return string_from_cstr("Percent `%`");
    case TK_PercentEqual:
        return string_from_cstr("PercentEqual `%=`");
    case TK_LParen:
        return string_from_cstr("LeftParen `(`");
    case TK_RParen:
        return string_from_cstr("RightParen `)`");
    case TK_LBracket:
        return string_from_cstr("LeftBracket `[`");
    case TK_RBracket:
        return string_from_cstr("RightBracket `]`");
    case TK_Comma:
        return string_from_cstr("Comma `,`");
    case TK_Semicolon:
        return string_from_cstr("Semicolon `;`");
    case TK_LBrace:
        return string_from_cstr("LeftBrace `{`");
    case TK_RBrace:
        return string_from_cstr("RightBrace `}`");
    case TK_Dot:
        return string_from_cstr("Dot `.`");
    case TK_At:
        return string_from_cstr("At `@`");
    case TK_Dollar:
        return string_from_cstr("Dollar `$`");
    case TK_Hash:
        return string_from_cstr("Hash `#`");
    case TK_Ellipsis:
        return string_from_cstr("Ellipsis `...`");
    case TK_Range:
        return string_from_cstr("Range `..`");
    case TK_RangeInclusive:
        return string_from_cstr("RangeInclusive `..=`");
    case TK_Colon:
        return string_from_cstr("Colon `:`");
    case TK_Equal:
        return string_from_cstr("Equal `=`");
    case TK_EqualEqual:
        return string_from_cstr("EqualEqual `==`");
    case TK_Bang:
        return string_from_cstr("Bang `!`");
    case TK_BangEqual:
        return string_from_cstr("BangEqual `!=`");
    case TK_Amp:
        return string_from_cstr("Amp `&`");
    case TK_AmpEqual:
        return string_from_cstr("AmpEqual `&=`");
    case TK_AmpAmp:
        return string_from_cstr("AmpAmp `&&`");
    case TK_AmpAmpEqual:
        return string_from_cstr("AmpAmpEqual `&&=`");
    case TK_Pipe:
        return string_from_cstr("Pipe `|`");
    case TK_PipeEqual:
        return string_from_cstr("PipeEqual `|=`");
    case TK_PipePipe:
        return string_from_cstr("PipePipe `||`");
    case TK_PipePipeEqual:
        return string_from_cstr("PipePipeEqual `||=`");
    case TK_Caret:
        return string_from_cstr("Caret `^`");
    case TK_CaretEqual:
        return string_from_cstr("CaretEqual `^=`");
    case TK_Less:
        return string_from_cstr("Less `<`");
    case TK_LessEqual:
        return string_from_cstr("LessEqual `<=`");
    case TK_Greater:
        return string_from_cstr("Greater `>`");
    case TK_GreaterEqual:
        return string_from_cstr("GreaterEqual `>=`");
    case TK_FatArrow:
        return string_from_cstr("FatArrow `=>`");
    case TK_ThinArrow:
        return string_from_cstr("ThinArrow `->`");
    case TK_fn:
        return string_from_cstr("Keyword `fn`");
    case TK_for:
        return string_from_cstr("Keyword `for`");
    case TK_on:
        return string_from_cstr("Keyword `on`");
    case TK_else:
        return string_from_cstr("Keyword `else`");
    case TK_break:
        return string_from_cstr("Keyword `break`");
    case TK_continue:
        return string_from_cstr("Keyword `continue`");
    case TK_return:
        return string_from_cstr("Keyword `return`");
    case TK_plex:
        return string_from_cstr("Keyword `plex`");
    case TK_union:
        return string_from_cstr("Keyword `union`");
    case TK_enum:
        return string_from_cstr("Keyword `enum`");
    case TK_ffi:
        return string_from_cstr("Keyword `ffi`");
    case TK_mod:
        return string_from_cstr("Keyword `mod`");
    case TK_use:
        return string_from_cstr("Keyword `use`");
    case TK_pub:
        return string_from_cstr("Keyword `pub`");
    case TK_with:
        return string_from_cstr("Keyword `with`");
    case TK_in:
        return string_from_cstr("Keyword `in`");
    case TK_as:
        return string_from_cstr("Keyword `as`");
    case TK_yes:
        return string_from_cstr("Keyword `yes`");
    case TK_no:
        return string_from_cstr("Keyword `no`");
    case TK_undefined:
        return string_from_cstr("Keyword `undefined`");
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
    usize float_index   = 0;
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
        case TK_Float:
            row[3] = table_cell_string(string_format(
                &temp_arena, "%.17g", lexer->floats[float_index++]));
            break;
        case TK_String:
        case TK_CString:
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
