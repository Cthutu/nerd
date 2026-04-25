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

void ir_add_tuple(Ir* ir, IrValue lvalue, u32 lvalue_type, Array(IrValue) items)
{
    u32             first_item = (u32)array_count(ir->tuple_items);
    const SemaType* tuple      = &ir->types[lvalue_type];
    for (u32 i = 0; i < array_count(items); ++i) {
        IrValue item = items[i];
        item.type    = ir->type_param_types[tuple->first_param_type + i];
        array_push(ir->tuple_items,
                   (IrTupleItem){
                       .value  = item,
                       .type   = item.type,
                       .symbol = U32_MAX,
                   });
    }

    u32 tuple_index = (u32)array_count(ir->tuples);
    array_push(ir->tuples,
               (IrTupleInfo){
                   .first_item = first_item,
                   .item_count = (u32)array_count(items),
               });
    lvalue.type = lvalue_type;
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_TUPLE,
            .lvalue = lvalue,
            .rvalue = {{.kind = IR_VALUE_INTEGER, .value.integer = tuple_index},
                       {0}},
        });
}

void ir_add_plex_items(Ir*     ir,
                       IrValue lvalue,
                       u32     lvalue_type,
                       Array(u32) symbols,
                       Array(IrValue) values);

void ir_add_plex(Ir*                       ir,
                 IrValue                   lvalue,
                 u32                       lvalue_type,
                 const Ast*                ast,
                 const AstPlexLiteralInfo* literal,
                 Array(IrValue) values)
{
    Array(u32) symbols = NULL;
    for (u32 i = 0; i < array_count(values); ++i) {
        const AstPlexLiteralField* field =
            &ast->plex_literal_fields[literal->first_field + i];
        array_push(symbols, field->symbol_handle);
    }
    ir_add_plex_items(ir, lvalue, lvalue_type, symbols, values);
    array_free(symbols);
}

void ir_add_plex_items(Ir*     ir,
                       IrValue lvalue,
                       u32     lvalue_type,
                       Array(u32) symbols,
                       Array(IrValue) values)
{
    u32             first_item = (u32)array_count(ir->tuple_items);
    const SemaType* plex       = &ir->types[lvalue_type];
    for (u32 i = 0; i < array_count(values); ++i) {
        u32 field_type = sema_no_type();
        for (u32 j = 0; j < plex->param_count; ++j) {
            if (ir->type_param_symbols[plex->first_param_type + j] ==
                symbols[i]) {
                field_type = ir->type_param_types[plex->first_param_type + j];
                break;
            }
        }
        IrValue item = values[i];
        item.type    = field_type;
        array_push(ir->tuple_items,
                   (IrTupleItem){
                       .value  = item,
                       .type   = item.type,
                       .symbol = symbols[i],
                   });
    }

    u32 plex_index = (u32)array_count(ir->tuples);
    array_push(ir->tuples,
               (IrTupleInfo){
                   .first_item = first_item,
                   .item_count = (u32)array_count(values),
               });
    lvalue.type = lvalue_type;
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_PLEX,
            .lvalue = lvalue,
            .rvalue = {{.kind = IR_VALUE_INTEGER, .value.integer = plex_index},
                       {0}},
        });
}

void ir_add_tuple_field(Ir*     ir,
                        IrValue lvalue,
                        u32     lvalue_type,
                        IrValue tuple,
                        u32     tuple_type,
                        u32     field_index)
{
    lvalue.type = lvalue_type;
    tuple.type  = tuple_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_TUPLE_FIELD,
                   .lvalue = lvalue,
                   .rvalue = {tuple,
                              {.kind          = IR_VALUE_INTEGER,
                               .value.integer = field_index}},
               });
}

void ir_add_enum(
    Ir* ir, IrValue lvalue, u32 lvalue_type, u32 variant_index, IrValue payload)
{
    lvalue.type = lvalue_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_ENUM,
                   .lvalue = lvalue,
                   .rvalue = {{.kind          = IR_VALUE_INTEGER,
                               .value.integer = variant_index},
                              payload},
               });
}

void ir_add_enum_payload(Ir*     ir,
                         IrValue lvalue,
                         u32     lvalue_type,
                         IrValue source,
                         u32     source_type,
                         u32     variant_index)
{
    lvalue.type = lvalue_type;
    source.type = source_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_ENUM_PAYLOAD,
                   .lvalue = lvalue,
                   .rvalue = {source,
                              {.kind          = IR_VALUE_INTEGER,
                               .value.integer = variant_index}},
               });
}

void ir_add_array(Ir* ir, IrValue lvalue, u32 lvalue_type, Array(IrValue) items)
{
    u32             first_item = (u32)array_count(ir->tuple_items);
    const SemaType* array      = &ir->types[lvalue_type];
    for (u32 i = 0; i < array_count(items); ++i) {
        IrValue item = items[i];
        item.type    = array->first_param_type;
        array_push(ir->tuple_items,
                   (IrTupleItem){
                       .value  = item,
                       .type   = item.type,
                       .symbol = U32_MAX,
                   });
    }

    u32 array_index = (u32)array_count(ir->tuples);
    array_push(ir->tuples,
               (IrTupleInfo){
                   .first_item = first_item,
                   .item_count = (u32)array_count(items),
               });
    lvalue.type = lvalue_type;
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_ARRAY,
            .lvalue = lvalue,
            .rvalue = {{.kind = IR_VALUE_INTEGER, .value.integer = array_index},
                       {0}},
        });
}

void ir_add_slice(Ir*     ir,
                  IrValue lvalue,
                  u32     lvalue_type,
                  IrValue target,
                  u32     target_type,
                  IrValue start,
                  u32     start_type,
                  IrValue end,
                  u32     end_type)
{
    lvalue.type     = lvalue_type;
    target.type     = target_type;
    start.type      = start_type;
    end.type        = end_type;
    u32 slice_index = (u32)array_count(ir->slices);
    array_push(ir->slices,
               (IrSliceInfo){
                   .target      = target,
                   .target_type = target_type,
                   .start       = start,
                   .start_type  = start_type,
                   .end         = end,
                   .end_type    = end_type,
               });
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_SLICE,
            .lvalue = lvalue,
            .rvalue = {{.kind = IR_VALUE_INTEGER, .value.integer = slice_index},
                       {0}},
        });
}

