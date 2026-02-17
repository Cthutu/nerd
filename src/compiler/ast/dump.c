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
    default:
        return s("Unknown");
    }
}

void ast_dump(const Ast* ast, const Lexer* lexer)
{
    cstr index_colour          = "\x1b[38;5;214m";
    cstr kind_colour           = "\x1b[38;5;111m";
    cstr ref_colour            = "\x1b[38;5;178m";
    cstr token_colour          = "\x1b[38;5;117m";
    cstr value_colour          = "\x1b[38;5;82m";

    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "Index", .colour = index_colour},
               (TableColumn){.title = "Kind", .colour = kind_colour},
               (TableColumn){.title = "Ref", .colour = ref_colour},
               (TableColumn){.title = "Token", .colour = token_colour},
               (TableColumn){.title = "Value", .colour = value_colour});

    Table table = {0};
    table_init(&table, columns);
    table_set_title(&table, "AST Nodes");
    table_reserve_rows(&table, array_count(ast->nodes));
    array_free(columns);

    Arena         string_arena = {0};
    StringBuilder sb           = {0};

    arena_init(&string_arena);

    for (usize i = 0; i < array_count(ast->nodes); i++) {
        TableCell row[5];
        row[0] = table_cell_u32((u32)i);
        row[1] = table_cell_string(ast_kind_to_string(ast->nodes[i].kind));
        row[2] = table_cell_u32(ast->nodes[i].ref);

        sb_init(&sb, &string_arena);
        sb_format(&sb,
                  "%.*s@%u",
                  STRINGV(token_kind_to_string(ast->nodes[i].token.kind)),
                  ast->nodes[i].token.offset);
        row[3] = table_cell_string(sb_to_string(&sb));

        switch (ast->nodes[i].kind) {
        case AK_IntegerLiteral:
            row[4] = table_cell_u64(lexer->integers[ast->nodes[i].a]);
            break;
        default:
            row[4] = table_cell_string(s("Unknown"));
            break;
        }
        table_add_row(&table, row);
    }

    table_print(&table);

    arena_done(&string_arena);
    table_done(&table);
}
