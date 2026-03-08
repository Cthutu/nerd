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

Ir ir_generate(const Lexer* lex, const Ast* ast)
{
    Ir  ir    = {0};
    u64 index = 0;

    for (usize i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        switch (node->kind) {
        case AK_IntegerLiteral:
            {
                ir_add_assign(
                    &ir,
                    index++,
                    (IrValue){.kind          = IR_VALUE_INTEGER,
                              .value.integer = ast_get_integer(lex, node)});
                break;
            }
        }
    }

    ir_add_return(
        &ir, (IrValue){.kind = IR_VALUE_VARIABLE, .value.integer = index - 1});
    return ir;
}

void ir_done(Ir* ir) { array_free(ir->instructions); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
