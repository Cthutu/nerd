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
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_GLOBAL,
            .lvalue = {.kind = IR_VALUE_SYMBOL, .value.integer = symbol_handle},
        });
}

//------------------------------------------------------------------------------
// Append the generated init-function start marker.

void ir_add_init_start(Ir* ir)
{
    array_push(ir->instructions, (IrInstruction){.op = IR_OP_INIT_START});
}

//------------------------------------------------------------------------------
// Append the generated init-function end marker.

void ir_add_init_end(Ir* ir)
{
    array_push(ir->instructions, (IrInstruction){.op = IR_OP_INIT_END});
}

//------------------------------------------------------------------------------
// Append a function start marker to the IR stream.

void ir_add_fn_start(Ir* ir, u32 symbol_handle)
{
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_FN_START,
            .lvalue = {.kind = IR_VALUE_SYMBOL, .value.integer = symbol_handle},
        });
}

//------------------------------------------------------------------------------
// Append a function end marker to the IR stream.

void ir_add_fn_end(Ir* ir)
{
    array_push(ir->instructions, (IrInstruction){.op = IR_OP_FN_END});
}

void ir_add_local(Ir* ir, u32 symbol_handle, IrValue rvalue)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_LOCAL,
                   .lvalue = {.kind = IR_VALUE_LOCAL,
                              .value.integer = symbol_handle},
                   .rvalue = {rvalue, {0}},
               });
}

//------------------------------------------------------------------------------
// Append an assignment instruction to the IR stream.

void ir_add_assign(Ir* ir, IrValue lvalue, IrValue rvalue)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_ASSIGN,
                   .lvalue = lvalue,
                   .rvalue = {rvalue, {0}},
               });
}

//------------------------------------------------------------------------------
// Append a call instruction to the IR stream.

void ir_add_call(Ir* ir, IrValue callee, IrValue arg)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_CALL,
                   .rvalue = {callee, arg},
               });
}

//------------------------------------------------------------------------------
// Append an explicit cast instruction to the IR stream.

void ir_add_cast(Ir* ir, IrValue lvalue, IrValue value, u32 type_index)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_CAST,
                   .lvalue = lvalue,
                   .rvalue = {value, {.kind = IR_VALUE_INTEGER,
                                      .value.integer = type_index}},
               });
}

//------------------------------------------------------------------------------
// Append a return instruction to the IR stream.

void ir_add_return(Ir* ir, IrValue rvalue)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_RETURN,
                   .rvalue = {rvalue, {0}},
               });
}

//------------------------------------------------------------------------------
// Append a unary instruction to the IR stream.

void ir_add_unary(Ir* ir, IrOperation op, IrValue lvalue, IrValue rhs)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = op,
                   .lvalue = lvalue,
                   .rvalue = {rhs, {0}},
               });
}

//------------------------------------------------------------------------------
// Append a binary instruction to the IR stream.

void ir_add_binary(
    Ir* ir, IrOperation op, IrValue lvalue, IrValue lhs, IrValue rhs)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = op,
                   .lvalue = lvalue,
                   .rvalue = {lhs, rhs},
               });
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
// Intern one string literal slice into the IR string pool.

internal u32 ir_add_string_literal(Ir* ir, string text)
{
    for (u32 i = 0; i < array_count(ir->strings); ++i) {
        if (string_eq(ir->strings[i], text)) {
            return i;
        }
    }

    u32 index = (u32)array_count(ir->strings);
    array_push(ir->strings, text);
    return index;
}

//------------------------------------------------------------------------------
// Build and intern one concatenated string literal slice into the IR pool.

internal u32 ir_add_concat_string(Ir* ir, string lhs, string rhs)
{
    u8* data = (u8*)arena_alloc(&ir->arena, lhs.count + rhs.count);
    if (lhs.count > 0) {
        memcpy(data, lhs.data, lhs.count);
    }
    if (rhs.count > 0) {
        memcpy(data + lhs.count, rhs.data, rhs.count);
    }

    return ir_add_string_literal(ir, string_from(data, lhs.count + rhs.count));
}

//------------------------------------------------------------------------------
// Return whether one declaration still requires emitted runtime state.

