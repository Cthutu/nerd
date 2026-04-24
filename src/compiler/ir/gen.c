//------------------------------------------------------------------------------
// IR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include "compiler/ir/ir.h"
#include <compiler/error/error.h>

//------------------------------------------------------------------------------
// AST ownership helpers.

internal bool ir_block_is_expr_block_body(const Ast* ast, u32 block_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_ExprBlock && node->a == block_index) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
// Append a global declaration marker to the IR stream.

void ir_add_global(Ir* ir, u32 symbol_handle, u32 type_index)
{
    array_push(ir->globals,
               (IrGlobal){
                   .symbol = symbol_handle,
                   .type   = type_index,
               });
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

void ir_add_block_start(Ir* ir)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op = IR_OP_BLOCK_START,
               });
}

void ir_add_block_end(Ir* ir)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op = IR_OP_BLOCK_END,
               });
}

void ir_add_local(Ir*     ir,
                  u32     function_index,
                  u32     symbol_handle,
                  u32     type_index,
                  IrValue rvalue,
                  u32     rvalue_type)
{
    rvalue.type = rvalue_type;
    array_push(ir->locals,
               (IrLocal){
                   .symbol         = symbol_handle,
                   .type           = type_index,
                   .function_index = function_index,
                   .is_param       = false,
               });
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_LOCAL,
                   .lvalue = {.kind          = IR_VALUE_LOCAL,
                              .type          = type_index,
                              .value.integer = symbol_handle},
                   .rvalue = {rvalue, {0}},
               });
}

void ir_add_temp_local(Ir* ir, IrValue lvalue, u32 type_index)
{
    lvalue.type = type_index;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_LOCAL,
                   .lvalue = lvalue,
                   .rvalue = {{.kind          = IR_VALUE_INTEGER,
                               .type          = type_index,
                               .value.integer = 0},
                              {0}},
               });
}

void ir_add_param(Ir* ir, u32 function_index, u32 symbol_handle, u32 type_index)
{
    array_push(ir->locals,
               (IrLocal){
                   .symbol         = symbol_handle,
                   .type           = type_index,
                   .function_index = function_index,
                   .is_param       = true,
               });
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_PARAM,
                   .lvalue = {.kind          = IR_VALUE_LOCAL,
                              .type          = type_index,
                              .value.integer = symbol_handle},
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

void ir_add_call(Ir*     ir,
                 IrValue lvalue,
                 u32     lvalue_type,
                 IrValue callee,
                 u32     callee_type,
                 Array(IrValue) args,
                 Array(u32) arg_types)
{
    callee.type   = callee_type;
    u32 first_arg = (u32)array_count(ir->call_args);
    for (u32 i = 0; i < array_count(args); ++i) {
        IrValue arg = args[i];
        arg.type    = arg_types[i];
        array_push(ir->call_args,
                   (IrCallArg){
                       .value = arg,
                       .type  = arg_types[i],
                   });
    }

    u32 call_index = (u32)array_count(ir->calls);
    array_push(ir->calls,
               (IrCallInfo){
                   .first_arg = first_arg,
                   .arg_count = (u32)array_count(args),
               });
    lvalue.type = lvalue_type;
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_CALL,
            .lvalue = lvalue,
            .rvalue = {callee,
                       {.kind = IR_VALUE_INTEGER, .value.integer = call_index}},
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

void ir_add_branch_false(Ir*     ir,
                         IrValue condition,
                         u32     condition_type,
                         i64     label)
{
    condition.type = condition_type;
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_BRANCH_FALSE,
            .rvalue = {condition,
                       {.kind = IR_VALUE_INTEGER, .value.integer = label}},
        });
}

void ir_add_jump(Ir* ir, i64 label)
{
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_JUMP,
            .rvalue = {{.kind = IR_VALUE_INTEGER, .value.integer = label}, {0}},
        });
}

void ir_add_label(Ir* ir, i64 label)
{
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_LABEL,
                   .lvalue = {.kind = IR_VALUE_INTEGER, .value.integer = label},
               });
}

typedef enum : u8 {
    IR_STMT_FALLTHROUGH,
    IR_STMT_RETURN,
    IR_STMT_BREAK,
    IR_STMT_CONTINUE,
} IrStatementResult;

#define IR_MAX_EXPR_BLOCK_TARGETS 64

typedef struct {
    u32     label_symbol;
    i64     break_label;
    IrValue result;
    u32     result_type;
} IrExprBlockTarget;

typedef struct {
    i64               break_label;
    i64               continue_label;
    u32               expr_target_count;
    IrExprBlockTarget expr_targets[IR_MAX_EXPR_BLOCK_TARGETS];
} IrLoopLabels;

internal IrLoopLabels ir_no_control_labels(void)
{
    return (IrLoopLabels){
        .break_label    = -1,
        .continue_label = -1,
    };
}

internal const IrExprBlockTarget* ir_find_expr_block_target(IrLoopLabels loop,
                                                            u32 label_symbol)
{
    for (u32 i = loop.expr_target_count; i > 0; --i) {
        const IrExprBlockTarget* target = &loop.expr_targets[i - 1];
        if (label_symbol == U32_MAX || target->label_symbol == label_symbol) {
            return target;
        }
    }
    return NULL;
}

void ir_add_equal(Ir*     ir,
                  IrValue lvalue,
                  u32     lvalue_type,
                  IrValue lhs,
                  u32     lhs_type,
                  IrValue rhs,
                  u32     rhs_type)
{
    lvalue.type = lvalue_type;
    lhs.type    = lhs_type;
    rhs.type    = rhs_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_EQUAL,
                   .lvalue = lvalue,
                   .rvalue = {lhs, rhs},
               });
}

