//------------------------------------------------------------------------------
// IR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include "compiler/ir/ir.h"
#include <compiler/error/error.h>

//------------------------------------------------------------------------------
// Append a global declaration marker to the IR stream.

void ir_add_global(Ir* ir, u32 symbol_handle, u32 type_index)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_GLOBAL,
                   .lvalue = {.kind          = IR_VALUE_SYMBOL,
                              .type          = type_index,
                              .value.integer = symbol_handle},
               });
}

//------------------------------------------------------------------------------
// Append the generated init-function start marker.

void ir_add_init_start(Ir* ir)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op = IR_OP_INIT_START,
               });
}

//------------------------------------------------------------------------------
// Append the generated init-function end marker.

void ir_add_init_end(Ir* ir)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op = IR_OP_INIT_END,
               });
}

//------------------------------------------------------------------------------
// Append a function start marker to the IR stream.

void ir_add_fn_start(Ir* ir, u32 symbol_handle, u32 type_index)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_FN_START,
                   .lvalue = {.kind          = IR_VALUE_SYMBOL,
                              .type          = type_index,
                              .value.integer = symbol_handle},
               });
}

//------------------------------------------------------------------------------
// Append a function end marker to the IR stream.

void ir_add_fn_end(Ir* ir)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op = IR_OP_FN_END,
               });
}

void ir_add_local(
    Ir* ir, u32 symbol_handle, u32 type_index, IrValue rvalue, u32 rvalue_type)
{
    rvalue.type = rvalue_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_LOCAL,
                   .lvalue = {.kind          = IR_VALUE_LOCAL,
                              .type          = type_index,
                              .value.integer = symbol_handle},
                   .rvalue = {rvalue, {0}},
               });
}

//------------------------------------------------------------------------------
// Append an assignment instruction to the IR stream.

void ir_add_assign(
    Ir* ir, IrValue lvalue, u32 lvalue_type, IrValue rvalue, u32 rvalue_type)
{
    lvalue.type = lvalue_type;
    rvalue.type = rvalue_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_ASSIGN,
                   .lvalue = lvalue,
                   .rvalue = {rvalue, {0}},
               });
}

//------------------------------------------------------------------------------
// Append a call instruction to the IR stream.

void ir_add_call(
    Ir* ir, IrValue callee, u32 callee_type, IrValue arg, u32 arg_type)
{
    callee.type = callee_type;
    arg.type    = arg_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_CALL,
                   .rvalue = {callee, arg},
               });
}

//------------------------------------------------------------------------------
// Append an explicit cast instruction to the IR stream.

void ir_add_cast(
    Ir* ir, IrValue lvalue, u32 lvalue_type, IrValue value, u32 value_type)
{
    lvalue.type = lvalue_type;
    value.type  = value_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_CAST,
                   .lvalue = lvalue,
                   .rvalue = {value, {0}},
               });
}

//------------------------------------------------------------------------------
// Append explicit string-builder instructions to the IR stream.

void ir_add_string_reset(Ir* ir)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op = IR_OP_STRING_RESET,
               });
}

void ir_add_string_start(Ir* ir, IrValue lvalue)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_STRING_START,
                   .lvalue = lvalue,
               });
}

void ir_add_string_append(Ir* ir, IrValue value, u32 type_index)
{
    value.type = type_index;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_STRING_APPEND,
                   .rvalue = {value, {0}},
               });
}

void ir_add_string_finish(Ir* ir, IrValue lvalue, IrValue start_value)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_STRING_FINISH,
                   .lvalue = lvalue,
                   .rvalue = {start_value, {0}},
               });
}

//------------------------------------------------------------------------------
// Append a return instruction to the IR stream.

void ir_add_return(Ir* ir, IrValue rvalue, u32 rvalue_type)
{
    rvalue.type = rvalue_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_RETURN,
                   .rvalue = {rvalue, {0}},
               });
}

//------------------------------------------------------------------------------
// Append a unary instruction to the IR stream.