internal bool ir_decl_requires_runtime(const Sema* sema, const SemaDecl* decl)
{
    if (decl->kind != SK_Constant) {
        return decl->kind == SK_Variable;
    }

    return !sema->node_const_known[decl->value_node_index];
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
    case AK_StringLiteral:
        {
            IrValue value = {
                .kind = IR_VALUE_STRING,
                .value.integer =
                    ir_add_string_literal(ir, ast_get_string(lex, node)),
            };
            node_values[node_index] = value;
            return value;
        }

    case AK_StringConcat:
        {
            IrValue lhs = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            IrValue rhs = ir_lower_node(
                lex, ast, sema, node->b, node_values, next_value_index, ir);
            ASSERT(
                lhs.kind == IR_VALUE_STRING && rhs.kind == IR_VALUE_STRING,
                "Expected adjacent string literals to lower to string values");
            IrValue value = {
                .kind = IR_VALUE_STRING,
                .value.integer =
                    ir_add_concat_string(ir,
                                         ir->strings[(u32)lhs.value.integer],
                                         ir->strings[(u32)rhs.value.integer]),
            };
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

    case AK_SymbolRef:
        {
            IrValue value = {0};
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                value = (IrValue){
                    .kind          = IR_VALUE_LOCAL,
                    .value.integer =
                        sema->locals[sema->node_local_indices[node_index]]
                            .symbol_handle,
                };
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                ASSERT(decl_index != U32_MAX,
                       "Expected resolved symbol reference");
                const SemaDecl* decl = &sema->decls[decl_index];

                value = (IrValue){
                    .kind = decl->kind == SK_BuiltinFunction ? IR_VALUE_BUILTIN
                                                             : IR_VALUE_SYMBOL,
                    .value.integer = decl->symbol_handle,
                };
            }
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

    case AK_Cast:
        {
            IrValue source = ir_lower_node(
                lex, ast, sema, node->a, node_values, next_value_index, ir);
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_cast(ir, value, source, sema->node_type_indices[node_index]);
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

    default:
        error_ice("Unhandled AST node kind during IR lowering: %u", node->kind);
        return (IrValue){0};
    }
}

//------------------------------------------------------------------------------
// Lower one call expression as a statement.

internal void ir_generate_call_statement(const Lexer*   lex,
                                         const Ast*     ast,
                                         const Sema*    sema,
                                         const AstNode* call_node,
                                         Array(IrValue) node_values,
                                         u64* next_value_index,
                                         Ir*  ir)
{
    ASSERT(call_node->kind == AK_Call, "Expected call node");

    IrValue callee = ir_lower_node(
        lex, ast, sema, call_node->a, node_values, next_value_index, ir);
    IrValue arg = ir_lower_node(
        lex, ast, sema, call_node->b, node_values, next_value_index, ir);
    ir_add_call(ir, callee, arg);
}

//------------------------------------------------------------------------------
// Lower one explicit return statement.

internal void ir_generate_return_statement(const Lexer*   lex,
                                           const Ast*     ast,
                                           const Sema*    sema,
                                           const AstNode* return_node,
                                           Array(IrValue) node_values,
                                           u64* next_value_index,
                                           Ir*  ir)
{
    ASSERT(return_node->kind == AK_Return, "Expected return node");

    IrValue value = ir_lower_node(
        lex, ast, sema, return_node->a, node_values, next_value_index, ir);
    ir_add_return(ir, value);
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
    IrValue result             = {.kind = IR_VALUE_INTEGER, .value.integer = 0};
    if (decl->value_node_index != sema_no_decl()) {
        result = ir_lower_node(lex,
                               ast,
                               sema,
                               decl->value_node_index,
                               node_values,
                               next_value_index,
                               ir);
    }
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
    const AstNode* fn_def_node   = &ast->nodes[decl->value_node_index];
    const AstNode* fn_start_node = &ast->nodes[fn_def_node->a];

    ASSERT(fn_def_node->kind == AK_FnDef, "Expected function definition");
    ASSERT(fn_start_node->kind == AK_FnStart, "Expected function start");
    ASSERT(fn_start_node->b > fn_def_node->a, "Expected valid function range");

    ir_add_fn_start(ir, ast_get_symbol(bind_node));

    Array(IrValue) node_values = ir_make_node_values(ast);
    u64 next_value_index       = 0;

    if (fn_def_node->b == AFK_Expr) {
        ASSERT(ast->nodes[fn_start_node->b - 1].kind == AK_Expression,
               "Expected expression node before function end");
        IrValue result = ir_lower_node(lex,
                                       ast,
                                       sema,
                                       fn_start_node->b - 1,
                                       node_values,
                                       &next_value_index,
                                       ir);
        ir_add_return(ir, result);
    } else {
        bool has_explicit_return = false;

        for (u32 i = fn_def_node->a + 1; i < fn_start_node->b; ++i) {
            const AstNode* node = &ast->nodes[i];
            if (node->kind != AK_Statement && node->kind != AK_Return &&
                node->kind != AK_Variable && node->kind != AK_Assign) {
                continue;
            }

            if (node->kind == AK_Return) {
                has_explicit_return = true;
                ir_generate_return_statement(
                    lex, ast, sema, node, node_values, &next_value_index, ir);
                break;
            }

            if (node->kind == AK_Variable) {
                IrValue value = {.kind = IR_VALUE_INTEGER, .value.integer = 0};
                if (ast->nodes[node->b].kind == AK_AnnotatedValue) {
                    value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          ast->nodes[node->b].b,
                                          node_values,
                                          &next_value_index,
                                          ir);
                } else if (ast->nodes[node->b].kind != AK_ZeroInit) {
                    value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          node->b,
                                          node_values,
                                          &next_value_index,
                                          ir);
                }
                ir_add_local(ir, node->a, value);
                continue;
            }

            if (node->kind == AK_Assign) {
                IrValue value = ir_lower_node(
                    lex, ast, sema, node->b, node_values, &next_value_index, ir);
                IrValue target = sema->node_local_indices[i] != sema_no_local()
                                     ? (IrValue){.kind = IR_VALUE_LOCAL,
                                                 .value.integer = node->a}
                                     : (IrValue){.kind = IR_VALUE_SYMBOL,
                                                 .value.integer = node->a};
                ir_add_assign(ir, target, value);
                continue;
            }

            const AstNode* expr            = &ast->nodes[node->a];
            u32            expr_root_index = node->a;
            if (expr->kind == AK_Expression) {
                expr_root_index = expr->a;
                expr            = &ast->nodes[expr_root_index];
            }

            if (expr->kind == AK_Call) {
                ir_generate_call_statement(
                    lex, ast, sema, expr, node_values, &next_value_index, ir);
            } else {
                (void)ir_lower_node(lex,
                                    ast,
                                    sema,
                                    expr_root_index,
                                    node_values,
                                    &next_value_index,
                                    ir);
            }
        }

        if (!has_explicit_return) {
            // Normal block functions currently omit explicit return types, so
            // the first implementation lowers them as i32-returning functions
            // with an implicit zero result.
            ir_add_return(
                ir, (IrValue){.kind = IR_VALUE_INTEGER, .value.integer = 0});
        }
    }

    ir_add_fn_end(ir);
    array_free(node_values);
}

//------------------------------------------------------------------------------
// Generate IR from ordered semantic declarations.

Ir ir_generate(const Lexer* lex, const Ast* ast, const Sema* sema)
{
    Ir   ir            = {0};
    bool has_constants = false;
    arena_init(&ir.arena);

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        if (ir_decl_requires_runtime(sema, decl)) {
            has_constants = true;
            ir_add_global(&ir, decl->symbol_handle);
        }
    }

    if (has_constants) {
        u64 next_global_value_index = 0;

        ir_add_init_start(&ir);
        for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
            const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
            if (ir_decl_requires_runtime(sema, decl)) {
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

void ir_done(Ir* ir)
{
    array_free(ir->instructions);
    array_free(ir->strings);
    if (ir->arena.data != NULL) {
        arena_done(&ir->arena);
    }
    *ir = (Ir){0};
}

//------------------------------------------------------------------------------