void ir_add_less(Ir*     ir,
                 IrValue lvalue,
                 u32     lvalue_type,
                 IrValue lhs,
                 u32     lhs_type,
                 IrValue rhs,
                 u32     rhs_type)
{
    lvalue.type = lvalue_type;
    lhs.type    = lhs_type;
    rhs.type    = rhs_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_LESS,
                   .lvalue = lvalue,
                   .rvalue = {lhs, rhs},
               });
}

void ir_add_less_equal(Ir*     ir,
                       IrValue lvalue,
                       u32     lvalue_type,
                       IrValue lhs,
                       u32     lhs_type,
                       IrValue rhs,
                       u32     rhs_type)
{
    lvalue.type = lvalue_type;
    lhs.type    = lhs_type;
    rhs.type    = rhs_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_LESS_EQUAL,
                   .lvalue = lvalue,
                   .rvalue = {lhs, rhs},
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

internal IrValue ir_make_bool_literal(u32 bool_type, bool value)
{
    return (IrValue){
        .kind          = IR_VALUE_INTEGER,
        .type          = bool_type,
        .value.integer = value ? 1 : 0,
    };
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

internal u32 ir_value_type_for_local_index(const Sema* sema, u32 local_index)
{
    if (local_index == sema_no_local()) {
        return sema_no_type();
    }
    return sema->locals[local_index].type_index;
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

internal u32 ir_find_function_param_local(const Sema*     sema,
                                          u32             root_scope_index,
                                          const AstParam* param)
{
    if (root_scope_index == U32_MAX) {
        return sema_no_local();
    }

    const SemaScope* scope = &sema->scopes[root_scope_index];
    u32              end   = scope->first_local + scope->local_count;
    for (u32 i = scope->first_local; i < end; ++i) {
        const SemaLocal* local = &sema->locals[i];
        if (local->kind == SLK_Param &&
            local->decl_node_index == sema_no_decl() &&
            local->symbol_handle == param->symbol_handle &&
            local->type_node_index == param->type_node_index) {
            return i;
        }
    }

    return sema_no_local();
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
    case AK_IntegerNegate:
    case AK_Cast:
        return ir_node_contains_interpolation(ast, node->a);
    case AK_Return:
    case AK_ReturnExpr:
        return node->a != U32_MAX &&
               ir_node_contains_interpolation(ast, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            if (ir_node_contains_interpolation(ast, node->a)) {
                return true;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        if (ir_node_contains_interpolation(
                                ast,
                                ast->on_pattern_nodes
                                    [branch->pattern_node_index + pattern])) {
                            return true;
                        }
                    }
                }
                if (ir_node_contains_interpolation(ast,
                                                   branch->expr_node_index)) {
                    return true;
                }
            }
            return false;
        }
        return ir_node_contains_interpolation(ast, node->a) ||
               ir_node_contains_interpolation(ast, node->b);
    case AK_Call:
        if (ir_node_contains_interpolation(ast, node->a)) {
            return true;
        }
        {
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (ir_node_contains_interpolation(
                        ast, ast->call_args[call->first_arg + i])) {
                    return true;
                }
            }
            return false;
        }
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            for (u32 i = 0; i < for_info->init_count; ++i) {
                if (ir_node_contains_interpolation(
                        ast, ast->for_items[for_info->first_init + i])) {
                    return true;
                }
            }
            if (for_info->condition_node_index != U32_MAX &&
                ir_node_contains_interpolation(
                    ast, for_info->condition_node_index)) {
                return true;
            }
            for (u32 i = 0; i < for_info->update_count; ++i) {
                if (ir_node_contains_interpolation(
                        ast, ast->for_items[for_info->first_update + i])) {
                    return true;
                }
            }
            return ir_node_contains_interpolation(ast, node->b);
        }
    default:
        return false;
    }
}

internal IrValue           ir_lower_node(const Lexer* lex,
                                         const Ast*   ast,
                                         const Sema*  sema,
                                         u32          node_index,
                                         IrLoopLabels loop,
                                         Array(IrValue) node_values,
                                         u64* next_value_index,
                                         Ir*  ir);
internal void              ir_generate_return_statement(const Lexer*   lex,
                                                        const Ast*     ast,
                                                        const Sema*    sema,
                                                        const AstNode* return_node,
                                                        IrLoopLabels   loop,
                                                        Array(IrValue) node_values,
                                                        u64* next_value_index,
                                                        Ir*  ir);
internal IrStatementResult ir_generate_statement(const Lexer* lex,
                                                 const Ast*   ast,
                                                 const Sema*  sema,
                                                 u32          function_index,
                                                 u32          node_index,
                                                 IrLoopLabels loop,
                                                 Array(IrValue) node_values,
                                                 u64* next_value_index,
                                                 Ir*  ir);

internal void ir_append_string_node(const Lexer* lex,
                                    const Ast*   ast,
                                    const Sema*  sema,
                                    u32          node_index,
                                    IrLoopLabels loop,
                                    Array(IrValue) node_values,
                                    u64* next_value_index,
                                    Ir*  ir);

internal IrValue ir_build_runtime_string(const Lexer* lex,
                                         const Ast*   ast,
                                         const Sema*  sema,
                                         u32          node_index,
                                         IrLoopLabels loop,
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
        lex, ast, sema, node_index, loop, node_values, next_value_index, ir);
    ir_add_string_finish(ir, result, start_value);
    return result;
}

