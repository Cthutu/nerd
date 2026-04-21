//------------------------------------------------------------------------------
// IR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include "compiler/ir/ir.h"
#include <compiler/error/error.h>

//------------------------------------------------------------------------------
// Append a function start marker to the IR stream.

void ir_add_fn_start(Ir* ir, u32 symbol_handle)
{
    IrInstruction instr = {
        .op     = IR_OP_FN_START,
        .lvalue = {.kind = IR_VALUE_SYMBOL, .value.integer = symbol_handle},
        .rvalue = {{0}, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append a function end marker to the IR stream.

void ir_add_fn_end(Ir* ir)
{
    IrInstruction instr = {
        .op     = IR_OP_FN_END,
        .lvalue = {0},
        .rvalue = {{0}, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append an assignment instruction to the IR stream.

void ir_add_assign(Ir* ir, u64 lvalue_index, IrValue rvalue)
{
    IrInstruction instr = {
        .op     = IR_OP_ASSIGN,
        .lvalue = {.kind = IR_VALUE_VARIABLE, .value.integer = lvalue_index},
        .rvalue = {rvalue, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append a return instruction to the IR stream.

void ir_add_return(Ir* ir, IrValue rvalue)
{
    IrInstruction instr = {
        .op     = IR_OP_RETURN,
        .lvalue = {0},
        .rvalue = {rvalue, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append a unary instruction to the IR stream.

void ir_add_unary(Ir* ir, IrOperation op, u64 lvalue_index, IrValue rhs)
{
    IrInstruction instr = {
        .op     = op,
        .lvalue = {.kind = IR_VALUE_VARIABLE, .value.integer = lvalue_index},
        .rvalue = {rhs, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append a binary instruction to the IR stream.

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

//------------------------------------------------------------------------------
// Return the sentinel value index used for nodes not yet lowered.

internal u64 ir_unset_value(void) { return U64_MAX; }

//------------------------------------------------------------------------------
// Lower a single AST expression node recursively into IR.

internal u64 ir_lower_node(const Lexer* lex,
                           const Ast*   ast,
                           const Sema*  sema,
                           u32          node_index,
                           Array(u64) node_values,
                           u64* next_value_index,
                           Ir*  ir)
{
    if (node_values[node_index] != ir_unset_value()) {
        return node_values[node_index];
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_IntegerLiteral:
        {
            u64 value_index = (*next_value_index)++;
            ir_add_assign(
                ir,
                value_index,
                (IrValue){.kind          = IR_VALUE_INTEGER,
                          .value.integer = ast_get_integer(lex, node)});
            node_values[node_index] = value_index;
            return value_index;
        }
    case AK_SymbolRef:
        {
            u32 decl_index = sema->node_decl_indices[node_index];
            ASSERT(decl_index != U32_MAX, "Expected resolved symbol reference");
            u32 value_node_index    = sema->decls[decl_index].value_node_index;

            // Resolve constant bindings by lowering their bound value on
            // demand.
            u64 value_index         = ir_lower_node(lex,
                                            ast,
                                            sema,
                                            value_node_index,
                                            node_values,
                                            next_value_index,
                                            ir);
            node_values[node_index] = value_index;
            return value_index;
        }
    case AK_IntegerNegate:
        {
            u64 rhs = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            u64 value_index = (*next_value_index)++;
            ir_add_unary(
                ir,
                IR_OP_NEGATE,
                value_index,
                (IrValue){.kind = IR_VALUE_VARIABLE, .value.integer = rhs});
            node_values[node_index] = value_index;
            return value_index;
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

            u64 lhs = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            u64 rhs = ir_lower_node(
                lex, ast, sema, node->b, node_values, next_value_index, ir);
            u64 value_index = (*next_value_index)++;
            ir_add_binary(
                ir,
                op,
                value_index,
                (IrValue){.kind = IR_VALUE_VARIABLE, .value.integer = lhs},
                (IrValue){.kind = IR_VALUE_VARIABLE, .value.integer = rhs});
            node_values[node_index] = value_index;
            return value_index;
        }
    case AK_Expression:
        {
            u64 value_index = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            node_values[node_index] = value_index;
            return value_index;
        }
    default:
        error_ice("Unhandled AST node kind during IR lowering: %u", node->kind);
        return 0;
    }
}

//------------------------------------------------------------------------------
// Lower a bound function definition into function-scoped IR.

internal void ir_generate_function(const Lexer*   lex,
                                   const Ast*     ast,
                                   const Sema*    sema,
                                   const AstNode* bind_node,
                                   Ir*            ir)
{
    const AstNode* fn_def_node   = &ast->nodes[bind_node->b];
    const AstNode* fn_start_node = &ast->nodes[fn_def_node->a];
    usize          fn_end        = fn_start_node->b;

    ASSERT(fn_def_node->kind == AK_FnDef, "Expected function definition");
    ASSERT(fn_start_node->kind == AK_FnStart, "Expected function start");
    ASSERT(fn_end > fn_def_node->a, "Expected non-empty function range");

    ir_add_fn_start(ir, ast_get_symbol(bind_node));

    Array(u64) node_values = NULL;
    for (usize i = 0; i < array_count(ast->nodes); ++i) {
        array_push(node_values, ir_unset_value());
    }

    u64 next_value_index = 0;
    ASSERT(fn_end > 0, "Expected function body expression");
    ASSERT(ast->nodes[fn_end - 1].kind == AK_Expression,
           "Expected expression node before function end");

    u64 result_index = ir_lower_node(
        lex, ast, sema, fn_end - 1, node_values, &next_value_index, ir);
    ir_add_return(
        ir,
        (IrValue){.kind = IR_VALUE_VARIABLE, .value.integer = result_index});
    ir_add_fn_end(ir);

    array_free(node_values);
}

//------------------------------------------------------------------------------
// Generate IR by lowering top-level function bindings.

Ir ir_generate(const Lexer* lex, const Ast* ast, const Sema* sema)
{
    Ir ir = {0};

    for (usize i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Bind && ast->nodes[node->b].kind == AK_FnDef) {
            ir_generate_function(lex, ast, sema, node, &ir);
        }
    }
    return ir;
}

//------------------------------------------------------------------------------
// Free the IR instruction table.

void ir_done(Ir* ir) { array_free(ir->instructions); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
