//------------------------------------------------------------------------------
// IR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include "compiler/ir/ir.h"
#include <compiler/error/error.h>

//------------------------------------------------------------------------------
// Append a global declaration marker to the IR stream.

void ir_add_global(Ir* ir, u32 symbol_handle)
{
    IrInstruction instr = {
        .op     = IR_OP_GLOBAL,
        .lvalue = {.kind = IR_VALUE_SYMBOL, .value.integer = symbol_handle},
        .rvalue = {{0}, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append the generated init-function start marker.

void ir_add_init_start(Ir* ir)
{
    IrInstruction instr = {
        .op     = IR_OP_INIT_START,
        .lvalue = {0},
        .rvalue = {{0}, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append the generated init-function end marker.

void ir_add_init_end(Ir* ir)
{
    IrInstruction instr = {
        .op     = IR_OP_INIT_END,
        .lvalue = {0},
        .rvalue = {{0}, {0}},
    };
    array_push(ir->instructions, instr);
}

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

void ir_add_assign(Ir* ir, IrValue lvalue, IrValue rvalue)
{
    IrInstruction instr = {
        .op     = IR_OP_ASSIGN,
        .lvalue = lvalue,
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

void ir_add_unary(Ir* ir, IrOperation op, IrValue lvalue, IrValue rhs)
{
    IrInstruction instr = {
        .op     = op,
        .lvalue = lvalue,
        .rvalue = {rhs, {0}},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Append a binary instruction to the IR stream.

void ir_add_binary(
    Ir* ir, IrOperation op, IrValue lvalue, IrValue lhs, IrValue rhs)
{
    IrInstruction instr = {
        .op     = op,
        .lvalue = lvalue,
        .rvalue = {lhs, rhs},
    };
    array_push(ir->instructions, instr);
}

//------------------------------------------------------------------------------
// Return the sentinel IR value used for nodes not yet lowered.

internal IrValue ir_unset_value(void)
{
    return (IrValue){.kind = IR_VALUE_NONE};
}

//------------------------------------------------------------------------------
// Allocate a fresh node-value table for one lowered body.

internal Array(IrValue) ir_make_node_values(const Ast* ast)
{
    Array(IrValue) node_values = NULL;
    for (usize i = 0; i < array_count(ast->nodes); ++i) {
        array_push(node_values, ir_unset_value());
    }
    return node_values;
}

//------------------------------------------------------------------------------
// Lower a single AST expression node recursively into IR values.

internal IrValue ir_lower_node(const Lexer* lex,
                               const Ast*   ast,
                               const Sema*  sema,
                               u32          node_index,
                               Array(IrValue) node_values,
                               u64* next_value_index,
                               Ir*  ir)
{
    if (node_values[node_index].kind != IR_VALUE_NONE) {
        return node_values[node_index];
    }

    if (sema->node_const_known[node_index]) {
        IrValue value = {
            .kind          = IR_VALUE_INTEGER,
            .value.integer = sema->node_const_values[node_index],
        };
        node_values[node_index] = value;
        return value;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 decl_index = sema->node_decl_indices[node_index];
            ASSERT(decl_index != U32_MAX, "Expected resolved symbol reference");
            const SemaDecl* decl = &sema->decls[decl_index];
            ASSERT(decl->kind == SK_Constant,
                   "Only constant bindings can be referenced as expression "
                   "values");

            IrValue value = {
                .kind          = IR_VALUE_SYMBOL,
                .value.integer = decl->symbol_handle,
            };
            node_values[node_index] = value;
            return value;
        }
    case AK_IntegerNegate:
        {
            IrValue rhs = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_unary(ir, IR_OP_NEGATE, value, rhs);
            node_values[node_index] = value;
            return value;
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

            IrValue lhs = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            IrValue rhs = ir_lower_node(
                lex, ast, sema, node->b, node_values, next_value_index, ir);
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_binary(ir, op, value, lhs, rhs);
            node_values[node_index] = value;
            return value;
        }
    case AK_Expression:
        {
            IrValue value = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            node_values[node_index] = value;
            return value;
        }
    default:
        error_ice("Unhandled AST node kind during IR lowering: %u", node->kind);
        return (IrValue){0};
    }
}

//------------------------------------------------------------------------------
// Lower one top-level constant binding into the generated init function.

internal void ir_generate_global_init(const Lexer*    lex,
                                      const Ast*      ast,
                                      const Sema*     sema,
                                      const SemaDecl* decl,
                                      u64*            next_value_index,
                                      Ir*             ir)
{
    Array(IrValue) node_values = ir_make_node_values(ast);
    IrValue result             = ir_lower_node(lex,
                                   ast,
                                   sema,
                                   decl->value_node_index,
                                   node_values,
                                   next_value_index,
                                   ir);
    ir_add_assign(ir,
                  (IrValue){.kind          = IR_VALUE_SYMBOL,
                            .value.integer = decl->symbol_handle},
                  result);
    array_free(node_values);
}

//------------------------------------------------------------------------------
// Lower a bound function definition into function-scoped IR.

internal void ir_generate_function(const Lexer*    lex,
                                   const Ast*      ast,
                                   const Sema*     sema,
                                   const SemaDecl* decl,
                                   Ir*             ir)
{
    const AstNode* bind_node     = &ast->nodes[decl->bind_node_index];
    const AstNode* fn_def_node   = &ast->nodes[bind_node->b];
    const AstNode* fn_start_node = &ast->nodes[fn_def_node->a];
    usize          fn_end        = fn_start_node->b;

    ASSERT(fn_def_node->kind == AK_FnDef, "Expected function definition");
    ASSERT(fn_start_node->kind == AK_FnStart, "Expected function start");
    ASSERT(fn_end > fn_def_node->a, "Expected non-empty function range");

    ir_add_fn_start(ir, ast_get_symbol(bind_node));

    Array(IrValue) node_values = ir_make_node_values(ast);
    u64 next_value_index       = 0;
    ASSERT(fn_end > 0, "Expected function body expression");
    ASSERT(ast->nodes[fn_end - 1].kind == AK_Expression,
           "Expected expression node before function end");

    IrValue result = ir_lower_node(
        lex, ast, sema, fn_end - 1, node_values, &next_value_index, ir);
    ir_add_return(ir, result);
    ir_add_fn_end(ir);

    array_free(node_values);
}

//------------------------------------------------------------------------------
// Generate IR from ordered semantic declarations.

Ir ir_generate(const Lexer* lex, const Ast* ast, const Sema* sema)
{
    Ir   ir            = {0};
    bool has_constants = false;

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        if (decl->kind == SK_Constant) {
            has_constants = true;
            ir_add_global(&ir, decl->symbol_handle);
        }
    }

    if (has_constants) {
        u64 next_global_value_index = 0;

        ir_add_init_start(&ir);
        for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
            const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
            if (decl->kind == SK_Constant) {
                ir_generate_global_init(
                    lex, ast, sema, decl, &next_global_value_index, &ir);
            }
        }
        ir_add_init_end(&ir);
    }

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        if (decl->kind == SK_Function) {
            ir_generate_function(lex, ast, sema, decl, &ir);
        }
    }
    return ir;
}

//------------------------------------------------------------------------------
// Free the IR instruction table.

void ir_done(Ir* ir) { array_free(ir->instructions); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