internal IrValue ir_lower_call(const Lexer*   lex,
                               const Ast*     ast,
                               const Sema*    sema,
                               const AstNode* call_node,
                               IrLoopLabels   loop,
                               Array(IrValue) node_values,
                               u64* next_value_index,
                               Ir*  ir)
{
    ASSERT(call_node->kind == AK_Call, "Expected call node");

    IrValue callee = ir_lower_node(
        lex, ast, sema, call_node->a, loop, node_values, next_value_index, ir);
    const AstCallInfo* call = &ast->calls[call_node->b];
    Array(IrValue) args     = NULL;
    Array(u32) arg_types    = NULL;
    for (u32 i = 0; i < call->arg_count; ++i) {
        u32 arg_node = ast->call_args[call->first_arg + i];
        array_push(args,
                   ir_lower_node(lex,
                                 ast,
                                 sema,
                                 arg_node,
                                 loop,
                                 node_values,
                                 next_value_index,
                                 ir));
        array_push(arg_types, ir_node_type_index(ast, sema, arg_node));
    }

    u32     result_type = ir_node_type_index(ast, sema, call_node - ast->nodes);
    IrValue result      = {.kind = IR_VALUE_NONE};
    if (result_type != ir_builtin_type(sema, STK_Void)) {
        result = (IrValue){
            .kind          = IR_VALUE_VARIABLE,
            .value.integer = (i64)(*next_value_index)++,
        };
    }

    ir_add_call(ir,
                result,
                result_type,
                callee,
                ir_node_type_index(ast, sema, call_node->a),
                args,
                arg_types);
    array_free(args);
    array_free(arg_types);
    return result;
}