void ir_add_unary(Ir*         ir,
                  IrOperation op,
                  IrValue     lvalue,
                  u32         lvalue_type,
                  IrValue     rhs,
                  u32         rhs_type)
{
    lvalue.type = lvalue_type;
    rhs.type    = rhs_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = op,
                   .lvalue = lvalue,
                   .rvalue = {rhs, {0}},
               });
}

//------------------------------------------------------------------------------
// Append a binary instruction to the IR stream.

void ir_add_binary(Ir*         ir,
                   IrOperation op,
                   IrValue     lvalue,
                   u32         lvalue_type,
                   IrValue     lhs,
                   u32         lhs_type,
                   IrValue     rhs,
                   u32         rhs_type)
{
    lvalue.type = lvalue_type;
    lhs.type    = lhs_type;
    rhs.type    = rhs_type;
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

internal u32 ir_builtin_string_type(const Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == STK_String) {
            return i;
        }
    }

    return sema_no_type();
}

internal u32 ir_builtin_type(const Sema* sema, SemaTypeKind kind)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == kind) {
            return i;
        }
    }

    return sema_no_type();
}

internal u32 ir_string_append_type(const Sema* sema, u32 node_type_index)
{
    return sema_materialise_type(sema, node_type_index);
}

internal u32 ir_node_type_index(const Ast*  ast,
                                const Sema* sema,
                                u32         node_index)
{
    if (node_index < array_count(sema->node_type_indices) &&
        sema->node_type_indices[node_index] != sema_no_type()) {
        return ir_string_append_type(sema, sema->node_type_indices[node_index]);
    }

    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_InterpPartExpr) {
        return ir_node_type_index(ast, sema, node->a);
    }

    if (node->kind == AK_SymbolRef) {
        if (sema->node_local_indices[node_index] != sema_no_local()) {
            return sema->locals[sema->node_local_indices[node_index]]
                .type_index;
        }
        if (sema->node_decl_indices[node_index] != sema_no_decl()) {
            return sema->decls[sema->node_decl_indices[node_index]].type_index;
        }
    }

    return sema_no_type();
}

internal u32 ir_value_type_for_local(const Sema* sema, u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        if (sema->locals[i].symbol_handle == symbol_handle) {
            return sema->locals[i].type_index;
        }
    }

    return sema_no_type();
}

internal u32 ir_value_type_for_decl(const Sema* sema, u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol_handle) {
            return sema->decls[i].type_index;
        }
    }

    return sema_no_type();
}

internal bool ir_node_contains_interpolation(const Ast* ast, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_InterpolatedString:
        return true;
    case AK_InterpPartExpr:
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
    case AK_IntegerNegate:
    case AK_Cast:
        return ir_node_contains_interpolation(ast, node->a);
    case AK_StringConcat:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_Call:
        return ir_node_contains_interpolation(ast, node->a) ||
               ir_node_contains_interpolation(ast, node->b);
    default:
        return false;
    }
}

internal IrValue ir_lower_node(const Lexer* lex,
                               const Ast*   ast,
                               const Sema*  sema,
                               u32          node_index,
                               Array(IrValue) node_values,
                               u64* next_value_index,
                               Ir*  ir);

internal void ir_append_string_node(const Lexer* lex,
                                    const Ast*   ast,
                                    const Sema*  sema,
                                    u32          node_index,
                                    Array(IrValue) node_values,
                                    u64* next_value_index,
                                    Ir*  ir);

internal IrValue ir_build_runtime_string(const Lexer* lex,
                                         const Ast*   ast,
                                         const Sema*  sema,
                                         u32          node_index,
                                         Array(IrValue) node_values,
                                         u64* next_value_index,
                                         Ir*  ir)
{
    IrValue start_value = {
        .kind          = IR_VALUE_VARIABLE,
        .type          = U32_MAX,
        .value.integer = (i64)(*next_value_index)++,
    };
    IrValue result = {
        .kind          = IR_VALUE_VARIABLE,
        .type          = ir_builtin_string_type(sema),
        .value.integer = (i64)(*next_value_index)++,
    };

    ir_add_string_start(ir, start_value);
    ir_append_string_node(
        lex, ast, sema, node_index, node_values, next_value_index, ir);
    ir_add_string_finish(ir, result, start_value);
    return result;
}

