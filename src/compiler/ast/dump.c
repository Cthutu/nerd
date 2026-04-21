//------------------------------------------------------------------------------
// AST debug dump
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/ast.h>
#include <table/table.h>

//------------------------------------------------------------------------------

string ast_kind_to_string(AstKind kind)
{
    switch (kind) {
    case AK_IntegerLiteral:
        return s("IntegerLiteral");
    case AK_StringLiteral:
        return s("StringLiteral");
    case AK_StringConcat:
        return s("StringConcat");
    case AK_SymbolRef:
        return s("SymbolRef");
    case AK_IntegerNegate:
        return s("IntegerNegate");
    case AK_IntegerPlus:
        return s("IntegerPlus");
    case AK_IntegerMinus:
        return s("IntegerMinus");
    case AK_IntegerMultiply:
        return s("IntegerMultiply");
    case AK_IntegerDivide:
        return s("IntegerDivide");
    case AK_IntegerModulo:
        return s("IntegerModulo");
    case AK_Call:
        return s("Call");
    case AK_Expression:
        return s("Expression");
    case AK_Statement:
        return s("Statement");
    case AK_Return:
        return s("Return");
    case AK_Bind:
        return s("Bind");
    case AK_FnDef:
        return s("FnDef");
    case AK_FnStart:
        return s("FnStart");
    case AK_FnEnd:
        return s("FnEnd");
    default:
        return s("Unknown");
    }
}

//------------------------------------------------------------------------------
// Render the AST node table for debugging.

void ast_dump(const Ast* ast, const Lexer* lexer)
{
    cstr index_colour          = "\x1b[38;5;214m";
    cstr kind_colour           = "\x1b[38;5;111m";
    cstr token_colour          = "\x1b[38;5;117m";
    cstr value_colour          = "\x1b[38;5;82m";

    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "Index", .colour = index_colour},
               (TableColumn){.title = "Kind", .colour = kind_colour},
               (TableColumn){.title = "Token", .colour = token_colour},
               (TableColumn){.title = "Value", .colour = value_colour});

    Table table = {0};
    table_init(&table, columns, .title = "AST Nodes (16 bytes each)");
    table_reserve_rows(&table, array_count(ast->nodes));
    array_free(columns);

    StringBuilder sb = {0};

    for (usize i = 0; i < array_count(ast->nodes); i++) {
        TableCell row[4];
        row[0] = table_cell_u32((u32)i);
        row[1] = table_cell_string(ast_kind_to_string(ast->nodes[i].kind));

        AstNode* node  = &ast->nodes[i];
        Token*   token = &lexer->tokens[node->token_index];
        sb_init(&sb, &temp_arena);
        sb_format(&sb,
                  "%u: " STRINGP "@%u",
                  node->token_index,
                  STRINGV(token_kind_to_string(token->kind)),
                  token->offset);
        row[2] = table_cell_string(sb_to_string(&sb));

        switch (node->kind) {
        case AK_IntegerLiteral:
            row[3] = table_cell_u64(lexer->integers[node->a]);
            break;
        case AK_StringLiteral:
            row[3] = table_cell_string(ast_get_string(lexer, node));
            break;
        case AK_StringConcat:
            row[3] = table_cell_string(
                string_format(&temp_arena, "lhs=%u rhs=%u", node->a, node->b));
            break;
        case AK_SymbolRef:
            row[3] = table_cell_string(lex_symbol(lexer, node->a));
            break;
        case AK_IntegerNegate:
            row[3] = table_cell_string(
                string_format(&temp_arena, "rhs=%u", node->a));
            break;
        case AK_IntegerPlus:
        case AK_IntegerMinus:
        case AK_IntegerMultiply:
        case AK_IntegerDivide:
        case AK_IntegerModulo:
            row[3] = table_cell_string(
                string_format(&temp_arena, "lhs=%u rhs=%u", node->a, node->b));
            break;
        case AK_Call:
            row[3] = table_cell_string(string_format(
                &temp_arena, "callee=%u arg=%u", node->a, node->b));
            break;
        case AK_Expression:
            row[3] = table_cell_string(
                string_format(&temp_arena, "root=%u", node->a));
            break;
        case AK_Statement:
            row[3] = table_cell_string(
                string_format(&temp_arena, "expr=%u", node->a));
            break;
        case AK_Return:
            row[3] = table_cell_string(
                string_format(&temp_arena, "expr=%u", node->a));
            break;
        case AK_Bind:
            row[3] = table_cell_string(
                string_format(&temp_arena,
                              "symbol=" STRINGP " value=%u",
                              STRINGV(lex_symbol(lexer, node->a)),
                              node->b));
            break;
        case AK_FnDef:
            row[3] = table_cell_string(
                string_format(&temp_arena,
                              "body=%u kind=%s",
                              node->a,
                              node->b == AFK_Block ? "block" : "expr"));
            break;
        case AK_FnStart:
        case AK_FnEnd:
            row[3] = table_cell_string(
                string_format(&temp_arena, "a=%u b=%u", node->a, node->b));
            break;
        default:
            row[3] = table_cell_string(s("Unknown"));
            break;
        }
        table_add_row(&table, row);
    }

    table_print(&table);

    table_done(&table);
}