void ir_add_field(Ir*     ir,
                  IrValue lvalue,
                  u32     lvalue_type,
                  IrValue target,
                  u32     target_type,
                  u32     field_symbol)
{
    lvalue.type = lvalue_type;
    target.type = target_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_FIELD,
                   .lvalue = lvalue,
                   .rvalue = {target,
                              {.kind          = IR_VALUE_INTEGER,
                               .value.integer = field_symbol}},
               });
}

void ir_add_index(Ir*     ir,
                  IrValue lvalue,
                  u32     lvalue_type,
                  IrValue array,
                  u32     array_type,
                  IrValue index,
                  u32     index_type)
{
    lvalue.type = lvalue_type;
    array.type  = array_type;
    index.type  = index_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_INDEX,
                   .lvalue = lvalue,
                   .rvalue = {array, index},
               });
}

void ir_add_address_of(Ir* ir, IrValue lvalue, u32 lvalue_type, IrValue value)
{
    lvalue.type = lvalue_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_ADDRESS_OF,
                   .lvalue = lvalue,
                   .rvalue = {value, {0}},
               });
}

void ir_add_address_of_index(Ir*     ir,
                             IrValue lvalue,
                             u32     lvalue_type,
                             IrValue target,
                             u32     target_type,
                             IrValue index,
                             u32     index_type)
{
    lvalue.type = lvalue_type;
    target.type = target_type;
    index.type  = index_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_ADDRESS_OF_INDEX,
                   .lvalue = lvalue,
                   .rvalue = {target, index},
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
    u32 label_symbol;
    i64 break_label;
    i64 continue_label;
} IrLoopTarget;

typedef struct {
    i64               break_label;
    i64               continue_label;
    u32               expr_target_count;
    u32               loop_target_count;
    IrExprBlockTarget expr_targets[IR_MAX_EXPR_BLOCK_TARGETS];
    IrLoopTarget      loop_targets[IR_MAX_EXPR_BLOCK_TARGETS];
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

internal const IrLoopTarget* ir_find_loop_target(IrLoopLabels loop,
                                                 u32          label_symbol)
{
    for (u32 i = loop.loop_target_count; i > 0; --i) {
        const IrLoopTarget* target = &loop.loop_targets[i - 1];
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

internal u32 ir_enum_variant_index(const Sema* sema,
                                   u32         enum_type,
                                   u32         symbol_handle)
{
    if (enum_type == sema_no_type() ||
        sema->types[enum_type].kind != STK_Enum) {
        return U32_MAX;
    }
    const SemaType* type = &sema->types[enum_type];
    for (u32 i = 0; i < type->param_count; ++i) {
        if (sema->type_param_symbols[type->first_param_type + i] ==
            symbol_handle) {
            return i;
        }
    }
    return U32_MAX;
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

internal bool ir_pattern_contains_interpolation(const Ast* ast,
                                                u32        pattern_index);

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
    case AK_AddressOf:
    case AK_Field:
    case AK_Cast:
        return ir_node_contains_interpolation(ast, node->a);
    case AK_DestructureBind:
    case AK_DestructureVariable:
    case AK_DestructureAssign:
        return ir_node_contains_interpolation(ast, node->b);
    case AK_Slice:
        {
            const AstSliceInfo* slice = &ast->slices[node->a];
            return ir_node_contains_interpolation(ast,
                                                  slice->target_node_index) ||
                   (slice->start_node_index != U32_MAX &&
                    ir_node_contains_interpolation(ast,
                                                   slice->start_node_index)) ||
                   (slice->end_node_index != U32_MAX &&
                    ir_node_contains_interpolation(ast, slice->end_node_index));
        }
    case AK_Return:
    case AK_ReturnExpr:
        return node->a != U32_MAX &&
               ir_node_contains_interpolation(ast, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_Tuple:
    case AK_TupleField:
    case AK_Index:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_RangeExclusive:
    case AK_RangeInclusive:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            if (node->a != U32_MAX &&
                ir_node_contains_interpolation(ast, node->a)) {
                return true;
            }
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (!(branch->flags & AOBF_Else)) {
                    for (u32 pattern = 0; pattern < branch->pattern_count;
                         ++pattern) {
                        if (ir_pattern_contains_interpolation(
                                ast,
                                ast->pattern_items[branch->pattern_index +
                                                   pattern])) {
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
    case AK_Array:
        for (u32 i = 0; i < node->b; ++i) {
            if (ir_node_contains_interpolation(ast,
                                               ast->tuple_items[node->a + i])) {
                return true;
            }
        }
        return false;
    case AK_Plex:
    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            if (node->kind == AK_PlexUpdate &&
                ir_node_contains_interpolation(ast,
                                               literal->target_node_index)) {
                return true;
            }
            for (u32 i = 0; i < literal->field_count; ++i) {
                if (ir_node_contains_interpolation(
                        ast,
                        ast->plex_literal_fields[literal->first_field + i]
                            .value_node_index)) {
                    return true;
                }
            }
            return false;
        }
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
            return ir_node_contains_interpolation(ast, node->b) ||
                   (for_info->else_block_index != U32_MAX &&
                    ir_node_contains_interpolation(ast,
                                                   for_info->else_block_index));
        }
    default:
        return false;
    }
}

internal bool ir_pattern_contains_interpolation(const Ast* ast,
                                                u32        pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        return ir_node_contains_interpolation(ast, pattern->a);
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        return ir_node_contains_interpolation(ast, pattern->a) ||
               ir_node_contains_interpolation(ast, pattern->b);
    case APK_Bind:
        return pattern->b != U32_MAX &&
               ir_pattern_contains_interpolation(ast, pattern->b);
    case APK_Tuple:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (ir_pattern_contains_interpolation(
                    ast, ast->pattern_items[pattern->a + i])) {
                return true;
            }
        }
        return false;
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            if (ir_pattern_contains_interpolation(
                    ast, ast->pattern_fields[pattern->a + i].pattern_index)) {
                return true;
            }
        }
        return false;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                if (ir_pattern_contains_interpolation(
                        ast,
                        ast->pattern_items[enum_pattern->first_pattern + i])) {
                    return true;
                }
            }
            return false;
        }
    case APK_Ignore:
        return false;
    }
    return false;
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

