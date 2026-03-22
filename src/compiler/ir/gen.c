//------------------------------------------------------------------------------
// IR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include "compiler/ir/ir.h"

//------------------------------------------------------------------------------

void ir_add_assign(Ir* ir, u64 lvalue_index, IrValue rvalue)
{
    IrInstruction instr = {
        .op     = IR_OP_ASSIGN,
        .lvalue = {.kind = IR_VALUE_VARIABLE, .value.integer = lvalue_index},
        .rvalue = {rvalue, {0}},
    };
    array_push(ir->instructions, instr);
}

void ir_add_return(Ir* ir, IrValue rvalue)
{
    IrInstruction instr = {
        .op     = IR_OP_RETURN,
        .lvalue = {0},
        .rvalue = {rvalue, {0}},
    };
    array_push(ir->instructions, instr);
}

void ir_add_unary(Ir* ir, IrOperation op, u64 lvalue_index, IrValue rhs)
{
    IrInstruction instr = {
        .op     = op,
        .lvalue = {.kind = IR_VALUE_VARIABLE, .value.integer = lvalue_index},
        .rvalue = {rhs, {0}},
    };
    array_push(ir->instructions, instr);
}

void ir_add_binary(
    Ir* ir, IrOperation op, u64 lvalue_index, IrValue lhs, IrValue rhs)
{
    IrInstruction instr = {
        .op     = op,
        .lvalue = {.kind = IR_VALUE_VARIABLE, .value.integer = lvalue_index},
        .rvalue = {lhs, rhs},
    };
    array_push(ir->instructions, instr);
}

Ir ir_generate(const Lexer* lex, const Ast* ast)
{
    Ir         ir          = {0};
    Array(u64) node_values = NULL;
    for (usize i = 0; i < array_count(ast->nodes); ++i) {
        array_push(node_values, 0);
    }

    for (usize i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        switch (node->kind) {
        case AK_IntegerLiteral:
            {
                u64 value_index = array_count(ir.instructions);
                ir_add_assign(
                    &ir,
                    value_index,
                    (IrValue){.kind          = IR_VALUE_INTEGER,
                              .value.integer = ast_get_integer(lex, node)});
                node_values[i] = value_index;
                break;
            }
        case AK_IntegerNegate:
            {
                u64 value_index = array_count(ir.instructions);
                ir_add_unary(&ir,
                             IR_OP_NEGATE,
                             value_index,
                             (IrValue){.kind          = IR_VALUE_VARIABLE,
                                       .value.integer = node_values[node->a]});
                node_values[i] = value_index;
                break;
            }
        case AK_IntegerPlus:
        case AK_IntegerMinus:
        case AK_IntegerMultiply:
        case AK_IntegerDivide:
        case AK_IntegerModulo:
            {
                IrOperation op = IR_OP_ADD;
                switch (node->kind) {
                case AK_IntegerPlus:
                    op = IR_OP_ADD;
                    break;
                case AK_IntegerMinus:
                    op = IR_OP_SUBTRACT;
                    break;
                case AK_IntegerMultiply:
                    op = IR_OP_MULTIPLY;
                    break;
                case AK_IntegerDivide:
                    op = IR_OP_DIVIDE;
                    break;
                case AK_IntegerModulo:
                    op = IR_OP_MODULO;
                    break;
                default:
                    break;
                }

                u64 value_index = array_count(ir.instructions);
                ir_add_binary(
                    &ir,
                    op,
                    value_index,
                    (IrValue){.kind          = IR_VALUE_VARIABLE,
                              .value.integer = node_values[node->a]},
                    (IrValue){.kind          = IR_VALUE_VARIABLE,
                              .value.integer = node_values[node->b]});
                node_values[i] = value_index;
                break;
            }
        case AK_Expression:
            {
                node_values[i] = node_values[node->a];
                break;
            }
        }
    }

    if (array_count(ast->nodes) != 0) {
        const AstNode* root = &ast->nodes[array_count(ast->nodes) - 1];
        if (root->kind == AK_Expression) {
            ir_add_return(&ir,
                          (IrValue){.kind          = IR_VALUE_VARIABLE,
                                    .value.integer =
                                        node_values[array_count(ast->nodes) - 1]});
        }
    }

    array_free(node_values);
    return ir;
}

void ir_done(Ir* ir) { array_free(ir->instructions); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