internal void ir_append_string_node(const Lexer* lex,
                                    const Ast*   ast,
                                    const Sema*  sema,
                                    u32          node_index,
                                    Array(IrValue) node_values,
                                    u64* next_value_index,
                                    Ir*  ir)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_StringLiteral:
        ir_add_string_append(ir,
                             (IrValue){
                                 .kind          = IR_VALUE_STRING,
                                 .value.integer = ir_add_string_literal(
                                     ir, ast_get_string(lex, node)),
                             },
                             ir_builtin_string_type(sema));
        return;

    case AK_StringConcat:
        ir_append_string_node(
            lex, ast, sema, node->a, node_values, next_value_index, ir);
        ir_append_string_node(
            lex, ast, sema, node->b, node_values, next_value_index, ir);
        return;

    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            const AstNode* part = &ast->nodes[i];
            if (part->kind == AK_StringLiteral) {
                ir_add_string_append(ir,
                                     (IrValue){
                                         .kind          = IR_VALUE_STRING,
                                         .value.integer = ir_add_string_literal(
                                             ir, ast_get_string(lex, part)),
                                     },
                                     ir_builtin_string_type(sema));
                continue;
            }

            ASSERT(part->kind == AK_InterpPartExpr,
                   "Expected interpolated string part expression");
            IrValue part_value = ir_lower_node(
                lex, ast, sema, part->a, node_values, next_value_index, ir);
            ir_add_string_append(
                ir, part_value, ir_node_type_index(ast, sema, part->a));
        }
        return;

    default:
        {
            IrValue value = ir_lower_node(
                lex, ast, sema, node_index, node_values, next_value_index, ir);
            ir_add_string_append(
                ir, value, ir_node_type_index(ast, sema, node_index));
            return;
        }
    }
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
            .type          = ir_node_type_index(ast, sema, node_index),
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
                .type = ir_builtin_string_type(sema),
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
            IrValue value = ir_unset_value();
            if (lhs.kind == IR_VALUE_STRING && rhs.kind == IR_VALUE_STRING) {
                value = (IrValue){
                    .kind          = IR_VALUE_STRING,
                    .value.integer = ir_add_concat_string(
                        ir,
                        ir->strings[(u32)lhs.value.integer],
                        ir->strings[(u32)rhs.value.integer]),
                };
            } else {
                value = ir_build_runtime_string(lex,
                                                ast,
                                                sema,
                                                node_index,
                                                node_values,
                                                next_value_index,
                                                ir);
            }
            node_values[node_index] = value;
            return value;
        }

    case AK_InterpolatedString:
        {
            IrValue value = ir_build_runtime_string(
                lex, ast, sema, node_index, node_values, next_value_index, ir);
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
            IrValue value = ir_unset_value();
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                value = (IrValue){
                    .kind = IR_VALUE_LOCAL,
                    .type = ir_value_type_for_local(
                        sema,
                        sema->locals[sema->node_local_indices[node_index]]
                            .symbol_handle),
                    .value.integer =
                        sema->locals[sema->node_local_indices[node_index]]
                            .symbol_handle,
                };
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                ASSERT(decl_index != U32_MAX,
                       "Expected resolved symbol reference");
                const SemaDecl* decl = &sema->decls[decl_index];

                value                = (IrValue){
                                   .kind = decl->kind == SK_BuiltinFunction ? IR_VALUE_BUILTIN
                                                                            : IR_VALUE_SYMBOL,
                                   .type = decl->type_index,
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
            ir_add_unary(ir,
                         IR_OP_NEGATE,
                         value,
                         ir_node_type_index(ast, sema, node_index),
                         rhs,
                         ir_node_type_index(ast, sema, node->a));
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
            ir_add_cast(ir,
                        value,
                        sema->node_type_indices[node_index],
                        source,
                        ir_node_type_index(ast, sema, node->a));
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
            ir_add_binary(ir,
                          op,
                          value,
                          ir_node_type_index(ast, sema, node_index),
                          lhs,
                          ir_node_type_index(ast, sema, node->a),
                          rhs,
                          ir_node_type_index(ast, sema, node->b));
            node_values[node_index] = value;
            return value;
        }

    default:
        error_ice("Unhandled AST node kind during IR lowering: %u", node->kind);
        return ir_unset_value();
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
    ir_add_call(ir,
                callee,
                ir_node_type_index(ast, sema, call_node->a),
                arg,
                ir_node_type_index(ast, sema, call_node->b));
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
    ir_add_return(ir, value, ir_node_type_index(ast, sema, return_node->a));
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
    IrValue result             = {
                    .kind = IR_VALUE_INTEGER, .type = decl->type_index, .value.integer = 0};
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
                            .type          = decl->type_index,
                            .value.integer = decl->symbol_handle},
                  decl->type_index,
                  result,
                  decl->type_index);
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

    ir_add_fn_start(ir, ast_get_symbol(bind_node), decl->type_index);

    bool needs_string_runtime = false;
    for (u32 i = fn_def_node->a; i < fn_start_node->b; ++i) {
        if (ast->nodes[i].kind == AK_InterpolatedString) {
            needs_string_runtime = true;
            break;
        }
    }
    if (needs_string_runtime) {
        ir_add_string_reset(ir);
    }

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
        ir_add_return(
            ir, result, ir_node_type_index(ast, sema, fn_start_node->b - 1));
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
                u32     local_type = ir_value_type_for_local(sema, node->a);
                IrValue value      = {.kind          = IR_VALUE_INTEGER,
                                      .type          = local_type,
                                      .value.integer = 0};
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
                ir_add_local(ir, node->a, local_type, value, local_type);
                continue;
            }

            if (node->kind == AK_Assign) {
                IrValue value = ir_lower_node(lex,
                                              ast,
                                              sema,
                                              node->b,
                                              node_values,
                                              &next_value_index,
                                              ir);
                IrValue target =
                    sema->node_local_indices[i] != sema_no_local()
                        ? (IrValue){.kind = IR_VALUE_LOCAL,
                                    .type =
                                        ir_value_type_for_local(sema, node->a),
                                    .value.integer = node->a}
                        : (IrValue){.kind = IR_VALUE_SYMBOL,
                                    .type =
                                        ir_value_type_for_decl(sema, node->a),
                                    .value.integer = node->a};
                u32 target_type = sema->node_local_indices[i] != sema_no_local()
                                      ? ir_value_type_for_local(sema, node->a)
                                      : ir_value_type_for_decl(sema, node->a);
                ir_add_assign(ir, target, target_type, value, target_type);
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

            if (ir_node_contains_interpolation(ast, expr_root_index)) {
                ir_add_string_reset(ir);
            }
        }

        if (!has_explicit_return) {
            // Normal block functions currently omit explicit return types, so
            // the first implementation lowers them as i32-returning functions
            // with an implicit zero result.
            ir_add_return(ir,
                          (IrValue){.kind = IR_VALUE_INTEGER,
                                    .type = ir_builtin_type(sema, STK_I32),
                                    .value.integer = 0},
                          ir_builtin_type(sema, STK_I32));
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

    for (u32 i = 0; i < array_count(sema->types); ++i) {
        array_push(ir.types, sema->types[i]);
    }

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        if (ir_decl_requires_runtime(sema, decl)) {
            has_constants = true;
            ir_add_global(&ir, decl->symbol_handle, decl->type_index);
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
    array_free(ir->types);
    if (ir->arena.data != NULL) {
        arena_done(&ir->arena);
    }
    *ir = (Ir){0};
}

//------------------------------------------------------------------------------