internal u32 ir_current_function_index(const Ir* ir)
{
    ASSERT(array_count(ir->functions) > 0, "Expected current function");
    return (u32)array_count(ir->functions) - 1;
}

internal u32 ir_plex_field_type(const Sema* sema,
                                u32         plex_type,
                                u32         field_symbol)
{
    const SemaType* plex = &sema->types[plex_type];
    for (u32 i = 0; i < plex->param_count; ++i) {
        if (sema->type_param_symbols[plex->first_param_type + i] ==
            field_symbol) {
            return sema->type_param_types[plex->first_param_type + i];
        }
    }
    ASSERT(false, "Expected resolved plex field");
    return sema_no_type();
}

internal void ir_lower_on_pattern_match(const Lexer* lex,
                                        const Ast*   ast,
                                        const Sema*  sema,
                                        u32          pattern_index,
                                        IrValue      source,
                                        u32          source_type,
                                        i64          mismatch_label,
                                        IrLoopLabels loop,
                                        Array(IrValue) node_values,
                                        u64* next_value_index,
                                        Ir*  ir)
{
    const AstPattern* pattern   = &ast->patterns[pattern_index];
    u32               bool_type = ir_builtin_type(sema, STK_Bool);
    switch (pattern->kind) {
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
        {
            IrValue pattern_value = ir_lower_node(lex,
                                                  ast,
                                                  sema,
                                                  pattern->a,
                                                  loop,
                                                  node_values,
                                                  next_value_index,
                                                  ir);
            IrValue matches       = {
                      .kind          = IR_VALUE_VARIABLE,
                      .type          = bool_type,
                      .value.integer = (i64)(*next_value_index)++,
            };
            IrOperation op     = IR_OP_EQUAL;
            IrValue     lhs    = source;
            IrValue     rhs    = pattern_value;
            u32         lhs_ty = source_type;
            u32         rhs_ty = ir_node_type_index(ast, sema, pattern->a);
            if (pattern->kind == APK_NotEqual) {
                op = IR_OP_NOT_EQUAL;
            } else if (pattern->kind == APK_Less ||
                       pattern->kind == APK_Greater) {
                op = IR_OP_LESS;
            } else if (pattern->kind == APK_LessEqual ||
                       pattern->kind == APK_GreaterEqual) {
                op = IR_OP_LESS_EQUAL;
            }
            if (pattern->kind == APK_Greater ||
                pattern->kind == APK_GreaterEqual) {
                lhs    = pattern_value;
                rhs    = source;
                lhs_ty = ir_node_type_index(ast, sema, pattern->a);
                rhs_ty = source_type;
            }
            ir_add_binary(ir, op, matches, bool_type, lhs, lhs_ty, rhs, rhs_ty);
            ir_add_branch_false(ir, matches, bool_type, mismatch_label);
            return;
        }
    case APK_RangeExclusive:
    case APK_RangeInclusive:
        {
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
            ir_add_less_equal(ir,
                              start_matches,
                              bool_type,
                              start,
                              ir_node_type_index(ast, sema, pattern->a),
                              source,
                              source_type);
            ir_add_branch_false(ir, start_matches, bool_type, mismatch_label);

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
            if (pattern->kind == APK_RangeInclusive) {
                ir_add_less_equal(ir,
                                  end_matches,
                                  bool_type,
                                  source,
                                  source_type,
                                  end,
                                  ir_node_type_index(ast, sema, pattern->b));
            } else {
                ir_add_less(ir,
                            end_matches,
                            bool_type,
                            source,
                            source_type,
                            end,
                            ir_node_type_index(ast, sema, pattern->b));
            }
            ir_add_branch_false(ir, end_matches, bool_type, mismatch_label);
            return;
        }
    case APK_Bind:
        if (pattern->b != U32_MAX) {
            ir_lower_on_pattern_match(lex,
                                      ast,
                                      sema,
                                      pattern->b,
                                      source,
                                      source_type,
                                      mismatch_label,
                                      loop,
                                      node_values,
                                      next_value_index,
                                      ir);
        }
        return;
    case APK_Tuple:
        {
            const SemaType* tuple = &sema->types[source_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                u32 item_type =
                    sema->type_param_types[tuple->first_param_type + i];
                IrValue item = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = item_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_tuple_field(ir, item, item_type, source, source_type, i);
                ir_lower_on_pattern_match(lex,
                                          ast,
                                          sema,
                                          ast->pattern_items[pattern->a + i],
                                          item,
                                          item_type,
                                          mismatch_label,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
            }
            return;
        }
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            const AstPlexPatternField* field =
                &ast->pattern_fields[pattern->a + i];
            u32 field_type =
                ir_plex_field_type(sema, source_type, field->symbol_handle);
            IrValue field_value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = field_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_field(ir,
                         field_value,
                         field_type,
                         source,
                         source_type,
                         field->symbol_handle);
            ir_lower_on_pattern_match(lex,
                                      ast,
                                      sema,
                                      field->pattern_index,
                                      field_value,
                                      field_type,
                                      mismatch_label,
                                      loop,
                                      node_values,
                                      next_value_index,
                                      ir);
        }
        return;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            u32 variant = ir_enum_variant_index(
                sema, source_type, enum_pattern->symbol_handle);
            IrValue pattern_value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = source_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_enum(
                ir, pattern_value, source_type, variant, ir_unset_value());
            IrValue matches = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = bool_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_equal(ir,
                         matches,
                         bool_type,
                         source,
                         source_type,
                         pattern_value,
                         source_type);
            ir_add_branch_false(ir, matches, bool_type, mismatch_label);

            u32 payload_type =
                sema->type_param_types
                    [sema->types[source_type].first_param_type + variant];
            if (payload_type == sema_no_type()) {
                return;
            }
            IrValue payload = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = payload_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_enum_payload(
                ir, payload, payload_type, source, source_type, variant);
            if (enum_pattern->pattern_count == 1) {
                ir_lower_on_pattern_match(
                    lex,
                    ast,
                    sema,
                    ast->pattern_items[enum_pattern->first_pattern],
                    payload,
                    payload_type,
                    mismatch_label,
                    loop,
                    node_values,
                    next_value_index,
                    ir);
                return;
            }
            const SemaType* tuple = &sema->types[payload_type];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                u32 item_type =
                    sema->type_param_types[tuple->first_param_type + i];
                IrValue item = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = item_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_tuple_field(
                    ir, item, item_type, payload, payload_type, i);
                ir_lower_on_pattern_match(
                    lex,
                    ast,
                    sema,
                    ast->pattern_items[enum_pattern->first_pattern + i],
                    item,
                    item_type,
                    mismatch_label,
                    loop,
                    node_values,
                    next_value_index,
                    ir);
            }
            return;
        }
    case APK_Ignore:
        return;
    }
}

