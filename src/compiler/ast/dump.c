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
    case AK_FloatLiteral:
        return s("FloatLiteral");
    case AK_StringLiteral:
        return s("StringLiteral");
    case AK_BoolLiteral:
        return s("BoolLiteral");
    case AK_NilLiteral:
        return s("NilLiteral");
    case AK_StringConcat:
        return s("StringConcat");
    case AK_InterpPartExpr:
        return s("InterpPartExpr");
    case AK_InterpolatedString:
        return s("InterpolatedString");
    case AK_SymbolRef:
        return s("SymbolRef");
    case AK_LogicalNot:
        return s("LogicalNot");
    case AK_IntegerNegate:
        return s("IntegerNegate");
    case AK_AddressOf:
        return s("AddressOf");
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
    case AK_BitwiseAnd:
        return s("BitwiseAnd");
    case AK_BitwiseXor:
        return s("BitwiseXor");
    case AK_BitwiseOr:
        return s("BitwiseOr");
    case AK_Equal:
        return s("Equal");
    case AK_NotEqual:
        return s("NotEqual");
    case AK_Less:
        return s("Less");
    case AK_LessEqual:
        return s("LessEqual");
    case AK_Greater:
        return s("Greater");
    case AK_GreaterEqual:
        return s("GreaterEqual");
    case AK_LogicalAnd:
        return s("LogicalAnd");
    case AK_LogicalOr:
        return s("LogicalOr");
    case AK_Call:
        return s("Call");
    case AK_Cast:
        return s("Cast");
    case AK_Tuple:
        return s("Tuple");
    case AK_TupleField:
        return s("TupleField");
    case AK_Array:
        return s("Array");
    case AK_Index:
        return s("Index");
    case AK_Slice:
        return s("Slice");
    case AK_Field:
        return s("Field");
    case AK_RangeExclusive:
        return s("RangeExclusive");
    case AK_RangeInclusive:
        return s("RangeInclusive");
    case AK_On:
        return s("On");
    case AK_TypeFn:
        return s("TypeFn");
    case AK_TypeTuple:
        return s("TypeTuple");
    case AK_TypeArray:
        return s("TypeArray");
    case AK_TypeSlice:
        return s("TypeSlice");
    case AK_TypeDynamicArray:
        return s("TypeDynamicArray");
    case AK_TypePointer:
        return s("TypePointer");
    case AK_Expression:
        return s("Expression");
    case AK_Statement:
        return s("Statement");
    case AK_Return:
        return s("Return");
    case AK_Defer:
        return s("Defer");
    case AK_Assert:
        return s("Assert");
    case AK_ReturnExpr:
        return s("ReturnExpr");
    case AK_BreakExpr:
        return s("BreakExpr");
    case AK_ContinueExpr:
        return s("ContinueExpr");
    case AK_ExprBlock:
        return s("ExprBlock");
    case AK_Block:
        return s("Block");
    case AK_For:
        return s("For");
    case AK_Break:
        return s("Break");
    case AK_Continue:
        return s("Continue");
    case AK_Bind:
        return s("Bind");
    case AK_Variable:
        return s("Variable");
    case AK_Assign:
        return s("Assign");
    case AK_AnnotatedValue:
        return s("AnnotatedValue");
    case AK_ZeroInit:
        return s("ZeroInit");
    case AK_Undefined:
        return s("Undefined");
    case AK_FfiDef:
        return s("FfiDef");
    case AK_ModRef:
        return s("ModRef");
    case AK_Use:
        return s("Use");
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
        case AK_FloatLiteral:
            row[3] = table_cell_string(string_format(
                &temp_arena, "%.17g", ast_get_float(lexer, node)));
            break;
        case AK_StringLiteral:
            row[3] = table_cell_string(ast_get_string(lexer, node));
            break;
        case AK_BoolLiteral:
            row[3] = table_cell_string(node->a != 0 ? s("yes") : s("no"));
            break;
        case AK_NilLiteral:
            row[3] = table_cell_string(s("nil"));
            break;
        case AK_StringConcat:
            row[3] = table_cell_string(
                string_format(&temp_arena, "lhs=%u rhs=%u", node->a, node->b));
            break;
        case AK_InterpPartExpr:
            row[3] = table_cell_string(
                string_format(&temp_arena, "expr=%u", node->a));
            break;
        case AK_InterpolatedString:
            row[3] = table_cell_string(
                string_format(&temp_arena, "parts=%u..%u", node->a, node->b));
            break;
        case AK_SymbolRef:
            row[3] = table_cell_string(lex_symbol(lexer, node->a));
            break;
        case AK_LogicalNot:
        case AK_IntegerNegate:
            row[3] = table_cell_string(
                string_format(&temp_arena, "rhs=%u", node->a));
            break;
        case AK_IntegerPlus:
        case AK_IntegerMinus:
        case AK_IntegerMultiply:
        case AK_IntegerDivide:
        case AK_IntegerModulo:
        case AK_BitwiseAnd:
        case AK_BitwiseXor:
        case AK_BitwiseOr:
        case AK_Equal:
        case AK_NotEqual:
        case AK_Less:
        case AK_LessEqual:
        case AK_Greater:
        case AK_GreaterEqual:
        case AK_LogicalAnd:
        case AK_LogicalOr:
            row[3] = table_cell_string(
                string_format(&temp_arena, "lhs=%u rhs=%u", node->a, node->b));
            break;
        case AK_Call:
            {
                const AstCallInfo* call = &ast->calls[node->b];
                row[3]                  = table_cell_string(
                    string_format(&temp_arena,
                                  "callee=%u args=%u..%u",
                                  node->a,
                                  call->first_arg,
                                  call->first_arg + call->arg_count));
            }
            break;
        case AK_Cast:
            {
                const AstCastInfo* cast = &ast->casts[node->b];
                row[3] =
                    table_cell_string(string_format(&temp_arena,
                                                    "value=%u type=%u extra=%u",
                                                    node->a,
                                                    cast->type_node_index,
                                                    cast->extra_node_index));
            }
            break;
        case AK_AddressOf:
        case AK_TypePointer:
            row[3] = table_cell_string(
                string_format(&temp_arena, "value=%u", node->a));
            break;
        case AK_Tuple:
        case AK_TypeTuple:
            row[3] = table_cell_string(string_format(
                &temp_arena, "items=%u..%u", node->a, node->a + node->b));
            break;
        case AK_TupleField:
            row[3] = table_cell_string(string_format(
                &temp_arena, "value=%u field=%u", node->a, node->b));
            break;
        case AK_Array:
            row[3] = table_cell_string(string_format(
                &temp_arena, "items=%u..%u", node->a, node->a + node->b));
            break;
        case AK_Index:
            row[3] = table_cell_string(string_format(
                &temp_arena, "value=%u index=%u", node->a, node->b));
            break;
        case AK_Slice:
            row[3] = table_cell_string(
                string_format(&temp_arena, "slice=%u", node->a));
            break;
        case AK_Field:
            row[3] = table_cell_string(string_format(
                &temp_arena, "value=%u symbol=%u", node->a, node->b));
            break;
        case AK_TypeArray:
            row[3] = table_cell_string(string_format(
                &temp_arena, "length=%u type=%u", node->a, node->b));
            break;
        case AK_TypeSlice:
            row[3] = table_cell_string(
                string_format(&temp_arena, "type=%u", node->a));
            break;
        case AK_TypeDynamicArray:
            row[3] = table_cell_string(
                string_format(&temp_arena, "min=%u type=%u", node->a, node->b));
            break;
        case AK_RangeExclusive:
        case AK_RangeInclusive:
            row[3] = table_cell_string(string_format(
                &temp_arena, "start=%u end=%u", node->a, node->b));
            break;
        case AK_On:
            {
                const AstOnInfo* on = &ast->ons[node->b];
                row[3]              = table_cell_string(
                    string_format(&temp_arena,
                                  "scrutinee=%u kind=%u branches=%u..%u",
                                  node->a,
                                  on->kind,
                                  on->first_branch,
                                  on->first_branch + on->branch_count));
            }
            break;
        case AK_TypeFn:
            row[3] = table_cell_string(
                string_format(&temp_arena, "signature=%u", node->a));
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
        case AK_Defer:
        case AK_Assert:
        case AK_ReturnExpr:
            if (node->kind == AK_Assert) {
                row[3] = table_cell_string(
                    string_format(&temp_arena,
                                  "condition=%u message=%u",
                                  node->a,
                                  node->b));
            } else {
                row[3] = table_cell_string(
                    node->a == U32_MAX
                        ? s("expr=<none>")
                        : string_format(&temp_arena, "expr=%u", node->a));
            }
            break;
        case AK_Block:
            row[3] = table_cell_string(
                string_format(&temp_arena, "stmts=%u..%u", node->a, node->b));
            break;
        case AK_For:
            {
                const AstForInfo* for_info = &ast->fors[node->a];
                row[3]                     = table_cell_string(string_format(
                    &temp_arena,
                    "init=%u..%u condition=%u update=%u..%u body=%u else=%u",
                    for_info->first_init,
                    for_info->first_init + for_info->init_count,
                    for_info->condition_node_index,
                    for_info->first_update,
                    for_info->first_update + for_info->update_count,
                    node->b,
                    for_info->else_block_index));
            }
            break;
        case AK_Bind:
            row[3] = table_cell_string(
                string_format(&temp_arena,
                              "symbol=" STRINGP " value=%u",
                              STRINGV(lex_symbol(lexer, node->a)),
                              node->b));
            break;
        case AK_Variable:
            row[3] = table_cell_string(
                string_format(&temp_arena,
                              "symbol=" STRINGP " value=%u",
                              STRINGV(lex_symbol(lexer, node->a)),
                              node->b));
            break;
        case AK_Assign:
            row[3] = table_cell_string(string_format(
                &temp_arena, "target=%u value=%u", node->a, node->b));
            break;
        case AK_AnnotatedValue:
            row[3] = table_cell_string(string_format(
                &temp_arena, "type=%u value=%u", node->a, node->b));
            break;
        case AK_ZeroInit:
            row[3] = table_cell_string(
                string_format(&temp_arena, "type=%u", node->a));
            break;
        case AK_Undefined:
            row[3] = table_cell_string(
                string_format(&temp_arena, "type=%u", node->a));
            break;
        case AK_FnDef:
            row[3] = table_cell_string(
                string_format(&temp_arena,
                              "body=%u kind=%s",
                              node->a,
                              node->b == AFK_Block ? "block" : "expr"));
            break;
        case AK_FnStart:
            row[3] = table_cell_string(string_format(
                &temp_arena, "signature=%u end=%u", node->a, node->b));
            break;
        case AK_FnEnd:
            row[3] = table_cell_string(string_format(
                &temp_arena, "def=%u start=%u", node->a, node->b));
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