internal void ir_append_string_node(const Lexer* lex,
                                    const Ast*   ast,
                                    const Sema*  sema,
                                    u32          node_index,
                                    IrLoopLabels loop,
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
            lex, ast, sema, node->a, loop, node_values, next_value_index, ir);
        ir_append_string_node(
            lex, ast, sema, node->b, loop, node_values, next_value_index, ir);
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
            IrValue part_value = ir_lower_node(lex,
                                               ast,
                                               sema,
                                               part->a,
                                               loop,
                                               node_values,
                                               next_value_index,
                                               ir);
            ir_add_string_append(
                ir, part_value, ir_node_type_index(ast, sema, part->a));
        }
        return;

    default:
        {
            IrValue value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          node_index,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
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
                               IrLoopLabels loop,
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
    case AK_FloatLiteral:
        {
            IrValue value = {
                .kind           = IR_VALUE_FLOAT,
                .type           = ir_node_type_index(ast, sema, node_index),
                .value.floating = ast_get_float(lex, node),
            };
            node_values[node_index] = value;
            return value;
        }

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
            IrValue lhs   = ir_lower_node(lex,
                                        ast,
                                        sema,
                                        node->a,
                                        loop,
                                        node_values,
                                        next_value_index,
                                        ir);
            IrValue rhs   = ir_lower_node(lex,
                                        ast,
                                        sema,
                                        node->b,
                                        loop,
                                        node_values,
                                        next_value_index,
                                        ir);
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
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
            }
            node_values[node_index] = value;
            return value;
        }

    case AK_InterpolatedString:
        {
            IrValue value           = ir_build_runtime_string(lex,
                                                    ast,
                                                    sema,
                                                    node_index,
                                                    loop,
                                                    node_values,
                                                    next_value_index,
                                                    ir);
            node_values[node_index] = value;
            return value;
        }

    case AK_Expression:
        {
            IrValue value           = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          node->a,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
            node_values[node_index] = value;
            return value;
        }

    case AK_FnDef:
        {
            ASSERT(sema->node_lowered_symbol_handles[node_index] != U32_MAX,
                   "Expected lowered symbol for function value");
            IrValue value = {
                .kind          = IR_VALUE_SYMBOL,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = sema->node_lowered_symbol_handles[node_index],
            };
            node_values[node_index] = value;
            return value;
        }

    case AK_SymbolRef:
        {
            IrValue value = ir_unset_value();
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                u32 local_index        = sema->node_local_indices[node_index];
                const SemaLocal* local = &sema->locals[local_index];
                if (local->kind == SLK_Constant) {
                    value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          local->value_node_index,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
                } else if (local->kind == SLK_Binder) {
                    value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          local->value_node_index,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
                } else if (local->kind == SLK_Function) {
                    value = (IrValue){
                        .kind          = IR_VALUE_SYMBOL,
                        .type          = local->type_index,
                        .value.integer = local->lowered_symbol_handle,
                    };
                } else {
                    value = (IrValue){
                        .kind = IR_VALUE_LOCAL,
                        .type =
                            ir_value_type_for_local_index(sema, local_index),
                        .value.integer = local->symbol_handle,
                    };
                }
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
    case AK_LogicalNot:
        {
            IrValue rhs   = ir_lower_node(lex,
                                        ast,
                                        sema,
                                        node->a,
                                        loop,
                                        node_values,
                                        next_value_index,
                                        ir);
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_unary(ir,
                         node->kind == AK_LogicalNot ? IR_OP_LOGICAL_NOT
                                                     : IR_OP_NEGATE,
                         value,
                         ir_node_type_index(ast, sema, node_index),
                         rhs,
                         ir_node_type_index(ast, sema, node->a));
            node_values[node_index] = value;
            return value;
        }

    case AK_Cast:
        {
            IrValue source = ir_lower_node(lex,
                                           ast,
                                           sema,
                                           node->a,
                                           loop,
                                           node_values,
                                           next_value_index,
                                           ir);
            IrValue value  = {
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

    case AK_Call:
        {
            IrValue value = ir_lower_call(
                lex, ast, sema, node, loop, node_values, next_value_index, ir);
            node_values[node_index] = value;
            return value;
        }

    case AK_On:
        {
            const AstOnInfo* on        = &ast->ons[node->b];
            IrValue          scrutinee = ir_lower_node(lex,
                                              ast,
                                              sema,
                                              node->a,
                                              loop,
                                              node_values,
                                              next_value_index,
                                              ir);
            u32  result_type = ir_node_type_index(ast, sema, node_index);
            bool produces_value =
                result_type != ir_builtin_type(sema, STK_Void);

            IrValue result = ir_unset_value();
            if (produces_value) {
                result = (IrValue){
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = result_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_temp_local(ir, result, result_type);
            }

            i64 end_label = (i64)(*next_value_index)++;
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                i64 next_label = i + 1 < on->branch_count
                                     ? (i64)(*next_value_index)++
                                     : end_label;

                if (on->kind == AOK_Bool) {
                    if (!(branch->flags & AOBF_Else)) {
                        ir_add_branch_false(
                            ir,
                            scrutinee,
                            ir_node_type_index(ast, sema, node->a),
                            next_label);
                    }
                } else if (!(branch->flags & AOBF_Else)) {
                    u32 bool_type   = ir_builtin_type(sema, STK_Bool);
                    i64 match_label = (i64)(*next_value_index)++;
                    for (u32 pattern_index = 0;
                         pattern_index < branch->pattern_count;
                         ++pattern_index) {
                        u32 pattern_node =
                            ast->on_pattern_nodes[branch->pattern_node_index +
                                                  pattern_index];
                        i64 mismatch_label = next_label;
                        if (pattern_index + 1 < branch->pattern_count) {
                            mismatch_label = (i64)(*next_value_index)++;
                        }
                        const AstNode* pattern = &ast->nodes[pattern_node];
                        if (pattern->kind == AK_RangeExclusive ||
                            pattern->kind == AK_RangeInclusive) {
                            IrValue start         = ir_lower_node(lex,
                                                          ast,
                                                          sema,
                                                          pattern->a,
                                                          loop,
                                                          node_values,
                                                          next_value_index,
                                                          ir);
                            IrValue start_matches = {
                                .kind          = IR_VALUE_VARIABLE,
                                .type          = bool_type,
                                .value.integer = (i64)(*next_value_index)++,
                            };
                            ir_add_less_equal(
                                ir,
                                start_matches,
                                bool_type,
                                start,
                                ir_node_type_index(ast, sema, pattern->a),
                                scrutinee,
                                ir_node_type_index(ast, sema, node->a));
                            ir_add_branch_false(
                                ir, start_matches, bool_type, mismatch_label);

                            IrValue end         = ir_lower_node(lex,
                                                        ast,
                                                        sema,
                                                        pattern->b,
                                                        loop,
                                                        node_values,
                                                        next_value_index,
                                                        ir);
                            IrValue end_matches = {
                                .kind          = IR_VALUE_VARIABLE,
                                .type          = bool_type,
                                .value.integer = (i64)(*next_value_index)++,
                            };
                            if (pattern->kind == AK_RangeInclusive) {
                                ir_add_less_equal(
                                    ir,
                                    end_matches,
                                    bool_type,
                                    scrutinee,
                                    ir_node_type_index(ast, sema, node->a),
                                    end,
                                    ir_node_type_index(ast, sema, pattern->b));
                            } else {
                                ir_add_less(
                                    ir,
                                    end_matches,
                                    bool_type,
                                    scrutinee,
                                    ir_node_type_index(ast, sema, node->a),
                                    end,
                                    ir_node_type_index(ast, sema, pattern->b));
                            }
                            ir_add_branch_false(
                                ir, end_matches, bool_type, mismatch_label);
                        } else {
                            IrValue pattern_value =
                                ir_lower_node(lex,
                                              ast,
                                              sema,
                                              pattern_node,
                                              loop,
                                              node_values,
                                              next_value_index,
                                              ir);
                            IrValue matches = {
                                .kind          = IR_VALUE_VARIABLE,
                                .type          = bool_type,
                                .value.integer = (i64)(*next_value_index)++,
                            };
                            ir_add_equal(
                                ir,
                                matches,
                                bool_type,
                                scrutinee,
                                ir_node_type_index(ast, sema, node->a),
                                pattern_value,
                                ir_node_type_index(ast, sema, pattern_node));
                            ir_add_branch_false(
                                ir, matches, bool_type, mismatch_label);
                        }
                        if (pattern_index + 1 < branch->pattern_count) {
                            ir_add_jump(ir, match_label);
                            ir_add_label(ir, mismatch_label);
                        }
                    }
                    ir_add_label(ir, match_label);
                }

                if (ast->nodes[branch->expr_node_index].kind == AK_Return ||
                    ast->nodes[branch->expr_node_index].kind == AK_ReturnExpr) {
                    ir_generate_return_statement(
                        lex,
                        ast,
                        sema,
                        &ast->nodes[branch->expr_node_index],
                        loop,
                        node_values,
                        next_value_index,
                        ir);
                } else if (ast->nodes[branch->expr_node_index].kind ==
                               AK_Break ||
                           ast->nodes[branch->expr_node_index].kind ==
                               AK_BreakExpr) {
                    ASSERT(loop.break_label >= 0, "Expected loop break label");
                    ir_add_jump(ir, loop.break_label);
                } else if (ast->nodes[branch->expr_node_index].kind ==
                               AK_Continue ||
                           ast->nodes[branch->expr_node_index].kind ==
                               AK_ContinueExpr) {
                    ASSERT(loop.continue_label >= 0,
                           "Expected loop continue label");
                    ir_add_jump(ir, loop.continue_label);
                } else {
                    IrValue branch_value =
                        ir_lower_node(lex,
                                      ast,
                                      sema,
                                      branch->expr_node_index,
                                      loop,
                                      node_values,
                                      next_value_index,
                                      ir);
                    if (produces_value) {
                        ir_add_assign(ir,
                                      result,
                                      result_type,
                                      branch_value,
                                      ir_node_type_index(
                                          ast, sema, branch->expr_node_index));
                    }
                }
                if (i + 1 < on->branch_count) {
                    ir_add_jump(ir, end_label);
                    ir_add_label(ir, next_label);
                }
            }
            ir_add_label(ir, end_label);

            node_values[node_index] = result;
            return result;
        }

    case AK_ExprBlock:
        {
            u32     result_type = ir_node_type_index(ast, sema, node_index);
            IrValue result      = ir_unset_value();
            if (result_type != ir_builtin_type(sema, STK_Void)) {
                result = (IrValue){
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = result_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_temp_local(ir, result, result_type);
            }

            i64          end_label     = (i64)(*next_value_index)++;
            IrLoopLabels block_control = loop;
            ASSERT(block_control.expr_target_count < IR_MAX_EXPR_BLOCK_TARGETS,
                   "Too many nested expression block targets");
            block_control.expr_targets[block_control.expr_target_count++] =
                (IrExprBlockTarget){
                    .label_symbol = node->b,
                    .break_label  = end_label,
                    .result       = result,
                    .result_type  = result_type,
                };

            (void)ir_generate_statement(lex,
                                        ast,
                                        sema,
                                        U32_MAX,
                                        node->a,
                                        block_control,
                                        node_values,
                                        next_value_index,
                                        ir);
            ir_add_label(ir, end_label);
            node_values[node_index] = result;
            return result;
        }

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
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        {
            if (node->kind == AK_RangeExclusive ||
                node->kind == AK_RangeInclusive) {
                error_ice(
                    "Range patterns must only appear inside block-form `on`");
            }
            if (node->kind == AK_LogicalAnd || node->kind == AK_LogicalOr) {
                u32     bool_type = ir_builtin_type(sema, STK_Bool);
                IrValue result    = {
                       .kind          = IR_VALUE_VARIABLE,
                       .type          = bool_type,
                       .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_temp_local(ir, result, bool_type);

                if (node->kind == AK_LogicalAnd) {
                    i64     false_label = (i64)(*next_value_index)++;
                    i64     end_label   = (i64)(*next_value_index)++;
                    IrValue lhs         = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                node->a,
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
                    ir_add_branch_false(ir,
                                        lhs,
                                        ir_node_type_index(ast, sema, node->a),
                                        false_label);
                    IrValue rhs = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                node->b,
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
                    ir_add_branch_false(ir,
                                        rhs,
                                        ir_node_type_index(ast, sema, node->b),
                                        false_label);
                    ir_add_assign(ir,
                                  result,
                                  bool_type,
                                  ir_make_bool_literal(bool_type, true),
                                  bool_type);
                    ir_add_jump(ir, end_label);
                    ir_add_label(ir, false_label);
                    ir_add_assign(ir,
                                  result,
                                  bool_type,
                                  ir_make_bool_literal(bool_type, false),
                                  bool_type);
                    ir_add_label(ir, end_label);
                } else {
                    i64     eval_rhs_label = (i64)(*next_value_index)++;
                    i64     false_label    = (i64)(*next_value_index)++;
                    i64     end_label      = (i64)(*next_value_index)++;
                    IrValue lhs            = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                node->a,
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
                    ir_add_branch_false(ir,
                                        lhs,
                                        ir_node_type_index(ast, sema, node->a),
                                        eval_rhs_label);
                    ir_add_assign(ir,
                                  result,
                                  bool_type,
                                  ir_make_bool_literal(bool_type, true),
                                  bool_type);
                    ir_add_jump(ir, end_label);
                    ir_add_label(ir, eval_rhs_label);
                    IrValue rhs = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                node->b,
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
                    ir_add_branch_false(ir,
                                        rhs,
                                        ir_node_type_index(ast, sema, node->b),
                                        false_label);
                    ir_add_assign(ir,
                                  result,
                                  bool_type,
                                  ir_make_bool_literal(bool_type, true),
                                  bool_type);
                    ir_add_jump(ir, end_label);
                    ir_add_label(ir, false_label);
                    ir_add_assign(ir,
                                  result,
                                  bool_type,
                                  ir_make_bool_literal(bool_type, false),
                                  bool_type);
                    ir_add_label(ir, end_label);
                }

                node_values[node_index] = result;
                return result;
            }

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
            case AK_BitwiseAnd:
                op = IR_OP_BITWISE_AND;
                break;
            case AK_BitwiseXor:
                op = IR_OP_BITWISE_XOR;
                break;
            case AK_BitwiseOr:
                op = IR_OP_BITWISE_OR;
                break;
            case AK_Equal:
                op = IR_OP_EQUAL;
                break;
            case AK_NotEqual:
                op = IR_OP_NOT_EQUAL;
                break;
            case AK_Less:
            case AK_Greater:
                op = IR_OP_LESS;
                break;
            case AK_LessEqual:
            case AK_GreaterEqual:
                op = IR_OP_LESS_EQUAL;
                break;
            default:
                break;
            }

            IrValue lhs   = ir_lower_node(lex,
                                        ast,
                                        sema,
                                        node->a,
                                        loop,
                                        node_values,
                                        next_value_index,
                                        ir);
            IrValue rhs   = ir_lower_node(lex,
                                        ast,
                                        sema,
                                        node->b,
                                        loop,
                                        node_values,
                                        next_value_index,
                                        ir);
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .value.integer = (i64)(*next_value_index)++,
            };
            IrValue op_lhs   = lhs;
            IrValue op_rhs   = rhs;
            u32     lhs_type = ir_node_type_index(ast, sema, node->a);
            u32     rhs_type = ir_node_type_index(ast, sema, node->b);
            if (node->kind == AK_Greater || node->kind == AK_GreaterEqual) {
                op_lhs   = rhs;
                op_rhs   = lhs;
                lhs_type = ir_node_type_index(ast, sema, node->b);
                rhs_type = ir_node_type_index(ast, sema, node->a);
            }
            ir_add_binary(ir,
                          op,
                          value,
                          ir_node_type_index(ast, sema, node_index),
                          op_lhs,
                          lhs_type,
                          op_rhs,
                          rhs_type);
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
                                         IrLoopLabels   loop,
                                         Array(IrValue) node_values,
                                         u64* next_value_index,
                                         Ir*  ir)
{
    ASSERT(call_node->kind == AK_Call, "Expected call node");

    (void)ir_lower_call(
        lex, ast, sema, call_node, loop, node_values, next_value_index, ir);
}

//------------------------------------------------------------------------------
// Lower one explicit return statement.

internal void ir_generate_return_statement(const Lexer*   lex,
                                           const Ast*     ast,
                                           const Sema*    sema,
                                           const AstNode* return_node,
                                           IrLoopLabels   loop,
                                           Array(IrValue) node_values,
                                           u64* next_value_index,
                                           Ir*  ir)
{
    ASSERT(return_node->kind == AK_Return || return_node->kind == AK_ReturnExpr,
           "Expected return node");

    if (return_node->a == U32_MAX) {
        ir_add_return(ir,
                      (IrValue){.kind          = IR_VALUE_INTEGER,
                                .type          = ir_builtin_type(sema, STK_I32),
                                .value.integer = 0},
                      ir_builtin_type(sema, STK_I32));
        return;
    }

    IrValue value = ir_lower_node(lex,
                                  ast,
                                  sema,
                                  return_node->a,
                                  loop,
                                  node_values,
                                  next_value_index,
                                  ir);
    ir_add_return(ir, value, ir_node_type_index(ast, sema, return_node->a));
}

internal IrStatementResult ir_generate_statement(const Lexer* lex,
                                                 const Ast*   ast,
                                                 const Sema*  sema,
                                                 u32          function_index,
                                                 u32          node_index,
                                                 IrLoopLabels loop,
                                                 Array(IrValue) node_values,
                                                 u64* next_value_index,
                                                 Ir*  ir)
{
    const AstNode* node = &ast->nodes[node_index];

    if (node->kind == AK_Block) {
        ir_add_block_start(ir);
        for (u32 i = node->a; i < node->b; ++i) {
            if (ast->nodes[i].kind == AK_Block &&
                ir_block_is_expr_block_body(ast, i)) {
                i = ast->nodes[i].b - 1;
                continue;
            }
            if (!ast_node_is_block_statement(&ast->nodes[i])) {
                continue;
            }
            IrStatementResult result = ir_generate_statement(lex,
                                                             ast,
                                                             sema,
                                                             function_index,
                                                             i,
                                                             loop,
                                                             node_values,
                                                             next_value_index,
                                                             ir);
            if (result != IR_STMT_FALLTHROUGH) {
                ir_add_block_end(ir);
                return result;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }
        ir_add_block_end(ir);
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind == AK_Return) {
        ir_generate_return_statement(
            lex, ast, sema, node, loop, node_values, next_value_index, ir);
        return IR_STMT_RETURN;
    }

    if (node->kind == AK_Break) {
        const IrExprBlockTarget* expr_target =
            ir_find_expr_block_target(loop, node->b);
        if (node->a != U32_MAX || node->b != U32_MAX ||
            (loop.break_label < 0 && expr_target != NULL &&
             expr_target->result_type == ir_builtin_type(sema, STK_Void))) {
            ASSERT(expr_target != NULL, "Expected expression block target");
            if (node->a != U32_MAX) {
                IrValue value = ir_lower_node(lex,
                                              ast,
                                              sema,
                                              node->a,
                                              loop,
                                              node_values,
                                              next_value_index,
                                              ir);
                ir_add_assign(ir,
                              expr_target->result,
                              expr_target->result_type,
                              value,
                              ir_node_type_index(ast, sema, node->a));
            }
            ir_add_jump(ir, expr_target->break_label);
        } else {
            ASSERT(loop.break_label >= 0, "Expected loop break label");
            ir_add_jump(ir, loop.break_label);
        }
        return IR_STMT_BREAK;
    }

    if (node->kind == AK_Continue) {
        ASSERT(loop.continue_label >= 0, "Expected loop continue label");
        ir_add_jump(ir, loop.continue_label);
        return IR_STMT_CONTINUE;
    }

    if (node->kind == AK_For) {
        ASSERT(ast->nodes[node->b].kind == AK_Block, "Expected for body block");
        const AstForInfo* for_info = &ast->fors[node->a];
        for (u32 item = 0; item < for_info->init_count; ++item) {
            ir_generate_statement(lex,
                                  ast,
                                  sema,
                                  function_index,
                                  ast->for_items[for_info->first_init + item],
                                  loop,
                                  node_values,
                                  next_value_index,
                                  ir);
        }
        i64 start_label    = (i64)(*next_value_index)++;
        i64 continue_label = for_info->update_count > 0
                                 ? (i64)(*next_value_index)++
                                 : start_label;
        i64 end_label      = (i64)(*next_value_index)++;
        ir_add_label(ir, start_label);
        if (for_info->condition_node_index != U32_MAX) {
            IrValue condition = ir_lower_node(lex,
                                              ast,
                                              sema,
                                              for_info->condition_node_index,
                                              loop,
                                              node_values,
                                              next_value_index,
                                              ir);
            ir_add_branch_false(
                ir,
                condition,
                ir_node_type_index(ast, sema, for_info->condition_node_index),
                end_label);
        }
        IrLoopLabels inner_loop       = loop;
        inner_loop.break_label        = end_label;
        inner_loop.continue_label     = continue_label;
        IrStatementResult body_result = ir_generate_statement(lex,
                                                              ast,
                                                              sema,
                                                              function_index,
                                                              node->b,
                                                              inner_loop,
                                                              node_values,
                                                              next_value_index,
                                                              ir);
        if (for_info->condition_node_index == U32_MAX &&
            body_result == IR_STMT_RETURN) {
            return IR_STMT_RETURN;
        }
        if (body_result != IR_STMT_RETURN) {
            if (continue_label != start_label) {
                ir_add_label(ir, continue_label);
            }
            for (u32 item = 0; item < for_info->update_count; ++item) {
                ir_generate_statement(
                    lex,
                    ast,
                    sema,
                    function_index,
                    ast->for_items[for_info->first_update + item],
                    loop,
                    node_values,
                    next_value_index,
                    ir);
            }
            ir_add_jump(ir, start_label);
        }
        ir_add_label(ir, end_label);
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind == AK_Bind) {
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind == AK_Variable) {
        u32     local_index = sema->node_local_indices[node_index];
        u32     local_type  = ir_value_type_for_local_index(sema, local_index);
        IrValue value       = {
                  .kind = IR_VALUE_INTEGER, .type = local_type, .value.integer = 0};
        if (ast->nodes[node->b].kind == AK_AnnotatedValue) {
            value = ir_lower_node(lex,
                                  ast,
                                  sema,
                                  ast->nodes[node->b].b,
                                  loop,
                                  node_values,
                                  next_value_index,
                                  ir);
        } else if (ast->nodes[node->b].kind != AK_ZeroInit) {
            value = ir_lower_node(lex,
                                  ast,
                                  sema,
                                  node->b,
                                  loop,
                                  node_values,
                                  next_value_index,
                                  ir);
        }
        ir_add_local(
            ir, function_index, node->a, local_type, value, local_type);
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind == AK_Assign) {
        IrValue value = ir_lower_node(
            lex, ast, sema, node->b, loop, node_values, next_value_index, ir);
        u32     local_index = sema->node_local_indices[node_index];
        IrValue target =
            local_index != sema_no_local()
                ? (IrValue){.kind          = IR_VALUE_LOCAL,
                            .type          = ir_value_type_for_local_index(sema,
                                                                  local_index),
                            .value.integer = node->a}
                : (IrValue){.kind = IR_VALUE_SYMBOL,
                            .type = ir_value_type_for_decl(sema, node->a),
                            .value.integer = node->a};
        u32 target_type = local_index != sema_no_local()
                              ? ir_value_type_for_local_index(sema, local_index)
                              : ir_value_type_for_decl(sema, node->a);
        ir_add_assign(ir, target, target_type, value, target_type);
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind != AK_Statement) {
        return IR_STMT_FALLTHROUGH;
    }

    const AstNode* expr            = &ast->nodes[node->a];
    u32            expr_root_index = node->a;
    if (expr->kind == AK_Expression) {
        expr_root_index = expr->a;
        expr            = &ast->nodes[expr_root_index];
    }

    if (expr->kind == AK_Call) {
        ir_generate_call_statement(
            lex, ast, sema, expr, loop, node_values, next_value_index, ir);
    } else {
        (void)ir_lower_node(lex,
                            ast,
                            sema,
                            expr_root_index,
                            loop,
                            node_values,
                            next_value_index,
                            ir);
    }

    if (ir_node_contains_interpolation(ast, expr_root_index)) {
        ir_add_string_reset(ir);
    }

    return IR_STMT_FALLTHROUGH;
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
                               ir_no_control_labels(),
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

internal void ir_generate_nested_functions_in_range(const Lexer* lex,
                                                    const Ast*   ast,
                                                    const Sema*  sema,
                                                    u32          first_node,
                                                    u32          end_node,
                                                    Ir*          ir);

internal void ir_generate_function_body(const Lexer* lex,
                                        const Ast*   ast,
                                        const Sema*  sema,
                                        u32          symbol_handle,
                                        u32          type_index,
                                        u32          fn_def_index,
                                        u32          root_scope_index,
                                        Ir*          ir)
{
    const AstNode* fn_def_node   = &ast->nodes[fn_def_index];
    const AstNode* fn_start_node = &ast->nodes[fn_def_node->a];

    ASSERT(fn_def_node->kind == AK_FnDef, "Expected function definition");
    ASSERT(fn_start_node->kind == AK_FnStart, "Expected function start");
    ASSERT(fn_start_node->b > fn_def_node->a, "Expected valid function range");
    const AstFnSignature* signature = &ast->fn_signatures[fn_start_node->a];

    ir_generate_nested_functions_in_range(
        lex, ast, sema, fn_def_node->a + 1, fn_start_node->b, ir);

    u32 function_index = (u32)array_count(ir->functions);
    array_push(ir->functions,
               (IrFunction){
                   .symbol            = symbol_handle,
                   .type              = type_index,
                   .first_instruction = (u32)array_count(ir->instructions),
                   .first_local       = (u32)array_count(ir->locals),
                   .param_count       = signature->param_count,
               });

    ir_add_fn_start(ir, symbol_handle, type_index);

    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        u32             local_index =
            ir_find_function_param_local(sema, root_scope_index, param);
        ASSERT(local_index != sema_no_local(),
               "Expected semantic parameter local");
        ir_add_param(ir,
                     function_index,
                     param->symbol_handle,
                     sema->locals[local_index].type_index);
    }

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
                                       ir_no_control_labels(),
                                       node_values,
                                       &next_value_index,
                                       ir);
        ir_add_return(
            ir, result, ir_node_type_index(ast, sema, fn_start_node->b - 1));
    } else {
        bool has_explicit_return = false;

        for (u32 i = fn_def_node->a + 1; i < fn_start_node->b; ++i) {
            if (ast->nodes[i].kind == AK_Block &&
                ir_block_is_expr_block_body(ast, i)) {
                i = ast->nodes[i].b - 1;
                continue;
            }
            if (!ast_node_is_block_statement(&ast->nodes[i])) {
                continue;
            }
            IrStatementResult result =
                ir_generate_statement(lex,
                                      ast,
                                      sema,
                                      function_index,
                                      i,
                                      ir_no_control_labels(),
                                      node_values,
                                      &next_value_index,
                                      ir);
            if (result == IR_STMT_RETURN) {
                has_explicit_return = true;
                break;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
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
    IrFunction* function                = &ir->functions[function_index];
    function->one_past_last_instruction = (u32)array_count(ir->instructions);
    function->local_count =
        (u32)array_count(ir->locals) - function->first_local;
    array_free(node_values);
}

internal void ir_generate_function(const Lexer*    lex,
                                   const Ast*      ast,
                                   const Sema*     sema,
                                   const SemaDecl* decl,
                                   Ir*             ir)
{
    ir_generate_function_body(lex,
                              ast,
                              sema,
                              decl->symbol_handle,
                              decl->type_index,
                              decl->value_node_index,
                              sema->node_scope_indices[decl->value_node_index],
                              ir);
}

internal void ir_generate_nested_functions_in_range(const Lexer* lex,
                                                    const Ast*   ast,
                                                    const Sema*  sema,
                                                    u32          first_node,
                                                    u32          end_node,
                                                    Ir*          ir)
{
    for (u32 i = first_node; i < end_node; ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Block) {
            ir_generate_nested_functions_in_range(
                lex, ast, sema, node->a, node->b, ir);
            i = node->b - 1;
            continue;
        }
        if (node->kind == AK_For) {
            const AstNode* body = &ast->nodes[node->b];
            ASSERT(body->kind == AK_Block, "Expected for body block");
            ir_generate_nested_functions_in_range(
                lex, ast, sema, body->a, body->b, ir);
            i = body->b - 1;
            continue;
        }
        if (node->kind == AK_FnDef &&
            sema->node_lowered_symbol_handles[i] != U32_MAX) {
            ir_generate_function_body(lex,
                                      ast,
                                      sema,
                                      sema->node_lowered_symbol_handles[i],
                                      sema->node_type_indices[i],
                                      i,
                                      sema->node_scope_indices[i],
                                      ir);
            continue;
        }
    }
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
    for (u32 i = 0; i < array_count(sema->type_param_types); ++i) {
        array_push(ir.type_param_types, sema->type_param_types[i]);
    }

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        if (ir_decl_requires_runtime(sema, decl)) {
            has_constants = true;
            ir_add_global(&ir, decl->symbol_handle, decl->type_index);
        }
    }

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        u32             fn_value_node = decl->value_node_index;
        if (fn_value_node != sema_no_decl() &&
            ast->nodes[fn_value_node].kind == AK_Expression &&
            ast->nodes[ast->nodes[fn_value_node].a].kind == AK_FnDef) {
            fn_value_node = ast->nodes[fn_value_node].a;
        }

        if (decl->kind != SK_Function && fn_value_node != sema_no_decl() &&
            ast->nodes[fn_value_node].kind == AK_FnDef &&
            sema->node_lowered_symbol_handles[fn_value_node] != U32_MAX) {
            ir_generate_function_body(
                lex,
                ast,
                sema,
                sema->node_lowered_symbol_handles[fn_value_node],
                sema->node_type_indices[fn_value_node],
                fn_value_node,
                sema->node_scope_indices[fn_value_node],
                &ir);
        }
        if (decl->kind == SK_Function) {
            ir_generate_function(lex, ast, sema, decl, &ir);
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
    return ir;
}

//------------------------------------------------------------------------------
// Free the IR instruction table.

void ir_done(Ir* ir)
{
    array_free(ir->instructions);
    array_free(ir->globals);
    array_free(ir->functions);
    array_free(ir->locals);
    array_free(ir->call_args);
    array_free(ir->calls);
    array_free(ir->strings);
    array_free(ir->types);
    array_free(ir->type_param_types);
    if (ir->arena.data != NULL) {
        arena_done(&ir->arena);
    }
    *ir = (Ir){0};
}

//------------------------------------------------------------------------------