internal void ir_lower_on_pattern_binders(const Ast*  ast,
                                          const Sema* sema,
                                          u32         function_index,
                                          u32         pattern_index,
                                          IrValue     source,
                                          u32         source_type,
                                          u64*        next_value_index,
                                          Ir*         ir)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    if (pattern->kind == APK_Bind) {
        u32 local_index = sema->pattern_local_indices[pattern_index];
        ASSERT(local_index != sema_no_local(), "Expected on pattern binder");
        const SemaLocal* local = &sema->locals[local_index];
        ir_add_local(ir,
                     function_index,
                     local->lowered_symbol_handle,
                     ir_value_type_for_local_index(sema, local_index),
                     source,
                     source_type);
        if (pattern->b != U32_MAX) {
            ir_lower_on_pattern_binders(ast,
                                        sema,
                                        function_index,
                                        pattern->b,
                                        source,
                                        source_type,
                                        next_value_index,
                                        ir);
        }
        return;
    }

    switch (pattern->kind) {
    case APK_Tuple:
        {
            const SemaType* tuple = &sema->types[source_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                u32 item_type =
                    sema->type_param_types[tuple->first_param_type + i];
                IrValue item = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = item_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_tuple_field(ir, item, item_type, source, source_type, i);
                ir_lower_on_pattern_binders(ast,
                                            sema,
                                            function_index,
                                            ast->pattern_items[pattern->a + i],
                                            item,
                                            item_type,
                                            next_value_index,
                                            ir);
            }
            return;
        }
    case APK_Plex:
        for (u32 i = 0; i < pattern->b; ++i) {
            const AstPlexPatternField* field =
                &ast->pattern_fields[pattern->a + i];
            u32 field_type =
                ir_plex_field_type(sema, source_type, field->symbol_handle);
            IrValue field_value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = field_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_field(ir,
                         field_value,
                         field_type,
                         source,
                         source_type,
                         field->symbol_handle);
            ir_lower_on_pattern_binders(ast,
                                        sema,
                                        function_index,
                                        field->pattern_index,
                                        field_value,
                                        field_type,
                                        next_value_index,
                                        ir);
        }
        return;
    case APK_EnumVariant:
        {
            const AstEnumPattern* enum_pattern =
                &ast->enum_patterns[pattern->a];
            u32 variant = ir_enum_variant_index(
                sema, source_type, enum_pattern->symbol_handle);
            u32 payload_type =
                sema->type_param_types
                    [sema->types[source_type].first_param_type + variant];
            if (payload_type == sema_no_type()) {
                return;
            }
            IrValue payload = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = payload_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_enum_payload(
                ir, payload, payload_type, source, source_type, variant);
            if (enum_pattern->pattern_count == 1) {
                ir_lower_on_pattern_binders(
                    ast,
                    sema,
                    function_index,
                    ast->pattern_items[enum_pattern->first_pattern],
                    payload,
                    payload_type,
                    next_value_index,
                    ir);
                return;
            }
            const SemaType* tuple = &sema->types[payload_type];
            for (u32 i = 0; i < enum_pattern->pattern_count; ++i) {
                u32 item_type =
                    sema->type_param_types[tuple->first_param_type + i];
                IrValue item = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = item_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_tuple_field(
                    ir, item, item_type, payload, payload_type, i);
                ir_lower_on_pattern_binders(
                    ast,
                    sema,
                    function_index,
                    ast->pattern_items[enum_pattern->first_pattern + i],
                    item,
                    item_type,
                    next_value_index,
                    ir);
            }
            return;
        }
    case APK_Bind:
    case APK_Value:
    case APK_Equal:
    case APK_NotEqual:
    case APK_Less:
    case APK_LessEqual:
    case APK_Greater:
    case APK_GreaterEqual:
    case APK_RangeExclusive:
    case APK_RangeInclusive:
    case APK_Ignore:
        return;
    }
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
            IrValue value     = ir_unset_value();
            u32     node_type = ir_node_type_index(ast, sema, node_index);
            u32     variant   = ir_enum_variant_index(sema, node_type, node->a);
            if (variant != U32_MAX) {
                value = (IrValue){
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = node_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_enum(ir, value, node_type, variant, ir_unset_value());
                node_values[node_index] = value;
                return value;
            }
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
                } else if (local->kind == SLK_Binder &&
                           local->decl_node_index != sema_no_decl() &&
                           (ast->nodes[local->decl_node_index].kind ==
                                AK_DestructureBind ||
                            ast->nodes[local->decl_node_index].kind ==
                                AK_DestructureVariable)) {
                    value = (IrValue){
                        .kind = IR_VALUE_LOCAL,
                        .type =
                            ir_value_type_for_local_index(sema, local_index),
                        .value.integer = local->symbol_handle,
                    };
                } else if (local->kind == SLK_Binder &&
                           local->value_node_index == sema_no_decl()) {
                    value = (IrValue){
                        .kind = IR_VALUE_LOCAL,
                        .type =
                            ir_value_type_for_local_index(sema, local_index),
                        .value.integer = local->lowered_symbol_handle,
                    };
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
                                   .kind          = decl->kind == SK_BuiltinFunction ||
                                    decl->kind == SK_FfiFunction
                                                        ? IR_VALUE_BUILTIN
                                                        : IR_VALUE_SYMBOL,
                                   .type          = decl->type_index,
                                   .value.integer = decl->symbol_handle,
                };
            }
            node_values[node_index] = value;
            return value;
        }

    case AK_AddressOf:
        {
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            const AstNode* target_node = &ast->nodes[node->a];
            if (target_node->kind == AK_Index) {
                IrValue target = ir_lower_node(lex,
                                               ast,
                                               sema,
                                               target_node->a,
                                               loop,
                                               node_values,
                                               next_value_index,
                                               ir);
                IrValue index  = ir_lower_node(lex,
                                              ast,
                                              sema,
                                              target_node->b,
                                              loop,
                                              node_values,
                                              next_value_index,
                                              ir);
                ir_add_address_of_index(
                    ir,
                    value,
                    ir_node_type_index(ast, sema, node_index),
                    target,
                    ir_node_type_index(ast, sema, target_node->a),
                    index,
                    ir_node_type_index(ast, sema, target_node->b));
            } else {
                IrValue target = ir_lower_node(lex,
                                               ast,
                                               sema,
                                               node->a,
                                               loop,
                                               node_values,
                                               next_value_index,
                                               ir);
                ir_add_address_of(ir,
                                  value,
                                  ir_node_type_index(ast, sema, node_index),
                                  target);
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

    case AK_Tuple:
        {
            Array(IrValue) items = NULL;
            for (u32 i = 0; i < node->b; ++i) {
                array_push(items,
                           ir_lower_node(lex,
                                         ast,
                                         sema,
                                         ast->tuple_items[node->a + i],
                                         loop,
                                         node_values,
                                         next_value_index,
                                         ir));
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_tuple(
                ir, value, ir_node_type_index(ast, sema, node_index), items);
            array_free(items);
            node_values[node_index] = value;
            return value;
        }

    case AK_TupleField:
        {
            IrValue tuple = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          node->a,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_tuple_field(ir,
                               value,
                               ir_node_type_index(ast, sema, node_index),
                               tuple,
                               ir_node_type_index(ast, sema, node->a),
                               node->b);
            node_values[node_index] = value;
            return value;
        }

    case AK_Plex:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            Array(IrValue) values             = NULL;
            for (u32 i = 0; i < literal->field_count; ++i) {
                array_push(
                    values,
                    ir_lower_node(
                        lex,
                        ast,
                        sema,
                        ast->plex_literal_fields[literal->first_field + i]
                            .value_node_index,
                        loop,
                        node_values,
                        next_value_index,
                        ir));
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_plex(ir,
                        value,
                        ir_node_type_index(ast, sema, node_index),
                        ast,
                        literal,
                        values);
            array_free(values);
            node_values[node_index] = value;
            return value;
        }

    case AK_EnumVariant:
        {
            u32 type_index = ir_node_type_index(ast, sema, node_index);
            u32 variant    = ir_enum_variant_index(sema, type_index, node->a);
            ASSERT(variant != U32_MAX, "Expected valid enum variant");
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = type_index,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_enum(ir, value, type_index, variant, ir_unset_value());
            node_values[node_index] = value;
            return value;
        }

    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            IrValue                   base    = ir_lower_node(lex,
                                         ast,
                                         sema,
                                         literal->target_node_index,
                                         loop,
                                         node_values,
                                         next_value_index,
                                         ir);
            u32 type_index        = ir_node_type_index(ast, sema, node_index);
            const SemaType* plex  = &ir->types[type_index];
            Array(IrValue) values = NULL;
            Array(u32) symbols    = NULL;
            for (u32 field_index = 0; field_index < plex->param_count;
                 ++field_index) {
                u32 field_symbol =
                    ir->type_param_symbols[plex->first_param_type +
                                           field_index];
                u32 field_type =
                    ir->type_param_types[plex->first_param_type + field_index];
                u32 update_value_node = U32_MAX;
                for (u32 update = 0; update < literal->field_count; ++update) {
                    const AstPlexLiteralField* update_field =
                        &ast->plex_literal_fields[literal->first_field +
                                                  update];
                    if (update_field->symbol_handle == field_symbol) {
                        update_value_node = update_field->value_node_index;
                        break;
                    }
                }
                IrValue field_value = {0};
                if (update_value_node != U32_MAX) {
                    field_value = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                update_value_node,
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
                } else {
                    field_value = (IrValue){
                        .kind          = IR_VALUE_VARIABLE,
                        .type          = field_type,
                        .value.integer = (i64)(*next_value_index)++,
                    };
                    ir_add_field(ir,
                                 field_value,
                                 field_type,
                                 base,
                                 type_index,
                                 field_symbol);
                }
                array_push(values, field_value);
                array_push(symbols, field_symbol);
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = type_index,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_plex_items(ir, value, type_index, symbols, values);
            array_free(values);
            array_free(symbols);
            node_values[node_index] = value;
            return value;
        }

    case AK_Field:
        {
            u32 type_index = ir_node_type_index(ast, sema, node_index);
            u32 variant    = ir_enum_variant_index(sema, type_index, node->b);
            if (variant != U32_MAX) {
                IrValue value = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = type_index,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_enum(ir, value, type_index, variant, ir_unset_value());
                node_values[node_index] = value;
                return value;
            }
            IrValue target = ir_lower_node(lex,
                                           ast,
                                           sema,
                                           node->a,
                                           loop,
                                           node_values,
                                           next_value_index,
                                           ir);
            IrValue value  = {
                 .kind          = IR_VALUE_VARIABLE,
                 .type          = ir_node_type_index(ast, sema, node_index),
                 .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_field(ir,
                         value,
                         ir_node_type_index(ast, sema, node_index),
                         target,
                         ir_node_type_index(ast, sema, node->a),
                         node->b);
            node_values[node_index] = value;
            return value;
        }

    case AK_Array:
        {
            Array(IrValue) items = NULL;
            for (u32 i = 0; i < node->b; ++i) {
                array_push(items,
                           ir_lower_node(lex,
                                         ast,
                                         sema,
                                         ast->tuple_items[node->a + i],
                                         loop,
                                         node_values,
                                         next_value_index,
                                         ir));
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_array(
                ir, value, ir_node_type_index(ast, sema, node_index), items);
            array_free(items);
            node_values[node_index] = value;
            return value;
        }

    case AK_Slice:
        {
            const AstSliceInfo* slice      = &ast->slices[node->a];
            IrValue             target     = ir_lower_node(lex,
                                           ast,
                                           sema,
                                           slice->target_node_index,
                                           loop,
                                           node_values,
                                           next_value_index,
                                           ir);
            IrValue             start      = {0};
            u32                 start_type = sema_no_type();
            if (slice->start_node_index != U32_MAX) {
                start = ir_lower_node(lex,
                                      ast,
                                      sema,
                                      slice->start_node_index,
                                      loop,
                                      node_values,
                                      next_value_index,
                                      ir);
                start_type =
                    ir_node_type_index(ast, sema, slice->start_node_index);
            }
            IrValue end      = {0};
            u32     end_type = sema_no_type();
            if (slice->end_node_index != U32_MAX) {
                end      = ir_lower_node(lex,
                                    ast,
                                    sema,
                                    slice->end_node_index,
                                    loop,
                                    node_values,
                                    next_value_index,
                                    ir);
                end_type = ir_node_type_index(ast, sema, slice->end_node_index);
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_slice(
                ir,
                value,
                ir_node_type_index(ast, sema, node_index),
                target,
                ir_node_type_index(ast, sema, slice->target_node_index),
                start,
                start_type,
                end,
                end_type);
            node_values[node_index] = value;
            return value;
        }

    case AK_Index:
        {
            IrValue array = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          node->a,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
            IrValue index = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          node->b,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_index(ir,
                         value,
                         ir_node_type_index(ast, sema, node_index),
                         array,
                         ir_node_type_index(ast, sema, node->a),
                         index,
                         ir_node_type_index(ast, sema, node->b));
            node_values[node_index] = value;
            return value;
        }

    case AK_Call:
        {
            u32 type_index = ir_node_type_index(ast, sema, node_index);
            if (type_index != sema_no_type() &&
                sema->types[type_index].kind == STK_Enum) {
                const AstNode* callee = &ast->nodes[node->a];
                u32            variant_symbol =
                    callee->kind == AK_Field ? callee->b : callee->a;
                u32 variant =
                    ir_enum_variant_index(sema, type_index, variant_symbol);
                if (variant != U32_MAX) {
                    const AstCallInfo* call    = &ast->calls[node->b];
                    IrValue            payload = ir_unset_value();
                    u32                payload_type =
                        sema->type_param_types[sema->types[type_index]
                                                   .first_param_type +
                                               variant];
                    if (call->arg_count == 1) {
                        payload      = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                ast->call_args[call->first_arg],
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
                        payload.type = payload_type;
                    } else if (call->arg_count > 1) {
                        Array(IrValue) items = NULL;
                        for (u32 i = 0; i < call->arg_count; ++i) {
                            array_push(items,
                                       ir_lower_node(
                                           lex,
                                           ast,
                                           sema,
                                           ast->call_args[call->first_arg + i],
                                           loop,
                                           node_values,
                                           next_value_index,
                                           ir));
                        }
                        payload = (IrValue){
                            .kind          = IR_VALUE_VARIABLE,
                            .type          = payload_type,
                            .value.integer = (i64)(*next_value_index)++,
                        };
                        ir_add_tuple(ir, payload, payload_type, items);
                        array_free(items);
                    }
                    IrValue value = {
                        .kind          = IR_VALUE_VARIABLE,
                        .type          = type_index,
                        .value.integer = (i64)(*next_value_index)++,
                    };
                    ir_add_enum(ir, value, type_index, variant, payload);
                    node_values[node_index] = value;
                    return value;
                }
            }
            IrValue value = ir_lower_call(
                lex, ast, sema, node, loop, node_values, next_value_index, ir);
            node_values[node_index] = value;
            return value;
        }

    case AK_On:
        {
            const AstOnInfo* on        = &ast->ons[node->b];
            IrValue          scrutinee = ir_unset_value();
            if (on->kind != AOK_Condition) {
                scrutinee = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          node->a,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
            }
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

                if (on->kind == AOK_Condition) {
                    if (!(branch->flags & AOBF_Else)) {
                        IrValue condition =
                            ir_lower_node(lex,
                                          ast,
                                          sema,
                                          branch->guard_node_index,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
                        ir_add_branch_false(
                            ir,
                            condition,
                            ir_node_type_index(
                                ast, sema, branch->guard_node_index),
                            next_label);
                    }
                } else if (on->kind == AOK_Bool) {
                    if (!(branch->flags & AOBF_Else)) {
                        ir_add_branch_false(
                            ir,
                            scrutinee,
                            ir_node_type_index(ast, sema, node->a),
                            next_label);
                    }
                } else if (!(branch->flags & AOBF_Else)) {
                    i64 match_label = (i64)(*next_value_index)++;
                    for (u32 pattern_index = 0;
                         pattern_index < branch->pattern_count;
                         ++pattern_index) {
                        u32 pattern_index_row =
                            ast->pattern_items[branch->pattern_index +
                                               pattern_index];
                        i64 mismatch_label = next_label;
                        if (pattern_index + 1 < branch->pattern_count) {
                            mismatch_label = (i64)(*next_value_index)++;
                        }
                        ir_lower_on_pattern_match(
                            lex,
                            ast,
                            sema,
                            pattern_index_row,
                            scrutinee,
                            ir_node_type_index(ast, sema, node->a),
                            mismatch_label,
                            loop,
                            node_values,
                            next_value_index,
                            ir);
                        ir_lower_on_pattern_binders(
                            ast,
                            sema,
                            ir_current_function_index(ir),
                            pattern_index_row,
                            scrutinee,
                            ir_node_type_index(ast, sema, node->a),
                            next_value_index,
                            ir);
                        if (pattern_index + 1 < branch->pattern_count) {
                            ir_add_jump(ir, match_label);
                            ir_add_label(ir, mismatch_label);
                        }
                    }
                    ir_add_label(ir, match_label);
                }

                if (branch->guard_node_index != U32_MAX &&
                    on->kind != AOK_Condition) {
                    IrValue guard = ir_lower_node(lex,
                                                  ast,
                                                  sema,
                                                  branch->guard_node_index,
                                                  loop,
                                                  node_values,
                                                  next_value_index,
                                                  ir);
                    ir_add_branch_false(
                        ir,
                        guard,
                        ir_node_type_index(ast, sema, branch->guard_node_index),
                        next_label);
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
                    const AstNode* branch_node =
                        &ast->nodes[branch->expr_node_index];
                    const IrLoopTarget* loop_target =
                        ir_find_loop_target(loop, branch_node->b);
                    if (loop_target != NULL) {
                        ir_add_jump(ir, loop_target->break_label);
                    } else {
                        ASSERT(loop.break_label >= 0,
                               "Expected loop break label");
                        ir_add_jump(ir, loop.break_label);
                    }
                } else if (ast->nodes[branch->expr_node_index].kind ==
                               AK_Continue ||
                           ast->nodes[branch->expr_node_index].kind ==
                               AK_ContinueExpr) {
                    const AstNode* branch_node =
                        &ast->nodes[branch->expr_node_index];
                    const IrLoopTarget* loop_target =
                        ir_find_loop_target(loop, branch_node->b);
                    if (loop_target != NULL) {
                        ir_add_jump(ir, loop_target->continue_label);
                    } else {
                        ASSERT(loop.continue_label >= 0,
                               "Expected loop continue label");
                        ir_add_jump(ir, loop.continue_label);
                    }
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

    case AK_For:
        (void)ir_generate_statement(lex,
                                    ast,
                                    sema,
                                    U32_MAX,
                                    node_index,
                                    loop,
                                    node_values,
                                    next_value_index,
                                    ir);
        return node_values[node_index];

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

internal u32 ir_destructure_binder_symbol(const Ast* ast, u32 pattern_index)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    if (pattern->kind == APK_Bind) {
        return pattern->a;
    }
    if (pattern->kind == APK_Value) {
        const AstNode* node = &ast->nodes[pattern->a];
        if (node->kind == AK_SymbolRef) {
            return node->a;
        }
    }
    return U32_MAX;
}

internal void ir_lower_destructure_pattern(const Ast*  ast,
                                           const Sema* sema,
                                           u32         function_index,
                                           u32         pattern_index,
                                           IrValue     source,
                                           u32         source_type,
                                           u64*        next_value_index,
                                           Ir*         ir)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32               symbol = ir_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        u32 local_index = sema->pattern_local_indices[pattern_index];
        ASSERT(local_index != sema_no_local(),
               "Expected destructuring pattern local");
        u32 local_type = ir_value_type_for_local_index(sema, local_index);
        ir_add_local(
            ir, function_index, symbol, local_type, source, source_type);
        if (pattern->kind == APK_Bind && pattern->b != U32_MAX) {
            ir_lower_destructure_pattern(ast,
                                         sema,
                                         function_index,
                                         pattern->b,
                                         source,
                                         source_type,
                                         next_value_index,
                                         ir);
        }
        return;
    }

    switch (pattern->kind) {
    case APK_Ignore:
        return;
    case APK_Tuple:
        {
            const SemaType* tuple = &sema->types[source_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                u32 item_type =
                    sema->type_param_types[tuple->first_param_type + i];
                IrValue item = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = item_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_tuple_field(ir, item, item_type, source, source_type, i);
                ir_lower_destructure_pattern(ast,
                                             sema,
                                             function_index,
                                             ast->pattern_items[pattern->a + i],
                                             item,
                                             item_type,
                                             next_value_index,
                                             ir);
            }
            return;
        }
    case APK_Plex:
        {
            const SemaType* plex = &sema->types[source_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                const AstPlexPatternField* field =
                    &ast->pattern_fields[pattern->a + i];
                u32 field_type = sema_no_type();
                for (u32 j = 0; j < plex->param_count; ++j) {
                    if (sema->type_param_symbols[plex->first_param_type + j] ==
                        field->symbol_handle) {
                        field_type =
                            sema->type_param_types[plex->first_param_type + j];
                        break;
                    }
                }
                ASSERT(field_type != sema_no_type(),
                       "Expected resolved plex field");
                IrValue field_value = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = field_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_field(ir,
                             field_value,
                             field_type,
                             source,
                             source_type,
                             field->symbol_handle);
                ir_lower_destructure_pattern(ast,
                                             sema,
                                             function_index,
                                             field->pattern_index,
                                             field_value,
                                             field_type,
                                             next_value_index,
                                             ir);
            }
            return;
        }
    default:
        ASSERT(false, "Unsupported destructuring pattern");
        return;
    }
}

internal void ir_lower_destructure_assign_pattern(const Ast*  ast,
                                                  const Sema* sema,
                                                  u32         pattern_index,
                                                  IrValue     source,
                                                  u32         source_type,
                                                  u64*        next_value_index,
                                                  Ir*         ir)
{
    const AstPattern* pattern = &ast->patterns[pattern_index];
    u32               symbol = ir_destructure_binder_symbol(ast, pattern_index);
    if (symbol != U32_MAX) {
        u32 local_index = sema->pattern_local_indices[pattern_index];
        ASSERT(local_index != sema_no_local(),
               "Expected destructuring assignment target");
        u32     target_type = ir_value_type_for_local_index(sema, local_index);
        IrValue target      = {
                 .kind          = IR_VALUE_LOCAL,
                 .type          = target_type,
                 .value.integer = symbol,
        };
        ir_add_assign(ir, target, target_type, source, source_type);
        if (pattern->kind == APK_Bind && pattern->b != U32_MAX) {
            ir_lower_destructure_assign_pattern(ast,
                                                sema,
                                                pattern->b,
                                                source,
                                                source_type,
                                                next_value_index,
                                                ir);
        }
        return;
    }

    switch (pattern->kind) {
    case APK_Ignore:
        return;
    case APK_Tuple:
        {
            const SemaType* tuple = &sema->types[source_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                u32 item_type =
                    sema->type_param_types[tuple->first_param_type + i];
                IrValue item = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = item_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_tuple_field(ir, item, item_type, source, source_type, i);
                ir_lower_destructure_assign_pattern(
                    ast,
                    sema,
                    ast->pattern_items[pattern->a + i],
                    item,
                    item_type,
                    next_value_index,
                    ir);
            }
            return;
        }
    case APK_Plex:
        {
            const SemaType* plex = &sema->types[source_type];
            for (u32 i = 0; i < pattern->b; ++i) {
                const AstPlexPatternField* field =
                    &ast->pattern_fields[pattern->a + i];
                u32 field_type = sema_no_type();
                for (u32 j = 0; j < plex->param_count; ++j) {
                    if (sema->type_param_symbols[plex->first_param_type + j] ==
                        field->symbol_handle) {
                        field_type =
                            sema->type_param_types[plex->first_param_type + j];
                        break;
                    }
                }
                ASSERT(field_type != sema_no_type(),
                       "Expected resolved plex field");
                IrValue field_value = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = field_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_field(ir,
                             field_value,
                             field_type,
                             source,
                             source_type,
                             field->symbol_handle);
                ir_lower_destructure_assign_pattern(ast,
                                                    sema,
                                                    field->pattern_index,
                                                    field_value,
                                                    field_type,
                                                    next_value_index,
                                                    ir);
            }
            return;
        }
    default:
        ASSERT(false, "Unsupported destructuring assignment pattern");
        return;
    }
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
        const IrLoopTarget* loop_target = ir_find_loop_target(loop, node->b);
        if (node->a != U32_MAX || (node->b != U32_MAX && loop_target == NULL) ||
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
        } else if (loop_target != NULL) {
            ir_add_jump(ir, loop_target->break_label);
        } else {
            ASSERT(loop.break_label >= 0, "Expected loop break label");
            ir_add_jump(ir, loop.break_label);
        }
        return IR_STMT_BREAK;
    }

    if (node->kind == AK_Continue) {
        const IrLoopTarget* loop_target = ir_find_loop_target(loop, node->b);
        if (loop_target != NULL) {
            ir_add_jump(ir, loop_target->continue_label);
        } else {
            ASSERT(loop.continue_label >= 0, "Expected loop continue label");
            ir_add_jump(ir, loop.continue_label);
        }
        return IR_STMT_CONTINUE;
    }

    if (node->kind == AK_For) {
        ASSERT(ast->nodes[node->b].kind == AK_Block, "Expected for body block");
        const AstForInfo* for_info = &ast->fors[node->a];
        u32     result_type        = ir_node_type_index(ast, sema, node_index);
        IrValue result             = ir_unset_value();
        if (result_type != ir_builtin_type(sema, STK_Void)) {
            result = (IrValue){
                .kind          = IR_VALUE_VARIABLE,
                .type          = result_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_temp_local(ir, result, result_type);
        }
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
        i64 else_label     = for_info->else_block_index != U32_MAX
                                 ? (i64)(*next_value_index)++
                                 : end_label;
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
                else_label);
        }
        IrLoopLabels inner_loop   = loop;
        inner_loop.break_label    = end_label;
        inner_loop.continue_label = continue_label;
        ASSERT(inner_loop.loop_target_count < IR_MAX_EXPR_BLOCK_TARGETS,
               "Too many nested loop targets");
        inner_loop.loop_targets[inner_loop.loop_target_count++] =
            (IrLoopTarget){
                .label_symbol   = for_info->label_symbol,
                .break_label    = end_label,
                .continue_label = continue_label,
            };
        if (result_type != ir_builtin_type(sema, STK_Void)) {
            ASSERT(inner_loop.expr_target_count < IR_MAX_EXPR_BLOCK_TARGETS,
                   "Too many nested value targets");
            inner_loop.expr_targets[inner_loop.expr_target_count++] =
                (IrExprBlockTarget){
                    .label_symbol = for_info->label_symbol,
                    .break_label  = end_label,
                    .result       = result,
                    .result_type  = result_type,
                };
        }
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
        if (for_info->else_block_index != U32_MAX) {
            ir_add_label(ir, else_label);
            IrStatementResult else_result =
                ir_generate_statement(lex,
                                      ast,
                                      sema,
                                      function_index,
                                      for_info->else_block_index,
                                      inner_loop,
                                      node_values,
                                      next_value_index,
                                      ir);
            if (else_result == IR_STMT_RETURN) {
                return IR_STMT_RETURN;
            }
        }
        ir_add_label(ir, end_label);
        node_values[node_index] = result;
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

    if (node->kind == AK_DestructureBind ||
        node->kind == AK_DestructureVariable ||
        node->kind == AK_DestructureAssign) {
        u32 value_node = node->b;
        if (ast->nodes[value_node].kind == AK_AnnotatedValue) {
            value_node = ast->nodes[value_node].b;
        }
        IrValue value = ir_lower_node(lex,
                                      ast,
                                      sema,
                                      value_node,
                                      loop,
                                      node_values,
                                      next_value_index,
                                      ir);
        if (node->kind == AK_DestructureAssign) {
            ir_lower_destructure_assign_pattern(
                ast,
                sema,
                node->a,
                value,
                ir_node_type_index(ast, sema, value_node),
                next_value_index,
                ir);
        } else {
            ir_lower_destructure_pattern(
                ast,
                sema,
                function_index,
                node->a,
                value,
                ir_node_type_index(ast, sema, value_node),
                next_value_index,
                ir);
        }
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
    for (u32 i = 0; i < array_count(sema->type_param_symbols); ++i) {
        array_push(ir.type_param_symbols, sema->type_param_symbols[i]);
    }

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        if (decl->kind == SK_FfiFunction) {
            const AstNode*    ffi_node = &ast->nodes[decl->value_node_index];
            const AstFfiInfo* ffi_info = &ast->ffi_infos[ffi_node->a];
            array_push(
                ir.externs,
                (IrExtern){
                    .symbol               = decl->symbol_handle,
                    .type                 = decl->type_index,
                    .library_string_index = ffi_info->library_string_index,
                });
        }
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
    array_free(ir->externs);
    array_free(ir->functions);
    array_free(ir->locals);
    array_free(ir->call_args);
    array_free(ir->calls);
    array_free(ir->tuple_items);
    array_free(ir->tuples);
    array_free(ir->slices);
    array_free(ir->strings);
    array_free(ir->types);
    array_free(ir->type_param_types);
    array_free(ir->type_param_symbols);
    if (ir->arena.data != NULL) {
        arena_done(&ir->arena);
    }
    *ir = (Ir){0};
}

//------------------------------------------------------------------------------
