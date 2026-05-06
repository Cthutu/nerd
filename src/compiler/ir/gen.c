//------------------------------------------------------------------------------
// IR generation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include "compiler/ir/ir.h"
#include <compiler/build/build.h>
#include <compiler/error/error.h>

#include <stdio.h>

//------------------------------------------------------------------------------
// AST ownership helpers.

internal u32 g_ir_current_global_init_decl = U32_MAX;

typedef struct {
    u32     local_index;
    IrValue value;
} IrLocalSubstitution;

internal Array(IrLocalSubstitution) g_ir_local_substitutions = NULL;

internal bool ir_symbol_is_discard(const Lexer* lexer, u32 symbol_handle)
{
    return string_eq(lex_symbol(lexer, symbol_handle), s("_"));
}

internal bool ir_imported_ffi_decl_source(const Sema*      sema,
                                          const SemaDecl*  decl,
                                          const Lexer**    out_lexer,
                                          const Ast**      out_ast,
                                          const Sema**     out_sema,
                                          const SemaDecl** out_decl)
{
    if (decl->import_module_index == sema_no_decl() ||
        decl->import_decl_index == sema_no_decl() || sema->program == NULL ||
        decl->import_module_index >= array_count(sema->program->modules)) {
        return false;
    }

    const ModuleInfo* module =
        &sema->program->modules[decl->import_module_index];
    if (decl->import_decl_index >= array_count(module->front_end.sema.decls)) {
        return false;
    }

    const SemaDecl* source_decl =
        &module->front_end.sema.decls[decl->import_decl_index];
    if (source_decl->kind != SK_FfiFunction ||
        source_decl->value_node_index == sema_no_decl()) {
        return false;
    }

    *out_lexer = &module->front_end.lexer;
    *out_ast   = &module->front_end.ast;
    *out_sema  = &module->front_end.sema;
    *out_decl  = source_decl;
    return true;
}

internal bool ir_imported_decl_source(const Sema*      sema,
                                      const SemaDecl*  decl,
                                      const Lexer**    out_lexer,
                                      const Ast**      out_ast,
                                      const Sema**     out_sema,
                                      const SemaDecl** out_decl)
{
    if (decl->import_module_index == sema_no_decl() ||
        decl->import_decl_index == sema_no_decl() || sema->program == NULL ||
        decl->import_module_index >= array_count(sema->program->modules)) {
        return false;
    }

    const ModuleInfo* module =
        &sema->program->modules[decl->import_module_index];
    if (decl->import_decl_index >= array_count(module->front_end.sema.decls)) {
        return false;
    }

    *out_lexer = &module->front_end.lexer;
    *out_ast   = &module->front_end.ast;
    *out_sema  = &module->front_end.sema;
    *out_decl  = &module->front_end.sema.decls[decl->import_decl_index];
    return true;
}

internal bool ir_function_symbol_conflicts(const ProgramInfo* program,
                                           u32                module_index,
                                           u32                symbol)
{
    if (program == NULL || module_index >= array_count(program->modules)) {
        return false;
    }
    const ModuleInfo* module  = &program->modules[module_index];
    string            name    = lex_symbol(&module->front_end.lexer, symbol);
    u32               matches = 0;
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        const ModuleInfo* other = &program->modules[i];
        for (u32 export = 0; export < array_count(other->export_decl_indices);
             ++export) {
            u32 decl_index = other->export_decl_indices[export];
            if (decl_index >= array_count(other->front_end.sema.decls)) {
                continue;
            }
            const SemaDecl* decl = &other->front_end.sema.decls[decl_index];
            if (decl->kind == SK_Function &&
                string_eq(
                    name,
                    lex_symbol(&other->front_end.lexer, decl->symbol_handle))) {
                matches++;
            }
        }
    }
    return matches > 1;
}

internal u32 ir_module_qualified_symbol(Lexer*            lexer,
                                        const ModuleInfo* module,
                                        u32               source_symbol)
{
    string source_name = lex_symbol(&module->front_end.lexer, source_symbol);
    StringBuilder sb   = {0};
    sb_init(&sb, &temp_arena);
    for (usize i = 0; i < module->qualified_name.count; ++i) {
        u8 ch = module->qualified_name.data[i];
        sb_append_char(&sb, ch == '.' ? '$' : (char)ch);
    }
    sb_append_char(&sb, '$');
    sb_append_string(&sb, source_name);
    InternAddResult ignored = {0};
    return lex_add_symbol(lexer, sb_to_string(&sb), &ignored);
}

internal bool ir_decl_ffi_info(const Lexer*       lex,
                               const Ast*         ast,
                               const Sema*        sema,
                               const SemaDecl*    decl,
                               const Lexer**      out_lexer,
                               const Ast**        out_ast,
                               const Sema**       out_sema,
                               const AstFfiInfo** out_info)
{
    const Lexer*    source_lexer = lex;
    const Ast*      source_ast   = ast;
    const Sema*     source_sema  = sema;
    const SemaDecl* source_decl  = decl;

    if (decl->value_node_index == sema_no_decl() &&
        !ir_imported_ffi_decl_source(sema,
                                     decl,
                                     &source_lexer,
                                     &source_ast,
                                     &source_sema,
                                     &source_decl)) {
        return false;
    }

    if (source_decl->value_node_index == sema_no_decl()) {
        return false;
    }
    const AstNode* ffi_node = &source_ast->nodes[source_decl->value_node_index];
    if (ffi_node->kind != AK_FfiDef) {
        return false;
    }

    *out_lexer = source_lexer;
    *out_ast   = source_ast;
    *out_sema  = source_sema;
    *out_info  = &source_ast->ffi_infos[ffi_node->a];
    return true;
}

internal u32 ir_unwrap_expr_node(const Ast* ast, u32 node_index)
{
    while (node_index < array_count(ast->nodes) &&
           (ast->nodes[node_index].kind == AK_Expression ||
            ast->nodes[node_index].kind == AK_Statement)) {
        node_index = ast->nodes[node_index].a;
    }
    return node_index;
}

internal bool ir_address_path_targets_decl(const Ast*  ast,
                                           const Sema* sema,
                                           u32         node_index,
                                           u32         target_decl_index)
{
    node_index          = ir_unwrap_expr_node(ast, node_index);
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_SymbolRef:
        return sema->node_local_indices[node_index] == sema_no_local() &&
               sema->node_decl_indices[node_index] == target_decl_index;
    case AK_Field:
    case AK_TupleField:
        return ir_address_path_targets_decl(
            ast, sema, node->a, target_decl_index);
    case AK_Index:
        return ir_address_path_targets_decl(
            ast, sema, node->a, target_decl_index);
    default:
        return false;
    }
}

internal bool ir_decl_is_pointer_alias_to_decl(const Ast*  ast,
                                               const Sema* sema,
                                               u32         decl_index,
                                               u32         target_decl_index)
{
    if (decl_index == sema_no_decl() ||
        decl_index >= array_count(sema->decls)) {
        return false;
    }
    const SemaDecl* decl = &sema->decls[decl_index];
    if (decl->kind != SK_Constant || decl->value_node_index == sema_no_decl()) {
        return false;
    }

    u32 value_node_index = ir_unwrap_expr_node(ast, decl->value_node_index);
    if (value_node_index >= array_count(ast->nodes) ||
        ast->nodes[value_node_index].kind != AK_AddressOf) {
        return false;
    }
    return ir_address_path_targets_decl(
        ast, sema, ast->nodes[value_node_index].a, target_decl_index);
}

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

void ir_add_store(
    Ir* ir, IrValue pointer, u32 pointer_type, IrValue rvalue, u32 rvalue_type)
{
    pointer.type = pointer_type;
    rvalue.type  = rvalue_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_STORE,
                   .lvalue = pointer,
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

void          ir_add_plex_items(Ir*     ir,
                                IrValue lvalue,
                                u32     lvalue_type,
                                Array(u32) symbols,
                                Array(IrValue) values);
internal bool ir_decl_uses_implicit_array_slice(const Ast*      ast,
                                                const Sema*     sema,
                                                const SemaDecl* decl);
internal u32  ir_decl_implicit_array_slice_type(const Ast*      ast,
                                                const Sema*     sema,
                                                const SemaDecl* decl);
internal u32  ir_hidden_backing_symbol(Lexer* lexer, u32 symbol_handle);

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

void ir_add_size(Ir* ir, IrValue lvalue, u32 lvalue_type, u32 source_type)
{
    lvalue.type = lvalue_type;
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = IR_OP_SIZE,
            .lvalue = lvalue,
            .rvalue = {{.kind = IR_VALUE_INTEGER, .value.integer = source_type},
                       {0}},
        });
}

void ir_add_dynarray_op(Ir*                      ir,
                        IrOperation              op,
                        IrDynamicArrayTargetKind target_kind,
                        IrValue                  target,
                        u32                      target_type,
                        u32                      field_symbol,
                        IrValue                  arg,
                        u32                      arg_type,
                        u32                      dynarray_type)
{
    target.type  = target_type;
    arg.type     = arg_type;
    u32 op_index = (u32)array_count(ir->dynarray_ops);
    array_push(ir->dynarray_ops,
               (IrDynamicArrayOpInfo){
                   .target_kind   = target_kind,
                   .target        = target,
                   .target_type   = target_type,
                   .field_symbol  = field_symbol,
                   .arg           = arg,
                   .arg_type      = arg_type,
                   .dynarray_type = dynarray_type,
               });
    array_push(
        ir->instructions,
        (IrInstruction){
            .op     = op,
            .lvalue = {.kind = IR_VALUE_INTEGER, .value.integer = op_index},
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

void ir_add_store_field(Ir*     ir,
                        IrValue target,
                        u32     target_type,
                        u32     field_symbol,
                        IrValue rvalue,
                        u32     rvalue_type)
{
    target.type = target_type;
    rvalue.type = rvalue_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_STORE_FIELD,
                   .lvalue = target,
                   .rvalue = {{.kind          = IR_VALUE_INTEGER,
                               .value.integer = field_symbol},
                              rvalue},
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

void ir_add_address_of_field_ptr(Ir*     ir,
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
                   .op     = IR_OP_ADDRESS_OF_FIELD_PTR,
                   .lvalue = lvalue,
                   .rvalue = {target,
                              {.kind          = IR_VALUE_INTEGER,
                               .value.integer = field_symbol}},
               });
}

void ir_add_address_of_index_ptr(Ir*     ir,
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
                   .op     = IR_OP_ADDRESS_OF_INDEX_PTR,
                   .lvalue = lvalue,
                   .rvalue = {target, index},
               });
}

void ir_add_add(Ir*     ir,
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
                   .op     = IR_OP_ADD,
                   .lvalue = lvalue,
                   .rvalue = {lhs, rhs},
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

internal u32 ir_add_string_literal(Ir* ir, string text);

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

void ir_add_assert(Ir*     ir,
                   IrValue condition,
                   u32     condition_type,
                   IrValue message,
                   u32     message_type,
                   string  source_path,
                   u32     line)
{
    condition.type = condition_type;
    message.type   = message_type;
    array_push(ir->instructions,
               (IrInstruction){
                   .op     = IR_OP_ASSERT,
                   .lvalue = message,
                   .rvalue = {condition,
                              {.kind = IR_VALUE_INTEGER, .value.integer = line},
                              {.kind = IR_VALUE_STRING,
                               .value.integer =
                                   ir_add_string_literal(ir, source_path)}},
               });
}

internal void ir_source_location_for_offset(NerdSource source,
                                            usize      offset,
                                            string*    out_source_path,
                                            u32*       out_line)
{
    NerdSource mapped_source = source;
    usize      mapped_offset = offset;
    for (u32 i = 0; i < array_count(source.fragments); ++i) {
        NerdSourceFragment fragment = source.fragments[i];
        if (offset < fragment.start || offset >= fragment.end) {
            continue;
        }

        usize source_prefix_start = fragment.start - fragment.source_start;
        usize source_count        = fragment.end - source_prefix_start;
        mapped_source             = (NerdSource){
            .source      = string_from(source.source.data + source_prefix_start,
                                       source_count),
            .source_path = fragment.source_path,
        };
        mapped_offset = offset - fragment.start + fragment.source_start;
        break;
    }

    u32 col = 0;
    lex_offset_to_line_col(mapped_source, mapped_offset, out_line, &col);
    *out_line += 1;
    *out_source_path = mapped_source.source_path;
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
    u32     defer_count;
} IrExprBlockTarget;

typedef struct {
    u32 label_symbol;
    i64 break_label;
    i64 continue_label;
    u32 defer_count;
} IrLoopTarget;

typedef struct {
    Array(u32) statements;
} IrDeferStack;

typedef struct {
    i64               break_label;
    i64               continue_label;
    IrDeferStack*     defers;
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

internal IrStatementResult ir_generate_statement(const Lexer* lex,
                                                 const Ast*   ast,
                                                 const Sema*  sema,
                                                 u32          function_index,
                                                 u32          node_index,
                                                 IrLoopLabels loop,
                                                 Array(IrValue) node_values,
                                                 u64*          next_value_index,
                                                 IrDeferStack* defers,
                                                 Ir*           ir);
internal                   Array(IrValue) ir_make_node_values(const Ast* ast);

internal void ir_emit_defers(const Lexer* lex,
                             const Ast*   ast,
                             const Sema*  sema,
                             u32          function_index,
                             IrLoopLabels loop,
                             Array(IrValue) node_values,
                             u64*          next_value_index,
                             u32           base_defer_count,
                             IrDeferStack* defers,
                             Ir*           ir)
{
    UNUSED(node_values);
    while (array_count(defers->statements) > base_defer_count) {
        u32 deferred_statement      = array_pop(defers->statements);
        Array(IrValue) defer_values = ir_make_node_values(ast);
        (void)ir_generate_statement(lex,
                                    ast,
                                    sema,
                                    function_index,
                                    deferred_statement,
                                    loop,
                                    defer_values,
                                    next_value_index,
                                    defers,
                                    ir);
        array_free(defer_values);
    }
}

internal void ir_emit_defers_no_pop(const Lexer* lex,
                                    const Ast*   ast,
                                    const Sema*  sema,
                                    u32          function_index,
                                    IrLoopLabels loop,
                                    Array(IrValue) node_values,
                                    u64*          next_value_index,
                                    u32           base_defer_count,
                                    IrDeferStack* defers,
                                    Ir*           ir)
{
    UNUSED(node_values);
    for (u32 i = (u32)array_count(defers->statements); i > base_defer_count;
         --i) {
        Array(IrValue) defer_values = ir_make_node_values(ast);
        (void)ir_generate_statement(lex,
                                    ast,
                                    sema,
                                    function_index,
                                    defers->statements[i - 1],
                                    loop,
                                    defer_values,
                                    next_value_index,
                                    defers,
                                    ir);
        array_free(defer_values);
    }
}

internal bool ir_ast_break_targets_loop(const AstNode* node,
                                        u32            loop_label,
                                        bool           allow_unlabelled)
{
    return node->b == loop_label || (allow_unlabelled && node->b == U32_MAX);
}

internal bool ir_ast_node_may_break_loop(const Ast* ast,
                                         u32        node_index,
                                         u32        loop_label,
                                         bool       allow_unlabelled)
{
    if (node_index >= array_count(ast->nodes)) {
        return false;
    }

    const AstNode* node = &ast->nodes[node_index];
    switch (node->kind) {
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            if (ir_ast_node_may_break_loop(
                    ast, i, loop_label, allow_unlabelled)) {
                return true;
            }
        }
        return false;
    case AK_Statement:
    case AK_Expression:
        return ir_ast_node_may_break_loop(
            ast, node->a, loop_label, allow_unlabelled);
    case AK_On:
        {
            const AstOnInfo* on = &ast->ons[node->b];
            for (u32 i = 0; i < on->branch_count; ++i) {
                const AstOnBranch* branch =
                    &ast->on_branches[on->first_branch + i];
                if (ir_ast_node_may_break_loop(ast,
                                               branch->expr_node_index,
                                               loop_label,
                                               allow_unlabelled)) {
                    return true;
                }
            }
            return false;
        }
    case AK_Break:
    case AK_BreakExpr:
        return ir_ast_break_targets_loop(node, loop_label, allow_unlabelled);
    case AK_For:
        {
            const AstForInfo* for_info = &ast->fors[node->a];
            return ir_ast_node_may_break_loop(
                       ast, node->b, loop_label, false) ||
                   (for_info->else_block_index != U32_MAX &&
                    ir_ast_node_may_break_loop(
                        ast, for_info->else_block_index, loop_label, false));
        }
    default:
        return false;
    }
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

internal bool ir_find_local_substitution(u32 local_index, IrValue* out_value)
{
    for (u32 i = array_count(g_ir_local_substitutions); i > 0; --i) {
        const IrLocalSubstitution* substitution =
            &g_ir_local_substitutions[i - 1];
        if (substitution->local_index == local_index) {
            *out_value = substitution->value;
            return true;
        }
    }
    return false;
}

internal u32 ir_signature_required_param_count(const Ast*            ast,
                                               const AstFnSignature* signature)
{
    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        if (param->default_node_index != U32_MAX) {
            return i;
        }
    }
    return signature->param_count;
}

internal bool ir_known_call_fn_node(const Ast*  ast,
                                    const Sema* sema,
                                    u32         callee_node_index,
                                    u32*        out_fn_node_index)
{
    callee_node_index     = ir_unwrap_expr_node(ast, callee_node_index);
    const AstNode* callee = &ast->nodes[callee_node_index];
    if (callee->kind != AK_SymbolRef) {
        return false;
    }

    if (callee_node_index < array_count(sema->node_local_indices)) {
        u32 local_index = sema->node_local_indices[callee_node_index];
        if (local_index != sema_no_local()) {
            const SemaLocal* local = &sema->locals[local_index];
            if (local->kind == SLK_Function &&
                local->value_node_index != sema_no_decl() &&
                ast->nodes[local->value_node_index].kind == AK_FnDef) {
                *out_fn_node_index = local->value_node_index;
                return true;
            }
        }
    }

    if (callee_node_index < array_count(sema->node_decl_indices)) {
        u32 decl_index = sema->node_decl_indices[callee_node_index];
        if (decl_index != sema_no_decl()) {
            const SemaDecl* decl = &sema->decls[decl_index];
            if (decl->kind == SK_Function &&
                decl->value_node_index != sema_no_decl() &&
                ast->nodes[decl->value_node_index].kind == AK_FnDef) {
                *out_fn_node_index = decl->value_node_index;
                return true;
            }
        }
    }

    return false;
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

internal bool ir_try_lower_contextual_enum_variant(const Ast*  ast,
                                                   const Sema* sema,
                                                   u32         node_index,
                                                   u32         type_index,
                                                   u64*        next_value_index,
                                                   IrValue*    out_value,
                                                   Ir*         ir)
{
    if (type_index == sema_no_type() ||
        sema->types[type_index].kind != STK_Enum) {
        return false;
    }

    const AstNode* node           = &ast->nodes[node_index];
    u32            variant_symbol = U32_MAX;
    if (node->kind == AK_SymbolRef) {
        variant_symbol = node->a;
    } else if (node->kind == AK_Field) {
        variant_symbol = node->b;
    } else {
        return false;
    }

    u32 variant = ir_enum_variant_index(sema, type_index, variant_symbol);
    if (variant == U32_MAX) {
        return false;
    }

    IrValue value = {
        .kind          = IR_VALUE_VARIABLE,
        .type          = type_index,
        .value.integer = (i64)(*next_value_index)++,
    };
    ir_add_enum(ir, value, type_index, variant, ir_unset_value());
    *out_value = value;
    return true;
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

internal bool ir_type_kind_is_integer(SemaTypeKind kind)
{
    return kind == STK_UntypedInteger || kind == STK_I8 || kind == STK_I16 ||
           kind == STK_I32 || kind == STK_I64 || kind == STK_U8 ||
           kind == STK_U16 || kind == STK_U32 || kind == STK_U64 ||
           kind == STK_Isize || kind == STK_Usize;
}

internal bool ir_type_kind_is_float(SemaTypeKind kind)
{
    return kind == STK_UntypedFloat || kind == STK_F32 || kind == STK_F64;
}

internal bool ir_eval_interpolation_part_constant(const Lexer*   lex,
                                                  const Ast*     ast,
                                                  const Sema*    sema,
                                                  Ir*            ir,
                                                  u32            node_index,
                                                  StringBuilder* sb);

internal bool ir_eval_string_constant(const Lexer* lex,
                                      const Ast*   ast,
                                      const Sema*  sema,
                                      Ir*          ir,
                                      u32          node_index,
                                      string*      out)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_StringLiteral:
        *out = ast_get_string(lex, node);
        return true;
    case AK_StringConcat:
        {
            string lhs = {0};
            string rhs = {0};
            if (!ir_eval_string_constant(lex, ast, sema, ir, node->a, &lhs) ||
                !ir_eval_string_constant(lex, ast, sema, ir, node->b, &rhs)) {
                return false;
            }

            u32 string_index = ir_add_concat_string(ir, lhs, rhs);
            *out             = ir->strings[string_index];
            return true;
        }
    case AK_InterpolatedString:
        {
            StringBuilder sb = {0};
            sb_init(&sb, &ir->arena);

            for (u32 i = node->a; i < node->b; ++i) {
                const AstNode* part = &ast->nodes[i];
                if (part->kind == AK_StringLiteral) {
                    sb_append_string(&sb, ast_get_string(lex, part));
                    continue;
                }

                ASSERT(part->kind == AK_InterpPartExpr,
                       "Expected interpolated string part expression");
                if (!ir_eval_interpolation_part_constant(
                        lex, ast, sema, ir, part->a, &sb)) {
                    return false;
                }
            }

            *out = sb_to_string(&sb);
            ir_add_string_literal(ir, *out);
            return true;
        }
    case AK_Expression:
    case AK_Statement:
    case AK_InterpPartExpr:
        return ir_eval_string_constant(lex, ast, sema, ir, node->a, out);
    case AK_SymbolRef:
        if (node_index < array_count(sema->node_local_indices)) {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index != sema_no_local() &&
                sema->locals[local_index].kind == SLK_Constant &&
                sema->locals[local_index].value_node_index != sema_no_decl()) {
                return ir_eval_string_constant(
                    lex,
                    ast,
                    sema,
                    ir,
                    sema->locals[local_index].value_node_index,
                    out);
            }
        }
        if (node_index < array_count(sema->node_decl_indices)) {
            u32 decl_index = sema->node_decl_indices[node_index];
            if (decl_index != sema_no_decl() &&
                sema->decls[decl_index].kind == SK_Constant &&
                sema->decls[decl_index].value_node_index != sema_no_decl()) {
                return ir_eval_string_constant(
                    lex,
                    ast,
                    sema,
                    ir,
                    sema->decls[decl_index].value_node_index,
                    out);
            }
        }
        return false;
    case AK_AnnotatedValue:
        return ir_eval_string_constant(lex, ast, sema, ir, node->b, out);
    default:
        return false;
    }
}

internal bool ir_eval_interpolation_part_constant(const Lexer*   lex,
                                                  const Ast*     ast,
                                                  const Sema*    sema,
                                                  Ir*            ir,
                                                  u32            node_index,
                                                  StringBuilder* sb)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_Expression:
    case AK_Statement:
    case AK_InterpPartExpr:
    case AK_AnnotatedValue:
        return ir_eval_interpolation_part_constant(
            lex,
            ast,
            sema,
            ir,
            node->kind == AK_AnnotatedValue ? node->b : node->a,
            sb);
    case AK_StringLiteral:
    case AK_StringConcat:
    case AK_InterpolatedString:
        {
            string text = {0};
            if (!ir_eval_string_constant(
                    lex, ast, sema, ir, node_index, &text)) {
                return false;
            }
            sb_append_string(sb, text);
            return true;
        }
    case AK_SymbolRef:
        if (node_index < array_count(sema->node_local_indices)) {
            u32 local_index = sema->node_local_indices[node_index];
            if (local_index != sema_no_local() &&
                sema->locals[local_index].kind == SLK_Constant &&
                sema->locals[local_index].value_node_index != sema_no_decl()) {
                return ir_eval_interpolation_part_constant(
                    lex,
                    ast,
                    sema,
                    ir,
                    sema->locals[local_index].value_node_index,
                    sb);
            }
        }
        if (node_index < array_count(sema->node_decl_indices)) {
            u32 decl_index = sema->node_decl_indices[node_index];
            if (decl_index != sema_no_decl() &&
                sema->decls[decl_index].kind == SK_Constant &&
                sema->decls[decl_index].value_node_index != sema_no_decl()) {
                return ir_eval_interpolation_part_constant(
                    lex,
                    ast,
                    sema,
                    ir,
                    sema->decls[decl_index].value_node_index,
                    sb);
            }
        }
        return false;
    case AK_FloatLiteral:
        sb_format(sb, "%g", ast_get_float(lex, node));
        return true;
    default:
        break;
    }

    u32          type_index = node_index < array_count(sema->node_type_indices)
                                  ? sema->node_type_indices[node_index]
                                  : sema_no_type();
    SemaTypeKind type_kind =
        type_index != sema_no_type() && type_index < array_count(sema->types)
            ? sema->types[type_index].kind
            : STK_Void;

    if (type_kind == STK_Bool &&
        node_index < array_count(sema->node_const_known) &&
        sema->node_const_known[node_index]) {
        sb_append_cstr(sb,
                       sema->node_const_values[node_index] != 0 ? "yes" : "no");
        return true;
    }

    if (ir_type_kind_is_integer(type_kind) &&
        node_index < array_count(sema->node_const_known) &&
        sema->node_const_known[node_index]) {
        sb_format(sb, "%lld", (long long)sema->node_const_values[node_index]);
        return true;
    }

    if (ir_type_kind_is_float(type_kind) && node->kind == AK_FloatLiteral) {
        sb_format(sb, "%g", ast_get_float(lex, node));
        return true;
    }

    return false;
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

internal u32 ir_ensure_builtin_type(Sema* sema, SemaTypeKind kind)
{
    u32 type_index = ir_builtin_type(sema, kind);
    if (type_index != sema_no_type()) {
        return type_index;
    }

    array_push(sema->types,
               ((SemaType){
                   .kind = kind,
               }));
    return (u32)array_count(sema->types) - 1;
}

internal u32 ir_add_pointer_type(Sema* sema, u32 pointee_type)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == STK_Pointer &&
            sema->types[i].first_param_type == pointee_type) {
            return i;
        }
    }

    array_push(sema->types,
               ((SemaType){
                   .kind             = STK_Pointer,
                   .first_param_type = pointee_type,
               }));
    return (u32)array_count(sema->types) - 1;
}

internal u32 ir_ensure_pointer_type(Sema* sema, Ir* ir, u32 pointee_type)
{
    u32 type_index = ir_add_pointer_type(sema, pointee_type);
    while (array_count(ir->types) < array_count(sema->types)) {
        array_push(ir->types, sema->types[array_count(ir->types)]);
    }
    return type_index;
}

internal u32 ir_add_slice_type(Sema* sema, u32 item_type)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == STK_Slice &&
            sema->types[i].first_param_type == item_type) {
            return i;
        }
    }

    array_push(sema->types,
               ((SemaType){
                   .kind             = STK_Slice,
                   .first_param_type = item_type,
               }));
    return (u32)array_count(sema->types) - 1;
}

internal u32 ir_index_result_type(const Sema* sema, u32 target_type)
{
    if (target_type == sema_no_type()) {
        return sema_no_type();
    }

    switch (sema->types[target_type].kind) {
    case STK_String:
        return ir_ensure_builtin_type((Sema*)sema, STK_U8);
    case STK_Array:
    case STK_Slice:
    case STK_DynamicArray:
        return sema->types[target_type].first_param_type;
    case STK_Pointer:
        return sema->types[target_type].first_param_type;
    default:
        return sema_no_type();
    }
}

internal u32 ir_string_append_type(const Sema* sema, u32 node_type_index)
{
    return sema_materialise_type(sema, node_type_index);
}

internal u32 ir_value_type_for_local_index(const Ast*  ast,
                                           const Sema* sema,
                                           u32         local_index);

internal u32 ir_node_type_index(const Ast*  ast,
                                const Sema* sema,
                                u32         node_index)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node_index < array_count(sema->node_type_indices) &&
        sema->node_type_indices[node_index] != sema_no_type()) {
        return ir_string_append_type(sema, sema->node_type_indices[node_index]);
    }

    if (node->kind == AK_ExprBlock || node->kind == AK_On ||
        node->kind == AK_For) {
        return ir_builtin_type(sema, STK_Void);
    }

    if (node->kind == AK_InterpPartExpr || node->kind == AK_Expression) {
        return ir_node_type_index(ast, sema, node->a);
    }

    if (node->kind == AK_Call) {
        u32 callee_type = ir_node_type_index(ast, sema, node->a);
        if (callee_type != sema_no_type() &&
            sema->types[callee_type].kind == STK_Function) {
            return sema->types[callee_type].return_type;
        }
    }

    switch (node->kind) {
    case AK_IntegerLiteral:
        return ir_builtin_type(sema, STK_UntypedInteger);
    case AK_FloatLiteral:
        return ir_builtin_type(sema, STK_UntypedFloat);
    case AK_BoolLiteral:
        return ir_builtin_type(sema, STK_Bool);
    case AK_NilLiteral:
        return ir_builtin_type(sema, STK_Nil);
    case AK_StringLiteral:
    case AK_StringConcat:
        return ir_builtin_type(sema, STK_String);
    default:
        break;
    }

    if (node->kind == AK_SymbolRef) {
        if (sema->node_local_indices[node_index] != sema_no_local()) {
            return ir_value_type_for_local_index(
                ast, sema, sema->node_local_indices[node_index]);
        }
        if (sema->node_decl_indices[node_index] != sema_no_decl()) {
            return sema->decls[sema->node_decl_indices[node_index]].type_index;
        }
    }

    return sema_no_type();
}

internal u32 ir_type_index_for_name(const Sema* sema, string name)
{
    if (string_eq(name, s("void"))) {
        return ir_builtin_type(sema, STK_Void);
    }
    if (string_eq(name, s("bool"))) {
        return ir_builtin_type(sema, STK_Bool);
    }
    if (string_eq(name, s("string"))) {
        return ir_builtin_type(sema, STK_String);
    }
    if (string_eq(name, s("i8"))) {
        return ir_builtin_type(sema, STK_I8);
    }
    if (string_eq(name, s("i16"))) {
        return ir_builtin_type(sema, STK_I16);
    }
    if (string_eq(name, s("i32"))) {
        return ir_builtin_type(sema, STK_I32);
    }
    if (string_eq(name, s("i64"))) {
        return ir_builtin_type(sema, STK_I64);
    }
    if (string_eq(name, s("u8"))) {
        return ir_builtin_type(sema, STK_U8);
    }
    if (string_eq(name, s("u16"))) {
        return ir_builtin_type(sema, STK_U16);
    }
    if (string_eq(name, s("u32"))) {
        return ir_builtin_type(sema, STK_U32);
    }
    if (string_eq(name, s("u64"))) {
        return ir_builtin_type(sema, STK_U64);
    }
    if (string_eq(name, s("f32"))) {
        return ir_builtin_type(sema, STK_F32);
    }
    if (string_eq(name, s("f64"))) {
        return ir_builtin_type(sema, STK_F64);
    }
    if (string_eq(name, s("isize"))) {
        return ir_builtin_type(sema, STK_Isize);
    }
    if (string_eq(name, s("usize"))) {
        return ir_builtin_type(sema, STK_Usize);
    }

    return sema_no_type();
}

internal u32 ir_value_type_for_local_index(const Ast*  ast,
                                           const Sema* sema,
                                           u32         local_index)
{
    if (local_index == sema_no_local()) {
        return sema_no_type();
    }
    u32 type_index = sema->locals[local_index].type_index;
    if (type_index != sema_no_type()) {
        return type_index;
    }

    const SemaLocal* local     = &sema->locals[local_index];
    u32              type_node = local->type_node_index;
    if (type_node == sema_no_type() &&
        local->decl_node_index != sema_no_decl() &&
        local->decl_node_index < array_count(ast->nodes) &&
        ast->nodes[local->decl_node_index].kind == AK_Variable) {
        u32 payload_node = ast->nodes[local->decl_node_index].b;
        if (payload_node < array_count(ast->nodes) &&
            ast->nodes[payload_node].kind == AK_AnnotatedValue) {
            type_node = ast->nodes[payload_node].a;
        }
    }

    while (type_node != sema_no_type() && type_node < array_count(ast->nodes) &&
           (ast->nodes[type_node].kind == AK_Expression ||
            ast->nodes[type_node].kind == AK_AnnotatedValue)) {
        type_node = ast->nodes[type_node].a;
    }

    if (type_node != sema_no_type() &&
        type_node < array_count(sema->node_type_indices) &&
        sema->node_type_indices[type_node] != sema_no_type()) {
        return ir_string_append_type(sema, sema->node_type_indices[type_node]);
    }
    if (type_node != sema_no_type() &&
        type_node < array_count(sema->node_decl_indices) &&
        sema->node_decl_indices[type_node] != sema_no_decl()) {
        return sema->decls[sema->node_decl_indices[type_node]].type_index;
    }
    if (local->value_node_index != sema_no_decl() &&
        local->value_node_index < array_count(sema->node_type_indices) &&
        sema->node_type_indices[local->value_node_index] != sema_no_type()) {
        return ir_string_append_type(
            sema, sema->node_type_indices[local->value_node_index]);
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

internal u32 ir_find_scope_local(const Sema* sema,
                                 u32         scope_index,
                                 u32         symbol_handle)
{
    if (scope_index == U32_MAX) {
        return sema_no_local();
    }
    const SemaScope* scope = &sema->scopes[scope_index];
    u32              end   = scope->first_local + scope->local_count;
    for (u32 i = end; i-- > scope->first_local;) {
        if (sema->locals[i].symbol_handle == symbol_handle) {
            return i;
        }
    }
    return sema_no_local();
}

internal bool ir_pattern_contains_interpolation(const Ast* ast,
                                                u32        pattern_index);

internal bool ir_instructions_include_runtime_string(const Ir* ir,
                                                     u32       first_index)
{
    for (u32 i = first_index; i < array_count(ir->instructions); ++i) {
        IrOperation op = ir->instructions[i].op;
        if (op == IR_OP_STRING_START || op == IR_OP_STRING_APPEND ||
            op == IR_OP_STRING_FINISH) {
            return true;
        }
    }
    return false;
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
    case AK_AddressOf:
    case AK_Deref:
    case AK_Field:
        return ir_node_contains_interpolation(ast, node->a);
    case AK_Cast:
        {
            const AstCastInfo* cast = &ast->casts[node->b];
            return ir_node_contains_interpolation(ast, node->a) ||
                   (cast->extra_node_index != U32_MAX &&
                    ir_node_contains_interpolation(ast,
                                                   cast->extra_node_index));
        }
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

internal IrValue ir_lower_node(const Lexer* lex,
                               const Ast*   ast,
                               const Sema*  sema,
                               u32          node_index,
                               IrLoopLabels loop,
                               Array(IrValue) node_values,
                               u64* next_value_index,
                               Ir*  ir);
internal bool    ir_try_lower_module_field(const Lexer* lex,
                                           const Ast*   ast,
                                           const Sema*  sema,
                                           u32          node_index,
                                           IrValue*     out_value);
internal IrValue ir_lower_lvalue_address(const Lexer* lex,
                                         const Ast*   ast,
                                         const Sema*  sema,
                                         u32          node_index,
                                         IrLoopLabels loop,
                                         Array(IrValue) node_values,
                                         u64* next_value_index,
                                         Ir*  ir);
internal void    ir_generate_return_statement(const Lexer*   lex,
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
                                                 u64*          next_value_index,
                                                 IrDeferStack* defers,
                                                 Ir*           ir);

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
                     ir_value_type_for_local_index(ast, sema, local_index),
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

internal bool ir_try_lower_module_field(const Lexer* lex,
                                        const Ast*   ast,
                                        const Sema*  sema,
                                        u32          node_index,
                                        IrValue*     out_value)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind != AK_Field) {
        return false;
    }

    u32 target_type = ir_node_type_index(ast, sema, node->a);
    if (target_type == sema_no_type() ||
        sema->types[target_type].kind != STK_Module) {
        return false;
    }

    u32 decl_index = sema->node_decl_indices[node_index];
    ASSERT(decl_index != sema_no_decl(), "Expected module export declaration");
    const SemaDecl* decl = &sema->decls[decl_index];
    ASSERT(decl->kind == SK_BuiltinFunction || decl->kind == SK_FfiFunction ||
               decl->kind == SK_Function,
           "Expected module export function");
    u32             runtime_symbol = decl->symbol_handle;
    const SemaDecl* runtime_decl   = decl;
    if (decl->kind == SK_Function || decl->kind == SK_BuiltinFunction) {
        const Lexer*    source_lexer = lex;
        const Ast*      source_ast   = ast;
        const Sema*     source_sema  = sema;
        const SemaDecl* source_decl  = decl;
        if (ir_imported_decl_source(sema,
                                    decl,
                                    &source_lexer,
                                    &source_ast,
                                    &source_sema,
                                    &source_decl)) {
            runtime_decl = source_decl;
            if (source_decl->kind == SK_FfiFunction) {
                const Lexer*      ffi_lexer = source_lexer;
                const Ast*        ffi_ast   = source_ast;
                const Sema*       ffi_sema  = source_sema;
                const AstFfiInfo* ffi_info  = NULL;
                if (ir_decl_ffi_info(source_lexer,
                                     source_ast,
                                     source_sema,
                                     source_decl,
                                     &ffi_lexer,
                                     &ffi_ast,
                                     &ffi_sema,
                                     &ffi_info)) {
                    runtime_symbol = sema_import_symbol_handle(
                        (Lexer*)lex,
                        ffi_lexer,
                        ffi_info->foreign_symbol_handle);
                }
                UNUSED(ffi_ast);
                UNUSED(ffi_sema);
            } else if (ir_function_symbol_conflicts(
                           sema->program,
                           decl->import_module_index,
                           source_decl->symbol_handle)) {
                runtime_symbol = ir_module_qualified_symbol(
                    (Lexer*)lex,
                    &sema->program->modules[decl->import_module_index],
                    source_decl->symbol_handle);
            } else if (source_decl->value_node_index != sema_no_decl() &&
                       source_decl->value_node_index <
                           array_count(
                               source_sema->node_lowered_symbol_handles)) {
                u32 lowered = source_sema->node_lowered_symbol_handles
                                  [source_decl->value_node_index];
                if (lowered != U32_MAX) {
                    runtime_symbol = sema_import_symbol_handle(
                        (Lexer*)lex, source_lexer, lowered);
                }
            }
        }
        UNUSED(source_ast);
    }
    if (decl->kind == SK_FfiFunction) {
        const Lexer*      ffi_lexer = lex;
        const Ast*        ffi_ast   = ast;
        const Sema*       ffi_sema  = sema;
        const AstFfiInfo* ffi_info  = NULL;
        if (ir_decl_ffi_info(lex,
                             ast,
                             sema,
                             decl,
                             &ffi_lexer,
                             &ffi_ast,
                             &ffi_sema,
                             &ffi_info)) {
            runtime_symbol = sema_import_symbol_handle(
                (Lexer*)lex, ffi_lexer, ffi_info->foreign_symbol_handle);
        }
        UNUSED(ffi_lexer);
        UNUSED(ffi_ast);
        UNUSED(ffi_sema);
    }

    *out_value = (IrValue){
        .kind          = runtime_decl->kind == SK_BuiltinFunction ||
                                 runtime_decl->kind == SK_FfiFunction
                             ? IR_VALUE_BUILTIN
                             : IR_VALUE_SYMBOL,
        .type          = decl->type_index,
        .value.integer = runtime_symbol,
    };
    return true;
}

internal bool
ir_dynarray_method_op(const Lexer* lex, u32 method_symbol, IrOperation* out_op)
{
    string method = lex_symbol(lex, method_symbol);
    if (string_eq(method, s("push"))) {
        *out_op = IR_OP_DYNARRAY_PUSH;
        return true;
    }
    if (string_eq(method, s("append"))) {
        *out_op = IR_OP_DYNARRAY_APPEND;
        return true;
    }
    if (string_eq(method, s("reserve"))) {
        *out_op = IR_OP_DYNARRAY_RESERVE;
        return true;
    }
    if (string_eq(method, s("resize"))) {
        *out_op = IR_OP_DYNARRAY_RESIZE;
        return true;
    }
    if (string_eq(method, s("resize_undefined"))) {
        *out_op = IR_OP_DYNARRAY_RESIZE_UNDEFINED;
        return true;
    }
    if (string_eq(method, s("clear"))) {
        *out_op = IR_OP_DYNARRAY_CLEAR;
        return true;
    }
    if (string_eq(method, s("free"))) {
        *out_op = IR_OP_DYNARRAY_FREE;
        return true;
    }
    if (string_eq(method, s("pop"))) {
        *out_op = IR_OP_DYNARRAY_POP;
        return true;
    }
    return false;
}

internal bool ir_try_lower_dynarray_method_call(const Lexer*   lex,
                                                const Ast*     ast,
                                                const Sema*    sema,
                                                const AstNode* call_node,
                                                IrLoopLabels   loop,
                                                Array(IrValue) node_values,
                                                u64*     next_value_index,
                                                IrValue* out_value,
                                                Ir*      ir)
{
    const AstNode* callee = &ast->nodes[call_node->a];
    if (callee->kind != AK_Field) {
        return false;
    }

    u32 receiver_type = ir_node_type_index(ast, sema, callee->a);
    if (receiver_type == sema_no_type() ||
        sema->types[receiver_type].kind != STK_DynamicArray) {
        return false;
    }

    IrOperation op = IR_OP_CALL;
    if (!ir_dynarray_method_op(lex, callee->b, &op)) {
        return false;
    }

    IrDynamicArrayTargetKind target_kind  = IR_DAT_DIRECT;
    IrValue                  target       = ir_unset_value();
    u32                      target_type  = sema_no_type();
    u32                      field_symbol = U32_MAX;
    const AstNode*           receiver     = &ast->nodes[callee->a];

    if (receiver->kind == AK_SymbolRef || receiver->kind == AK_Expression) {
        u32 receiver_node =
            receiver->kind == AK_Expression ? receiver->a : callee->a;
        target      = ir_lower_node(lex,
                                    ast,
                                    sema,
                                    receiver_node,
                                    loop,
                                    node_values,
                                    next_value_index,
                                    ir);
        target_type = ir_node_type_index(ast, sema, receiver_node);
        target_kind = IR_DAT_DIRECT;
    } else if (receiver->kind == AK_Field) {
        target       = ir_lower_node(lex,
                                     ast,
                                     sema,
                                     receiver->a,
                                     loop,
                                     node_values,
                                     next_value_index,
                                     ir);
        target_type  = ir_node_type_index(ast, sema, receiver->a);
        field_symbol = receiver->b;
        target_kind  = IR_DAT_FIELD;
    } else if (receiver->kind == AK_Deref) {
        target      = ir_lower_node(lex,
                                    ast,
                                    sema,
                                    receiver->a,
                                    loop,
                                    node_values,
                                    next_value_index,
                                    ir);
        target_type = ir_node_type_index(ast, sema, receiver->a);
        target_kind = IR_DAT_DEREF;
    } else {
        return false;
    }

    const AstCallInfo* call     = &ast->calls[call_node->b];
    IrValue            arg      = ir_unset_value();
    u32                arg_type = sema_no_type();
    if (call->arg_count > 0) {
        u32 arg_node = ast->call_args[call->first_arg];
        arg          = ir_lower_node(
            lex, ast, sema, arg_node, loop, node_values, next_value_index, ir);
        arg_type = ir_node_type_index(ast, sema, arg_node);
    }

    IrValue result      = ir_unset_value();
    u32     result_type = ir_node_type_index(ast, sema, call_node - ast->nodes);
    ir_add_dynarray_op(ir,
                       op,
                       target_kind,
                       target,
                       target_type,
                       field_symbol,
                       arg,
                       arg_type,
                       receiver_type);
    if (op == IR_OP_DYNARRAY_POP) {
        u32 op_index = (u32)array_count(ir->dynarray_ops) - 1;
        result       = (IrValue){
            .kind          = IR_VALUE_VARIABLE,
            .type          = result_type,
            .value.integer = (i64)(*next_value_index)++,
        };
        u32 instr_index                = (u32)array_count(ir->instructions) - 1;
        ir->dynarray_ops[op_index].arg = result;
        ir->dynarray_ops[op_index].arg_type  = result_type;
        ir->instructions[instr_index].lvalue = result;
        ir->instructions[instr_index].rvalue[0] =
            (IrValue){.kind = IR_VALUE_INTEGER, .value.integer = op_index};
    }
    *out_value = result;
    return true;
}

internal IrValue ir_lower_lvalue_address(const Lexer* lex,
                                         const Ast*   ast,
                                         const Sema*  sema,
                                         u32          node_index,
                                         IrLoopLabels loop,
                                         Array(IrValue) node_values,
                                         u64* next_value_index,
                                         Ir*  ir)
{
    const AstNode* node = &ast->nodes[node_index];
    if (node->kind == AK_Deref) {
        return ir_lower_node(
            lex, ast, sema, node->a, loop, node_values, next_value_index, ir);
    }

    IrValue value = {
        .kind = IR_VALUE_VARIABLE,
        .type = ir_ensure_pointer_type(
            (Sema*)sema, ir, ir_node_type_index(ast, sema, node_index)),
        .value.integer = (i64)(*next_value_index)++,
    };

    if (node->kind == AK_Field) {
        u32     base_type = ir_node_type_index(ast, sema, node->a);
        IrValue target    = ir_unset_value();
        if (base_type != sema_no_type() &&
            ir->types[base_type].kind == STK_Pointer &&
            (ir->types[ir->types[base_type].first_param_type].kind ==
                 STK_Plex ||
             ir->types[ir->types[base_type].first_param_type].kind ==
                 STK_Union)) {
            target = ir_lower_node(lex,
                                   ast,
                                   sema,
                                   node->a,
                                   loop,
                                   node_values,
                                   next_value_index,
                                   ir);
        } else {
            target = ir_lower_lvalue_address(lex,
                                             ast,
                                             sema,
                                             node->a,
                                             loop,
                                             node_values,
                                             next_value_index,
                                             ir);
        }
        if (target.type != sema_no_type() &&
            ir->types[target.type].kind == STK_Pointer) {
            u32 field_target_type = ir->types[target.type].first_param_type;
            if (field_target_type != sema_no_type() &&
                (ir->types[field_target_type].kind == STK_Plex ||
                 ir->types[field_target_type].kind == STK_Union)) {
                const SemaType* record = &ir->types[field_target_type];
                for (u32 i = 0; i < record->param_count; ++i) {
                    u32 candidate =
                        sema->type_param_symbols[record->first_param_type + i];
                    if (candidate == node->b ||
                        string_eq(lex_symbol(lex, candidate),
                                  lex_symbol(lex, node->b))) {
                        value.type = ir_ensure_pointer_type(
                            (Sema*)sema,
                            ir,
                            sema->type_param_types[record->first_param_type +
                                                   i]);
                        break;
                    }
                }
            }
        }
        ir_add_address_of_field_ptr(
            ir, value, value.type, target, target.type, node->b);
        return value;
    }

    if (node->kind == AK_Index) {
        u32 collection_type_index = ir_node_type_index(ast, sema, node->a);
        if (collection_type_index != sema_no_type() &&
            ir->types[collection_type_index].kind == STK_Pointer) {
            collection_type_index =
                ir->types[collection_type_index].first_param_type;
        }
        if (collection_type_index != sema_no_type()) {
            const SemaType* collection_type = &ir->types[collection_type_index];
            if (collection_type->kind == STK_Array ||
                collection_type->kind == STK_Slice ||
                collection_type->kind == STK_DynamicArray ||
                collection_type->kind == STK_String ||
                collection_type->kind == STK_Pointer) {
                value.type = ir_ensure_pointer_type(
                    (Sema*)sema, ir, collection_type->first_param_type);
            }
        }

        IrValue target = ir_lower_lvalue_address(
            lex, ast, sema, node->a, loop, node_values, next_value_index, ir);
        IrValue index = ir_lower_node(
            lex, ast, sema, node->b, loop, node_values, next_value_index, ir);
        ir_add_address_of_index_ptr(ir,
                                    value,
                                    value.type,
                                    target,
                                    target.type,
                                    index,
                                    ir_node_type_index(ast, sema, node->b));
        return value;
    }

    IrValue target = ir_lower_node(
        lex, ast, sema, node_index, loop, node_values, next_value_index, ir);
    if (target.type != sema_no_type() &&
        (value.type == sema_no_type() || value.type >= array_count(ir->types) ||
         ir->types[value.type].kind != STK_Pointer)) {
        value.type = ir_ensure_pointer_type((Sema*)sema, ir, target.type);
    }
    ir_add_address_of(ir, value, value.type, target);
    return value;
}

internal bool ir_try_lower_method_call(const Ast*     ast,
                                       const Sema*    sema,
                                       const AstNode* call_node,
                                       IrLoopLabels   loop,
                                       Array(IrValue) node_values,
                                       u64*         next_value_index,
                                       IrValue*     out_value,
                                       Ir*          ir,
                                       const Lexer* lex)
{
    u32 call_node_index = (u32)(call_node - ast->nodes);
    if (call_node_index >= array_count(sema->node_method_call_decl_indices) ||
        sema->node_method_call_decl_indices[call_node_index] ==
            sema_no_decl()) {
        return false;
    }

    const AstNode* callee_node = &ast->nodes[call_node->a];
    ASSERT(callee_node->kind == AK_Field, "Expected method field callee");

    u32 lowered_symbol = sema->node_lowered_symbol_handles[call_node->a];
    ASSERT(lowered_symbol != U32_MAX, "Expected lowered method symbol");
    IrValue callee = {
        .kind          = IR_VALUE_SYMBOL,
        .type          = ir_node_type_index(ast, sema, call_node->a),
        .value.integer = lowered_symbol,
    };

    const AstCallInfo* call = &ast->calls[call_node->b];
    Array(IrValue) args     = NULL;
    Array(u32) arg_types    = NULL;

    IrValue receiver        = ir_lower_node(lex,
                                            ast,
                                            sema,
                                            callee_node->a,
                                            loop,
                                            node_values,
                                            next_value_index,
                                            ir);
    u32     receiver_type   = ir_node_type_index(ast, sema, callee_node->a);
    if (sema->node_method_call_receiver_refs[call_node_index]) {
        IrValue receiver_ref = {
            .kind = IR_VALUE_VARIABLE,
            .type = ir_ensure_pointer_type((Sema*)sema, ir, receiver_type),
            .value.integer = (i64)(*next_value_index)++,
        };
        ir_add_address_of(ir, receiver_ref, receiver_ref.type, receiver);
        receiver      = receiver_ref;
        receiver_type = receiver_ref.type;
    } else if (sema->node_method_call_receiver_derefs[call_node_index]) {
        IrValue receiver_value = {
            .kind          = IR_VALUE_VARIABLE,
            .type          = ir->types[receiver_type].first_param_type,
            .value.integer = (i64)(*next_value_index)++,
        };
        IrValue index = {
            .kind          = IR_VALUE_INTEGER,
            .type          = ir_builtin_type(sema, STK_Usize),
            .value.integer = 0,
        };
        ir_add_index(ir,
                     receiver_value,
                     receiver_value.type,
                     receiver,
                     receiver_type,
                     index,
                     ir_builtin_type(sema, STK_Usize));
        receiver      = receiver_value;
        receiver_type = receiver_value.type;
    }
    array_push(args, receiver);
    array_push(arg_types, receiver_type);

    for (u32 i = 0; i < call->arg_count; ++i) {
        u32 arg_node = ast->call_args[call->first_arg + i];
        if (ast->nodes[arg_node].kind == AK_Assign) {
            arg_node = ast->nodes[arg_node].b;
        }
        u32     arg_type = ir_node_type_index(ast, sema, arg_node);
        IrValue arg      = ir_unset_value();
        if (!ir_try_lower_contextual_enum_variant(
                ast, sema, arg_node, arg_type, next_value_index, &arg, ir)) {
            arg = ir_lower_node(lex,
                                ast,
                                sema,
                                arg_node,
                                loop,
                                node_values,
                                next_value_index,
                                ir);
        }
        array_push(args, arg);
        array_push(arg_types, arg_type);
    }

    u32     result_type = ir_node_type_index(ast, sema, call_node_index);
    IrValue result      = {.kind = IR_VALUE_NONE};
    if (result_type != ir_builtin_type(sema, STK_Void)) {
        result = (IrValue){
            .kind          = IR_VALUE_VARIABLE,
            .value.integer = (i64)(*next_value_index)++,
        };
    }

    ir_add_call(ir, result, result_type, callee, callee.type, args, arg_types);
    array_free(args);
    array_free(arg_types);
    *out_value = result;
    return true;
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

    IrValue dynarray_result = ir_unset_value();
    if (ir_try_lower_dynarray_method_call(lex,
                                          ast,
                                          sema,
                                          call_node,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          &dynarray_result,
                                          ir)) {
        return dynarray_result;
    }

    IrValue method_result = ir_unset_value();
    if (ir_try_lower_method_call(ast,
                                 sema,
                                 call_node,
                                 loop,
                                 node_values,
                                 next_value_index,
                                 &method_result,
                                 ir,
                                 lex)) {
        return method_result;
    }

    IrValue callee = ir_unset_value();
    if (ir_try_lower_module_field(lex, ast, sema, call_node->a, &callee)) {
        // Module FFI functions must keep their external symbol spelling.
    } else if (ast->nodes[call_node->a].kind == AK_Field &&
               call_node->a < array_count(sema->node_lowered_symbol_handles) &&
               sema->node_lowered_symbol_handles[call_node->a] != U32_MAX) {
        IrValueKind callee_kind = IR_VALUE_SYMBOL;
        u32         decl_index  = sema->node_decl_indices[call_node->a];
        if (decl_index != sema_no_decl()) {
            const SemaDecl* runtime_decl = &sema->decls[decl_index];
            if (runtime_decl->import_module_index != sema_no_decl()) {
                const Lexer*    source_lexer = lex;
                const Ast*      source_ast   = ast;
                const Sema*     source_sema  = sema;
                const SemaDecl* source_decl  = runtime_decl;
                if (ir_imported_decl_source(sema,
                                            runtime_decl,
                                            &source_lexer,
                                            &source_ast,
                                            &source_sema,
                                            &source_decl)) {
                    runtime_decl = source_decl;
                }
                UNUSED(source_lexer);
                UNUSED(source_ast);
                UNUSED(source_sema);
            }
            if (runtime_decl->kind == SK_BuiltinFunction ||
                runtime_decl->kind == SK_FfiFunction) {
                callee_kind = IR_VALUE_BUILTIN;
            }
        }
        callee = (IrValue){
            .kind          = callee_kind,
            .type          = ir_node_type_index(ast, sema, call_node->a),
            .value.integer = sema->node_lowered_symbol_handles[call_node->a],
        };
    } else {
        callee = ir_lower_node(lex,
                               ast,
                               sema,
                               call_node->a,
                               loop,
                               node_values,
                               next_value_index,
                               ir);
    }
    const AstCallInfo* call                 = &ast->calls[call_node->b];
    Array(IrValue) args                     = NULL;
    Array(u32) arg_types                    = NULL;

    u32                   known_fn_node     = U32_MAX;
    const AstFnSignature* known_signature   = NULL;
    u32                   param_scope_index = sema_no_decl();
    if (ir_known_call_fn_node(ast, sema, call_node->a, &known_fn_node)) {
        const AstNode* fn_start = &ast->nodes[ast->nodes[known_fn_node].a];
        known_signature         = &ast->fn_signatures[fn_start->a];
        param_scope_index       = sema->node_scope_indices[known_fn_node];
    }

    for (u32 i = 0; i < call->arg_count; ++i) {
        u32 arg_node = ast->call_args[call->first_arg + i];
        if (ast->nodes[arg_node].kind == AK_Assign) {
            arg_node = ast->nodes[arg_node].b;
        }
        u32     arg_type = ir_node_type_index(ast, sema, arg_node);
        IrValue arg      = ir_unset_value();
        if (!ir_try_lower_contextual_enum_variant(
                ast, sema, arg_node, arg_type, next_value_index, &arg, ir)) {
            arg = ir_lower_node(lex,
                                ast,
                                sema,
                                arg_node,
                                loop,
                                node_values,
                                next_value_index,
                                ir);
        }
        array_push(args, arg);
        array_push(arg_types, arg_type);
    }

    u32 substitution_count = (u32)array_count(g_ir_local_substitutions);
    if (known_signature != NULL &&
        call->arg_count <
            ir_signature_required_param_count(ast, known_signature)) {
        known_signature = NULL;
    }
    if (known_signature != NULL &&
        call->arg_count < known_signature->param_count) {
        ASSERT(param_scope_index != sema_no_decl(),
               "Expected known function parameter scope");
        const SemaScope* param_scope = &sema->scopes[param_scope_index];
        for (u32 i = 0; i < call->arg_count; ++i) {
            array_push(g_ir_local_substitutions,
                       (IrLocalSubstitution){
                           .local_index = param_scope->first_local + i,
                           .value       = args[i],
                       });
        }
        for (u32 i = call->arg_count; i < known_signature->param_count; ++i) {
            const AstParam* param =
                &ast->params[known_signature->first_param + i];
            ASSERT(param->default_node_index != U32_MAX,
                   "Expected omitted parameter default");
            IrValue default_value = ir_lower_node(lex,
                                                  ast,
                                                  sema,
                                                  param->default_node_index,
                                                  loop,
                                                  node_values,
                                                  next_value_index,
                                                  ir);
            u32     default_type =
                ir_node_type_index(ast, sema, param->default_node_index);
            array_push(args, default_value);
            array_push(arg_types, default_type);
            array_push(g_ir_local_substitutions,
                       (IrLocalSubstitution){
                           .local_index = param_scope->first_local + i,
                           .value       = default_value,
                       });
        }
    }
    while (array_count(g_ir_local_substitutions) > substitution_count) {
        (void)array_pop(g_ir_local_substitutions);
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
    if (decl->bind_node_index == sema_no_decl()) {
        return false;
    }

    if (decl->kind == SK_Variable) {
        return true;
    }

    if (decl->value_node_index == sema_no_decl()) {
        return false;
    }

    if (decl->kind != SK_Constant) {
        return false;
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

    case AK_NilLiteral:
        {
            IrValue value = {
                .kind          = IR_VALUE_INTEGER,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = 0,
            };
            node_values[node_index] = value;
            return value;
        }

    case AK_StringLiteral:
        {
            IrValue value = {
                .kind = IR_VALUE_STRING,
                .type = ir_node_type_index(ast, sema, node_index),
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
            string  literal = {0};
            IrValue value   = ir_unset_value();
            if (ir_eval_string_constant(
                    lex, ast, sema, ir, node_index, &literal)) {
                value = (IrValue){
                    .kind          = IR_VALUE_STRING,
                    .type          = ir_node_type_index(ast, sema, node_index),
                    .value.integer = ir_add_string_literal(ir, literal),
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
            if (sema->node_lowered_symbol_handles[node_index] != U32_MAX &&
                node_type != sema_no_type() &&
                sema->types[node_type].kind == STK_Function) {
                value = (IrValue){
                    .kind = IR_VALUE_SYMBOL,
                    .type = node_type,
                    .value.integer =
                        sema->node_lowered_symbol_handles[node_index],
                };
                node_values[node_index] = value;
                return value;
            }
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                u32 local_index = sema->node_local_indices[node_index];
                if (ir_find_local_substitution(local_index, &value)) {
                    node_values[node_index] = value;
                    return value;
                }
                const SemaLocal* local = &sema->locals[local_index];
                if (local->kind == SLK_Constant &&
                    local->value_node_index != sema_no_decl()) {
                    value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          local->value_node_index,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
                } else if (local->kind == SLK_Constant) {
                    value = (IrValue){
                        .kind          = IR_VALUE_SYMBOL,
                        .type          = local->type_index,
                        .value.integer = local->lowered_symbol_handle,
                    };
                } else if (local->kind == SLK_Binder &&
                           local->decl_node_index != sema_no_decl() &&
                           (ast->nodes[local->decl_node_index].kind ==
                                AK_DestructureBind ||
                            ast->nodes[local->decl_node_index].kind ==
                                AK_DestructureVariable)) {
                    value = (IrValue){
                        .kind = IR_VALUE_LOCAL,
                        .type = ir_value_type_for_local_index(
                            ast, sema, local_index),
                        .value.integer = local->lowered_symbol_handle,
                    };
                } else if (local->kind == SLK_Binder &&
                           local->value_node_index == sema_no_decl()) {
                    value = (IrValue){
                        .kind = IR_VALUE_LOCAL,
                        .type = ir_value_type_for_local_index(
                            ast, sema, local_index),
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
                        .type = ir_value_type_for_local_index(
                            ast, sema, local_index),
                        .value.integer = local->lowered_symbol_handle,
                    };
                }
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                ASSERT(decl_index != U32_MAX,
                       "Expected resolved symbol reference");
                const SemaDecl* decl           = &sema->decls[decl_index];
                u32             runtime_symbol = decl->symbol_handle;
                if (g_ir_current_global_init_decl != U32_MAX &&
                    ir_decl_is_pointer_alias_to_decl(
                        ast, sema, decl_index, g_ir_current_global_init_decl)) {
                    value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          decl->value_node_index,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
                    node_values[node_index] = value;
                    return value;
                }
                if (decl->kind == SK_Constant &&
                    decl->import_module_index != sema_no_decl()) {
                    const Lexer*    source_lexer = lex;
                    const Ast*      source_ast   = ast;
                    const Sema*     source_sema  = sema;
                    const SemaDecl* source_decl  = NULL;
                    if (ir_imported_decl_source(sema,
                                                decl,
                                                &source_lexer,
                                                &source_ast,
                                                &source_sema,
                                                &source_decl) &&
                        source_decl->value_node_index != sema_no_decl()) {
                        const AstNode* source_value =
                            &source_ast->nodes[source_decl->value_node_index];
                        if (source_value->kind == AK_Expression) {
                            source_value = &source_ast->nodes[source_value->a];
                        }
                        if (source_value->kind == AK_IntegerLiteral) {
                            value = (IrValue){
                                .kind          = IR_VALUE_INTEGER,
                                .type          = decl->type_index,
                                .value.integer = (i64)ast_get_integer(
                                    source_lexer, source_value),
                            };
                            node_values[node_index] = value;
                            return value;
                        }
                    }
                    UNUSED(source_sema);
                }
                if (decl->kind == SK_Function ||
                    decl->kind == SK_BuiltinFunction) {
                    const Lexer*    source_lexer = lex;
                    const Sema*     source_sema  = sema;
                    const SemaDecl* source_decl  = decl;
                    if (decl->import_module_index != sema_no_decl()) {
                        const Ast* source_ast = ast;
                        ir_imported_decl_source(sema,
                                                decl,
                                                &source_lexer,
                                                &source_ast,
                                                &source_sema,
                                                &source_decl);
                        UNUSED(source_ast);
                    }
                    if (source_decl->value_node_index != sema_no_decl() &&
                        source_decl->value_node_index <
                            array_count(
                                source_sema->node_lowered_symbol_handles)) {
                        u32 lowered = source_sema->node_lowered_symbol_handles
                                          [source_decl->value_node_index];
                        if (lowered != U32_MAX) {
                            runtime_symbol = sema_import_symbol_handle(
                                (Lexer*)lex, source_lexer, lowered);
                        }
                    }
                }
                if (decl->kind == SK_FfiFunction) {
                    const Lexer*      ffi_lexer = lex;
                    const Ast*        ffi_ast   = ast;
                    const Sema*       ffi_sema  = sema;
                    const AstFfiInfo* ffi_info  = NULL;
                    if (ir_decl_ffi_info(lex,
                                         ast,
                                         sema,
                                         decl,
                                         &ffi_lexer,
                                         &ffi_ast,
                                         &ffi_sema,
                                         &ffi_info)) {
                        runtime_symbol = sema_import_symbol_handle(
                            (Lexer*)lex,
                            ffi_lexer,
                            ffi_info->foreign_symbol_handle);
                    }
                    UNUSED(ffi_lexer);
                    UNUSED(ffi_ast);
                    UNUSED(ffi_sema);
                }

                value = (IrValue){
                    .kind          = decl->kind == SK_BuiltinFunction ||
                                             decl->kind == SK_FfiFunction
                                         ? IR_VALUE_BUILTIN
                                         : IR_VALUE_SYMBOL,
                    .type          = decl->type_index,
                    .value.integer = runtime_symbol,
                };
            }
            node_values[node_index] = value;
            return value;
        }

    case AK_AddressOf:
        {
            IrValue value           = ir_lower_lvalue_address(lex,
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

    case AK_Deref:
        {
            IrValue pointer = ir_lower_node(lex,
                                            ast,
                                            sema,
                                            node->a,
                                            loop,
                                            node_values,
                                            next_value_index,
                                            ir);
            IrValue index   = {
                .kind          = IR_VALUE_INTEGER,
                .type          = ir_builtin_type(sema, STK_Usize),
                .value.integer = 0,
            };
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_index(ir,
                         value,
                         ir_node_type_index(ast, sema, node_index),
                         pointer,
                         ir_node_type_index(ast, sema, node->a),
                         index,
                         ir_builtin_type(sema, STK_Usize));
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
            const AstCastInfo* cast = &ast->casts[node->b];
            u32     source_type     = ir_node_type_index(ast, sema, node->a);
            u32     target_type     = sema->node_type_indices[node_index];
            IrValue source          = ir_lower_node(lex,
                                                    ast,
                                                    sema,
                                                    node->a,
                                                    loop,
                                                    node_values,
                                                    next_value_index,
                                                    ir);
            IrValue value           = {
                .kind          = IR_VALUE_VARIABLE,
                .value.integer = (i64)(*next_value_index)++,
            };

            if (cast->extra_node_index != U32_MAX &&
                sema->types[source_type].kind == STK_Pointer &&
                sema->types[target_type].kind == STK_Slice) {
                IrValue size = ir_lower_node(lex,
                                             ast,
                                             sema,
                                             cast->extra_node_index,
                                             loop,
                                             node_values,
                                             next_value_index,
                                             ir);
                ir_add_slice(
                    ir,
                    value,
                    target_type,
                    source,
                    source_type,
                    (IrValue){.kind = IR_VALUE_NONE},
                    sema_no_type(),
                    size,
                    ir_node_type_index(ast, sema, cast->extra_node_index));
            } else {
                ir_add_cast(ir, value, target_type, source, source_type);
            }
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
            IrValue value                     = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = ir_node_type_index(ast, sema, node_index),
                .value.integer = (i64)(*next_value_index)++,
            };
            u32 type_index = ir_node_type_index(ast, sema, node_index);
            if (literal->flags & APLF_ZeroMissing) {
                Array(u32) symbols   = NULL;
                const SemaType* plex = &ir->types[type_index];
                for (u32 field_index = 0; field_index < plex->param_count;
                     ++field_index) {
                    u32 field_symbol =
                        ir->type_param_symbols[plex->first_param_type +
                                               field_index];
                    u32 field_type =
                        ir->type_param_types[plex->first_param_type +
                                             field_index];
                    u32 value_node = U32_MAX;
                    for (u32 field = 0; field < literal->field_count; ++field) {
                        const AstPlexLiteralField* literal_field =
                            &ast->plex_literal_fields[literal->first_field +
                                                      field];
                        if (literal_field->symbol_handle == field_symbol) {
                            value_node = literal_field->value_node_index;
                            break;
                        }
                    }
                    IrValue field_value = value_node != U32_MAX
                                              ? ir_lower_node(lex,
                                                              ast,
                                                              sema,
                                                              value_node,
                                                              loop,
                                                              node_values,
                                                              next_value_index,
                                                              ir)
                                              : (IrValue){
                                                    .kind = IR_VALUE_INTEGER,
                                                    .type = field_type,
                                                    .value.integer = 0,
                                                };
                    array_push(values, field_value);
                    array_push(symbols, field_symbol);
                }
                ir_add_plex_items(ir, value, type_index, symbols, values);
                array_free(symbols);
            } else {
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
                ir_add_plex(ir, value, type_index, ast, literal, values);
            }
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

    case AK_Assign:
        {
            IrValue        value       = ir_lower_node(lex,
                                                       ast,
                                                       sema,
                                                       node->b,
                                                       loop,
                                                       node_values,
                                                       next_value_index,
                                                       ir);
            const AstNode* target_node = &ast->nodes[node->a];
            if (target_node->kind == AK_SymbolRef) {
                u32 local_index = sema->node_local_indices[node_index];
                IrValue target =
                    local_index != sema_no_local()
                        ? (IrValue){
                              .kind          = IR_VALUE_LOCAL,
                              .type          = ir_value_type_for_local_index(
                                  ast, sema, local_index),
                              .value.integer =
                                  sema->locals[local_index].lowered_symbol_handle,
                          }
                        : (IrValue){.kind = IR_VALUE_SYMBOL,
                                    .type = ir_value_type_for_decl(
                                        sema, target_node->a),
                                    .value.integer = target_node->a};
                u32 target_type =
                    local_index != sema_no_local()
                        ? ir_value_type_for_local_index(ast, sema, local_index)
                        : ir_value_type_for_decl(sema, target_node->a);
                ir_add_assign(ir, target, target_type, value, target_type);
            } else if (target_node->kind == AK_Field &&
                       ast->nodes[target_node->a].kind == AK_SymbolRef) {
                IrValue target  = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                target_node->a,
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
                u32 target_type = ir_node_type_index(ast, sema, target_node->a);
                if ((target_type == sema_no_type() ||
                     target_type >= array_count(ir->types) ||
                     ir->types[target_type].kind == STK_Void) &&
                    target.type != sema_no_type()) {
                    target_type = target.type;
                }
                ir_add_store_field(ir,
                                   target,
                                   target_type,
                                   target_node->b,
                                   value,
                                   ir_node_type_index(ast, sema, node->a));
            } else if (target_node->kind == AK_Deref ||
                       target_node->kind == AK_Field ||
                       target_node->kind == AK_Index) {
                IrValue pointer = ir_lower_lvalue_address(lex,
                                                          ast,
                                                          sema,
                                                          node->a,
                                                          loop,
                                                          node_values,
                                                          next_value_index,
                                                          ir);
                ir_add_store(ir,
                             pointer,
                             pointer.type,
                             value,
                             ir_node_type_index(ast, sema, node->a));
            } else {
                error_ice("Unsupported assignment target node kind: %d",
                          target_node->kind);
            }
            node_values[node_index] = value;
            return value;
        }

    case AK_PlexUpdate:
        {
            const AstPlexLiteralInfo* literal = &ast->plex_literals[node->a];
            IrValue base          = ir_lower_node(lex,
                                                  ast,
                                                  sema,
                                                  literal->target_node_index,
                                                  loop,
                                                  node_values,
                                                  next_value_index,
                                                  ir);
            u32     type_index    = ir_node_type_index(ast, sema, node_index);
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
                } else if (literal->flags & APLF_ZeroMissing) {
                    field_value = (IrValue){
                        .kind          = IR_VALUE_INTEGER,
                        .type          = field_type,
                        .value.integer = 0,
                    };
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
            u32    type_index = ir_node_type_index(ast, sema, node_index);
            string field_name = lex_symbol(lex, node->b);
            if (string_eq(field_name, s("size"))) {
                u32 source_type = ir_node_type_index(ast, sema, node->a);
                if (source_type == sema_no_type() &&
                    ast->nodes[node->a].kind == AK_SymbolRef) {
                    source_type = ir_type_index_for_name(
                        sema, lex_symbol(lex, ast->nodes[node->a].a));
                }

                u32  field_target_type = source_type;
                bool record_size_field = false;
                if (field_target_type != sema_no_type() &&
                    sema->types[field_target_type].kind == STK_Pointer) {
                    u32 pointee_type =
                        sema->types[field_target_type].first_param_type;
                    if (sema->types[pointee_type].kind == STK_Plex ||
                        sema->types[pointee_type].kind == STK_Union) {
                        field_target_type = pointee_type;
                    }
                }
                if (field_target_type != sema_no_type() &&
                    (sema->types[field_target_type].kind == STK_Plex ||
                     sema->types[field_target_type].kind == STK_Union)) {
                    const SemaType* record = &sema->types[field_target_type];
                    for (u32 i = 0; i < record->param_count; ++i) {
                        if (sema->type_param_symbols[record->first_param_type +
                                                     i] == node->b) {
                            record_size_field = true;
                            break;
                        }
                    }
                }

                if (!record_size_field) {
                    if (source_type == sema_no_type()) {
                        IrValue source = ir_lower_node(lex,
                                                       ast,
                                                       sema,
                                                       node->a,
                                                       loop,
                                                       node_values,
                                                       next_value_index,
                                                       ir);
                        source_type    = source.type;
                    }
                    source_type   = sema_materialise_type(sema, source_type);
                    IrValue value = {
                        .kind          = IR_VALUE_VARIABLE,
                        .type          = type_index,
                        .value.integer = (i64)(*next_value_index)++,
                    };
                    ir_add_size(ir, value, type_index, source_type);
                    node_values[node_index] = value;
                    return value;
                }
            }
            u32 variant = ir_enum_variant_index(sema, type_index, node->b);
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
            IrValue module_field = ir_unset_value();
            if (ir_try_lower_module_field(
                    lex, ast, sema, node_index, &module_field)) {
                node_values[node_index] = module_field;
                return module_field;
            }
            IrValue target      = ir_lower_node(lex,
                                                ast,
                                                sema,
                                                node->a,
                                                loop,
                                                node_values,
                                                next_value_index,
                                                ir);
            u32     target_type = ir_node_type_index(ast, sema, node->a);
            if (target_type == sema_no_type() &&
                target.type != sema_no_type()) {
                target_type = target.type;
            }
            if (target_type != sema_no_type()) {
                if (string_eq(field_name, s("data")) &&
                    (sema->types[target_type].kind == STK_String ||
                     sema->types[target_type].kind == STK_Slice ||
                     sema->types[target_type].kind == STK_DynamicArray)) {
                    if (sema->types[target_type].kind == STK_String) {
                        type_index = ir_ensure_pointer_type(
                            (Sema*)sema,
                            ir,
                            ir_ensure_builtin_type((Sema*)sema, STK_U8));
                    } else {
                        type_index = ir_ensure_pointer_type(
                            (Sema*)sema,
                            ir,
                            sema->types[target_type].first_param_type);
                    }
                } else if (string_eq(field_name, s("count")) ||
                           string_eq(field_name, s("capacity"))) {
                    type_index = ir_builtin_type(sema, STK_Usize);
                } else {
                    u32 field_target_type = target_type;
                    if (sema->types[target_type].kind == STK_Pointer) {
                        u32 pointee_type =
                            sema->types[target_type].first_param_type;
                        if (sema->types[pointee_type].kind == STK_Plex ||
                            sema->types[pointee_type].kind == STK_Union) {
                            field_target_type = pointee_type;
                        }
                    }
                    if (sema->types[field_target_type].kind == STK_Plex ||
                        sema->types[field_target_type].kind == STK_Union) {
                        const SemaType* record =
                            &sema->types[field_target_type];
                        for (u32 i = 0; i < record->param_count; ++i) {
                            u32 candidate = sema->type_param_symbols
                                                [record->first_param_type + i];
                            if (candidate == node->b ||
                                string_eq(lex_symbol(lex, candidate),
                                          field_name)) {
                                type_index = sema->type_param_types
                                                 [record->first_param_type + i];
                                break;
                            }
                        }
                    }
                }
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = type_index,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_field(ir, value, type_index, target, target_type, node->b);
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
            u32 implicit_array_type =
                node_index < array_count(sema->node_implicit_array_type_indices)
                    ? sema->node_implicit_array_type_indices[node_index]
                    : sema_no_type();
            if (implicit_array_type != sema_no_type()) {
                IrValue array_value = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = implicit_array_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_array(ir, array_value, implicit_array_type, items);
                IrValue slice_value = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = ir_node_type_index(ast, sema, node_index),
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_slice(ir,
                             slice_value,
                             ir_node_type_index(ast, sema, node_index),
                             array_value,
                             implicit_array_type,
                             ir_unset_value(),
                             sema_no_type(),
                             ir_unset_value(),
                             sema_no_type());
                array_free(items);
                node_values[node_index] = slice_value;
                return slice_value;
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
            const AstSliceInfo* slice  = &ast->slices[node->a];
            IrValue             target = ir_lower_node(lex,
                                                       ast,
                                                       sema,
                                                       slice->target_node_index,
                                                       loop,
                                                       node_values,
                                                       next_value_index,
                                                       ir);
            IrValue             start  = {0};
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
                if (start_type == sema_no_type() &&
                    start.type != sema_no_type()) {
                    start_type = start.type;
                }
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
                if (end_type == sema_no_type() && end.type != sema_no_type()) {
                    end_type = end.type;
                }
            }
            u32 target_type =
                ir_node_type_index(ast, sema, slice->target_node_index);
            if (target_type == sema_no_type() &&
                target.type != sema_no_type()) {
                target_type = target.type;
            }
            u32 value_type = ir_node_type_index(ast, sema, node_index);
            if (value_type == sema_no_type() && target_type != sema_no_type()) {
                if (sema->types[target_type].kind == STK_String) {
                    value_type = ir_builtin_type(sema, STK_String);
                } else if (sema->types[target_type].kind == STK_Slice ||
                           sema->types[target_type].kind == STK_Array) {
                    value_type = ir_add_slice_type(
                        (Sema*)sema, sema->types[target_type].first_param_type);
                } else if (sema->types[target_type].kind == STK_DynamicArray) {
                    value_type = ir_add_slice_type(
                        (Sema*)sema, sema->types[target_type].first_param_type);
                }
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = value_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_slice(ir,
                         value,
                         value_type,
                         target,
                         target_type,
                         start,
                         start_type,
                         end,
                         end_type);
            node_values[node_index] = value;
            return value;
        }

    case AK_Index:
        {
            if (sema->node_lowered_symbol_handles[node_index] != U32_MAX) {
                IrValue value = {
                    .kind = IR_VALUE_SYMBOL,
                    .type = ir_node_type_index(ast, sema, node_index),
                    .value.integer =
                        sema->node_lowered_symbol_handles[node_index],
                };
                node_values[node_index] = value;
                return value;
            }

            IrValue array      = ir_lower_node(lex,
                                               ast,
                                               sema,
                                               node->a,
                                               loop,
                                               node_values,
                                               next_value_index,
                                               ir);
            IrValue index      = ir_lower_node(lex,
                                               ast,
                                               sema,
                                               node->b,
                                               loop,
                                               node_values,
                                               next_value_index,
                                               ir);
            u32     array_type = ir_node_type_index(ast, sema, node->a);
            if (array_type == sema_no_type() && array.type != sema_no_type()) {
                array_type = array.type;
            }
            u32 index_type = ir_node_type_index(ast, sema, node->b);
            if (index_type == sema_no_type() && index.type != sema_no_type()) {
                index_type = index.type;
            }
            u32 value_type = ir_node_type_index(ast, sema, node_index);
            if (value_type == sema_no_type()) {
                value_type = ir_index_result_type(sema, array_type);
            }
            IrValue value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = value_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_index(
                ir, value, value_type, array, array_type, index, index_type);
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
                        payload = ir_lower_node(lex,
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
                    if (loop.defers != NULL) {
                        ir_emit_defers_no_pop(lex,
                                              ast,
                                              sema,
                                              ir_current_function_index(ir),
                                              loop,
                                              node_values,
                                              next_value_index,
                                              0,
                                              loop.defers,
                                              ir);
                    }
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
                        if (loop.defers != NULL) {
                            ir_emit_defers_no_pop(lex,
                                                  ast,
                                                  sema,
                                                  ir_current_function_index(ir),
                                                  loop,
                                                  node_values,
                                                  next_value_index,
                                                  loop_target->defer_count,
                                                  loop.defers,
                                                  ir);
                        }
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
                        if (loop.defers != NULL) {
                            ir_emit_defers_no_pop(lex,
                                                  ast,
                                                  sema,
                                                  ir_current_function_index(ir),
                                                  loop,
                                                  node_values,
                                                  next_value_index,
                                                  loop_target->defer_count,
                                                  loop.defers,
                                                  ir);
                        }
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
                    .defer_count  = 0,
                };

            IrDeferStack expr_defers = {0};
            (void)ir_generate_statement(lex,
                                        ast,
                                        sema,
                                        U32_MAX,
                                        node->a,
                                        block_control,
                                        node_values,
                                        next_value_index,
                                        &expr_defers,
                                        ir);
            array_free(expr_defers.statements);
            ir_add_label(ir, end_label);
            node_values[node_index] = result;
            return result;
        }

    case AK_For:
        {
            IrDeferStack expr_defers = {0};
            (void)ir_generate_statement(lex,
                                        ast,
                                        sema,
                                        U32_MAX,
                                        node_index,
                                        loop,
                                        node_values,
                                        next_value_index,
                                        &expr_defers,
                                        ir);
            array_free(expr_defers.statements);
        }
        return node_values[node_index];

    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_BitwiseAnd:
    case AK_BitwiseXor:
    case AK_BitwiseOr:
    case AK_ShiftLeft:
    case AK_ShiftRight:
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
            case AK_ShiftLeft:
                op = IR_OP_SHIFT_LEFT;
                break;
            case AK_ShiftRight:
                op = IR_OP_SHIFT_RIGHT;
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
            if (lhs_type == sema_no_type() && op_lhs.type != sema_no_type()) {
                lhs_type = op_lhs.type;
            }
            if (rhs_type == sema_no_type() && op_rhs.type != sema_no_type()) {
                rhs_type = op_rhs.type;
            }
            if (lhs_type == sema_no_type() && rhs_type != sema_no_type()) {
                lhs_type = rhs_type;
            }
            if (rhs_type == sema_no_type() && lhs_type != sema_no_type()) {
                rhs_type = lhs_type;
            }
            u32 result_type = ir_node_type_index(ast, sema, node_index);
            if (result_type == sema_no_type()) {
                switch (node->kind) {
                case AK_Equal:
                case AK_NotEqual:
                case AK_Less:
                case AK_LessEqual:
                case AK_Greater:
                case AK_GreaterEqual:
                case AK_LogicalAnd:
                case AK_LogicalOr:
                    result_type = ir_builtin_type(sema, STK_Bool);
                    break;
                default:
                    result_type =
                        lhs_type != sema_no_type() ? lhs_type : rhs_type;
                    break;
                }
            }
            if (lhs_type == sema_no_type() &&
                (ast->nodes[node->a].kind == AK_IntegerLiteral ||
                 ast->nodes[node->a].kind == AK_FloatLiteral)) {
                lhs_type = rhs_type;
            }
            if (rhs_type == sema_no_type() &&
                (ast->nodes[node->b].kind == AK_IntegerLiteral ||
                 ast->nodes[node->b].kind == AK_FloatLiteral)) {
                rhs_type = lhs_type;
            }
            ir_add_binary(
                ir, op, value, result_type, op_lhs, lhs_type, op_rhs, rhs_type);
            value.type              = result_type;
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
        ASSERT(array_count(ir->functions) > 0,
               "Expected active function when lowering return");
        u32 current_function_index = (u32)array_count(ir->functions) - 1;
        u32 function_type          = ir->functions[current_function_index].type;
        const SemaType* fn_type    = &ir->types[function_type];
        u32             return_type = fn_type->return_type;
        bool            is_main     = string_eq(
            lex_symbol(lex, ir->functions[current_function_index].symbol),
            s("main"));
        if (return_type == ir_builtin_type(sema, STK_Void) && !is_main) {
            ir_add_return(ir,
                          (IrValue){.kind = IR_VALUE_NONE},
                          ir_builtin_type(sema, STK_Void));
        } else {
            u32 zero_type = ir_builtin_type(sema, STK_I32);
            ir_add_return(ir,
                          (IrValue){.kind          = IR_VALUE_INTEGER,
                                    .type          = zero_type,
                                    .value.integer = 0},
                          zero_type);
        }
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
        u32 local_type = ir_value_type_for_local_index(ast, sema, local_index);
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
        u32 target_type = ir_value_type_for_local_index(ast, sema, local_index);
        IrValue target  = {
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

internal u32 ir_dynarray_min_capacity(const Ast*  ast,
                                      const Sema* sema,
                                      u32         type_node_index)
{
    if (type_node_index == sema_no_type()) {
        return 0;
    }
    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind != AK_TypeDynamicArray || type_node->a == U32_MAX) {
        return 0;
    }
    if (type_node->a < array_count(sema->node_const_known) &&
        sema->node_const_known[type_node->a]) {
        i64 value = sema->node_const_values[type_node->a];
        return value > 0 ? (u32)value : 0;
    }
    return 0;
}

internal u32 ir_dynarray_min_capacity_node(const Ast* ast, u32 type_node_index)
{
    if (type_node_index == sema_no_type()) {
        return sema_no_decl();
    }
    const AstNode* type_node = &ast->nodes[type_node_index];
    if (type_node->kind != AK_TypeDynamicArray || type_node->a == U32_MAX) {
        return sema_no_decl();
    }
    return type_node->a;
}

internal IrStatementResult ir_generate_statement(const Lexer* lex,
                                                 const Ast*   ast,
                                                 const Sema*  sema,
                                                 u32          function_index,
                                                 u32          node_index,
                                                 IrLoopLabels loop,
                                                 Array(IrValue) node_values,
                                                 u64*          next_value_index,
                                                 IrDeferStack* defers,
                                                 Ir*           ir)
{
    const AstNode* node = &ast->nodes[node_index];
    loop.defers         = defers;

    if (node->kind == AK_Block) {
        u32 base_defer_count = (u32)array_count(defers->statements);
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
                                                             defers,
                                                             ir);
            if (result != IR_STMT_FALLTHROUGH) {
                ir_add_block_end(ir);
                return result;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }
        ir_emit_defers(lex,
                       ast,
                       sema,
                       function_index,
                       loop,
                       node_values,
                       next_value_index,
                       base_defer_count,
                       defers,
                       ir);
        ir_add_block_end(ir);
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind == AK_Defer) {
        array_push(defers->statements, node->a);
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind == AK_Assert) {
        IrValue condition = ir_lower_node(
            lex, ast, sema, node->a, loop, node_values, next_value_index, ir);
        u32     string_type = ir_builtin_type(sema, STK_String);
        IrValue message =
            node->b == U32_MAX
                ? (IrValue){.kind          = IR_VALUE_STRING,
                            .type          = string_type,
                            .value.integer = ir_add_string_literal(
                                ir, s("assertion failed"))}
                : ir_lower_node(lex,
                                ast,
                                sema,
                                node->b,
                                loop,
                                node_values,
                                next_value_index,
                                ir);
        string source_path = lex->source.source_path;
        u32    line        = 0;
        ir_source_location_for_offset(lex->source,
                                      lex->tokens[node->token_index].offset,
                                      &source_path,
                                      &line);
        ir_add_assert(ir,
                      condition,
                      ir_builtin_type(sema, STK_Bool),
                      message,
                      string_type,
                      source_path,
                      line);
        return IR_STMT_FALLTHROUGH;
    }

    if (node->kind == AK_Return) {
        if (node->a == U32_MAX) {
            ir_emit_defers_no_pop(lex,
                                  ast,
                                  sema,
                                  function_index,
                                  loop,
                                  node_values,
                                  next_value_index,
                                  0,
                                  defers,
                                  ir);
            ir_generate_return_statement(
                lex, ast, sema, node, loop, node_values, next_value_index, ir);
            return IR_STMT_RETURN;
        }
        IrValue return_value = ir_unset_value();
        u32     return_type  = ir_builtin_type(sema, STK_Void);
        return_value         = ir_lower_node(
            lex, ast, sema, node->a, loop, node_values, next_value_index, ir);
        return_type = ir_node_type_index(ast, sema, node->a);
        if (array_count(defers->statements) > 0) {
            IrValue saved_return = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = return_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_temp_local(ir, saved_return, return_type);
            ir_add_assign(
                ir, saved_return, return_type, return_value, return_type);
            return_value = saved_return;
        }
        ir_emit_defers_no_pop(lex,
                              ast,
                              sema,
                              function_index,
                              loop,
                              node_values,
                              next_value_index,
                              0,
                              defers,
                              ir);
        ir_add_return(ir, return_value, return_type);
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
            ir_emit_defers_no_pop(lex,
                                  ast,
                                  sema,
                                  function_index,
                                  loop,
                                  node_values,
                                  next_value_index,
                                  expr_target->defer_count,
                                  defers,
                                  ir);
            ir_add_jump(ir, expr_target->break_label);
        } else if (loop_target != NULL) {
            ir_emit_defers_no_pop(lex,
                                  ast,
                                  sema,
                                  function_index,
                                  loop,
                                  node_values,
                                  next_value_index,
                                  loop_target->defer_count,
                                  defers,
                                  ir);
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
            ir_emit_defers_no_pop(lex,
                                  ast,
                                  sema,
                                  function_index,
                                  loop,
                                  node_values,
                                  next_value_index,
                                  loop_target->defer_count,
                                  defers,
                                  ir);
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
        if (for_info->iterable_node_index != U32_MAX) {
            u32 iterable_type =
                ir_node_type_index(ast, sema, for_info->iterable_node_index);
            u32 usize_type        = ir_builtin_type(sema, STK_Usize);
            u32 bool_type         = ir_builtin_type(sema, STK_Bool);
            u32 for_scope         = sema->node_scope_indices[node_index];
            u32 index_local_index = sema_no_local();
            if (for_info->index_symbol != U32_MAX) {
                index_local_index = ir_find_scope_local(
                    sema, for_scope, for_info->index_symbol);
                ASSERT(index_local_index != sema_no_local(),
                       "Expected resolved for-in index local");
            }
            u32 item_local_index =
                ir_find_scope_local(sema, for_scope, for_info->item_symbol);
            ASSERT(item_local_index != sema_no_local(),
                   "Expected resolved for-in item local");
            u32 lowered_item_type = sema->locals[item_local_index].type_index;

            IrValue iterable_value =
                ir_lower_node(lex,
                              ast,
                              sema,
                              for_info->iterable_node_index,
                              loop,
                              node_values,
                              next_value_index,
                              ir);
            IrValue iterable_temp = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = iterable_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_temp_local(ir, iterable_temp, iterable_type);
            ir_add_assign(ir,
                          iterable_temp,
                          iterable_type,
                          iterable_value,
                          iterable_type);

            IrValue index_temp = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = usize_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_temp_local(ir, index_temp, usize_type);
            ir_add_assign(ir,
                          index_temp,
                          usize_type,
                          (IrValue){.kind          = IR_VALUE_INTEGER,
                                    .type          = usize_type,
                                    .value.integer = 0},
                          usize_type);

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

            i64 start_label    = (i64)(*next_value_index)++;
            i64 continue_label = (i64)(*next_value_index)++;
            i64 end_label      = (i64)(*next_value_index)++;
            i64 else_label     = for_info->else_block_index != U32_MAX
                                     ? (i64)(*next_value_index)++
                                     : end_label;
            ir_add_label(ir, start_label);

            IrValue count_value = {0};
            if (sema->types[iterable_type].kind == STK_Array) {
                count_value = (IrValue){
                    .kind          = IR_VALUE_INTEGER,
                    .type          = usize_type,
                    .value.integer = sema->types[iterable_type].return_type,
                };
            } else {
                IrValue count_temp = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = usize_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_field(ir,
                             count_temp,
                             usize_type,
                             iterable_temp,
                             iterable_type,
                             lex_add_symbol((Lexer*)lex,
                                            s("count"),
                                            &(InternAddResult){0}));
                count_value = count_temp;
            }

            IrValue condition = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = bool_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            ir_add_less(ir,
                        condition,
                        bool_type,
                        index_temp,
                        usize_type,
                        count_value,
                        usize_type);
            ir_add_branch_false(ir, condition, bool_type, else_label);

            if (index_local_index != sema_no_local()) {
                ir_add_local(
                    ir,
                    function_index,
                    sema->locals[index_local_index].lowered_symbol_handle,
                    usize_type,
                    index_temp,
                    usize_type);
            }

            IrValue item_value = {
                .kind          = IR_VALUE_VARIABLE,
                .type          = lowered_item_type,
                .value.integer = (i64)(*next_value_index)++,
            };
            if (for_info->item_is_pointer) {
                ir_add_address_of_index(ir,
                                        item_value,
                                        lowered_item_type,
                                        iterable_temp,
                                        iterable_type,
                                        index_temp,
                                        usize_type);
            } else {
                ir_add_index(ir,
                             item_value,
                             lowered_item_type,
                             iterable_temp,
                             iterable_type,
                             index_temp,
                             usize_type);
            }
            ir_add_local(ir,
                         function_index,
                         sema->locals[item_local_index].lowered_symbol_handle,
                         lowered_item_type,
                         item_value,
                         lowered_item_type);

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
                    .defer_count    = (u32)array_count(defers->statements),
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
                        .defer_count  = (u32)array_count(defers->statements),
                    };
            }

            IrStatementResult body_result =
                ir_generate_statement(lex,
                                      ast,
                                      sema,
                                      function_index,
                                      node->b,
                                      inner_loop,
                                      node_values,
                                      next_value_index,
                                      defers,
                                      ir);
            if (body_result != IR_STMT_RETURN) {
                ir_add_label(ir, continue_label);
                IrValue next_index = {
                    .kind          = IR_VALUE_VARIABLE,
                    .type          = usize_type,
                    .value.integer = (i64)(*next_value_index)++,
                };
                ir_add_add(ir,
                           next_index,
                           usize_type,
                           index_temp,
                           usize_type,
                           (IrValue){.kind          = IR_VALUE_INTEGER,
                                     .type          = usize_type,
                                     .value.integer = 1},
                           usize_type);
                ir_add_assign(
                    ir, index_temp, usize_type, next_index, usize_type);
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
                                          defers,
                                          ir);
                if (else_result == IR_STMT_RETURN) {
                    return IR_STMT_RETURN;
                }
            }
            ir_add_label(ir, end_label);
            node_values[node_index] = result;
            return IR_STMT_FALLTHROUGH;
        }
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
        for (u32 item = 0; item < for_info->init_count; ++item) {
            ir_generate_statement(lex,
                                  ast,
                                  sema,
                                  function_index,
                                  ast->for_items[for_info->first_init + item],
                                  loop,
                                  node_values,
                                  next_value_index,
                                  defers,
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
                .defer_count    = (u32)array_count(defers->statements),
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
                    .defer_count  = (u32)array_count(defers->statements),
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
                                                              defers,
                                                              ir);
        bool              body_may_break = ir_ast_node_may_break_loop(
            ast, node->b, for_info->label_symbol, true);
        if (for_info->condition_node_index == U32_MAX &&
            body_result == IR_STMT_RETURN && !body_may_break) {
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
                    defers,
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
                                      defers,
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
        u32 local_index = sema->node_local_indices[node_index];
        u32 local_type  = ir_value_type_for_local_index(ast, sema, local_index);
        const SemaLocal* local = &sema->locals[local_index];
        if (ir_symbol_is_discard(lex, node->a)) {
            u32 value_node = node->b;
            if (ast->nodes[value_node].kind == AK_AnnotatedValue) {
                value_node = ast->nodes[value_node].b;
            }
            while (ast->nodes[value_node].kind == AK_Expression) {
                value_node = ast->nodes[value_node].a;
            }
            if (ast->nodes[value_node].kind != AK_Undefined &&
                ast->nodes[value_node].kind != AK_ZeroInit) {
                (void)ir_lower_node(lex,
                                    ast,
                                    sema,
                                    value_node,
                                    loop,
                                    node_values,
                                    next_value_index,
                                    ir);
            }
            return IR_STMT_FALLTHROUGH;
        }
        if (local_type == sema_no_type() &&
            local->type_node_index != sema_no_type()) {
            u32 type_node = local->type_node_index;
            while (ast->nodes[type_node].kind == AK_Expression) {
                type_node = ast->nodes[type_node].a;
            }
            if (type_node < array_count(sema->node_decl_indices) &&
                sema->node_decl_indices[type_node] != sema_no_decl()) {
                local_type =
                    sema->decls[sema->node_decl_indices[type_node]].type_index;
            }
        }
        if (local_type == sema_no_type() && node->b < array_count(ast->nodes) &&
            ast->nodes[node->b].kind == AK_AnnotatedValue) {
            u32 type_node = ast->nodes[node->b].a;
            while (type_node < array_count(ast->nodes) &&
                   ast->nodes[type_node].kind == AK_Expression) {
                type_node = ast->nodes[type_node].a;
            }
            if (type_node < array_count(sema->node_type_indices) &&
                sema->node_type_indices[type_node] != sema_no_type()) {
                local_type = ir_string_append_type(
                    sema, sema->node_type_indices[type_node]);
            }
        }
        if (local_type != sema_no_type() &&
            sema->types[local_type].kind == STK_DynamicArray) {
            IrValue target = {
                .kind = IR_VALUE_LOCAL,
                .type = local_type,
                .value.integer =
                    sema->locals[local_index].lowered_symbol_handle,
            };
            ir_add_local(ir,
                         function_index,
                         local->lowered_symbol_handle,
                         local_type,
                         (IrValue){
                             .kind          = IR_VALUE_INTEGER,
                             .type          = local_type,
                             .value.integer = 0,
                         },
                         local_type);

            u32 init_node = node->b;
            if (ast->nodes[init_node].kind == AK_AnnotatedValue) {
                init_node = ast->nodes[init_node].b;
            }
            while (ast->nodes[init_node].kind == AK_Expression) {
                init_node = ast->nodes[init_node].a;
            }

            u32 min_capacity =
                ir_dynarray_min_capacity(ast, sema, local->type_node_index);
            u32 min_capacity_node =
                ir_dynarray_min_capacity_node(ast, local->type_node_index);
            if (min_capacity_node != sema_no_decl() &&
                (ast->nodes[init_node].kind == AK_ZeroInit ||
                 ast->nodes[init_node].kind == AK_Undefined ||
                 ast->nodes[init_node].kind == AK_Array)) {
                IrValue capacity = ir_lower_node(lex,
                                                 ast,
                                                 sema,
                                                 min_capacity_node,
                                                 loop,
                                                 node_values,
                                                 next_value_index,
                                                 ir);
                ir_add_dynarray_op(
                    ir,
                    IR_OP_DYNARRAY_RESERVE,
                    IR_DAT_DIRECT,
                    target,
                    local_type,
                    U32_MAX,
                    capacity,
                    ir_node_type_index(ast, sema, min_capacity_node),
                    local_type);
            }

            if (ast->nodes[init_node].kind == AK_Array) {
                u32 literal_count = ast->nodes[init_node].b;
                if (literal_count > min_capacity) {
                    ir_add_dynarray_op(
                        ir,
                        IR_OP_DYNARRAY_RESERVE,
                        IR_DAT_DIRECT,
                        target,
                        local_type,
                        U32_MAX,
                        (IrValue){
                            .kind          = IR_VALUE_INTEGER,
                            .type          = ir_builtin_type(sema, STK_Usize),
                            .value.integer = literal_count,
                        },
                        ir_builtin_type(sema, STK_Usize),
                        local_type);
                }
                for (u32 i = 0; i < literal_count; ++i) {
                    u32 item_node =
                        ast->tuple_items[ast->nodes[init_node].a + i];
                    IrValue item = ir_lower_node(lex,
                                                 ast,
                                                 sema,
                                                 item_node,
                                                 loop,
                                                 node_values,
                                                 next_value_index,
                                                 ir);
                    ir_add_dynarray_op(ir,
                                       IR_OP_DYNARRAY_PUSH,
                                       IR_DAT_DIRECT,
                                       target,
                                       local_type,
                                       U32_MAX,
                                       item,
                                       ir_node_type_index(ast, sema, item_node),
                                       local_type);
                }
                return IR_STMT_FALLTHROUGH;
            }

            if (ast->nodes[init_node].kind == AK_Undefined ||
                ast->nodes[init_node].kind == AK_ZeroInit) {
                return IR_STMT_FALLTHROUGH;
            }

            IrValue value = ir_lower_node(lex,
                                          ast,
                                          sema,
                                          init_node,
                                          loop,
                                          node_values,
                                          next_value_index,
                                          ir);
            ir_add_assign(ir, target, local_type, value, local_type);
            return IR_STMT_FALLTHROUGH;
        }

        IrValue value = {
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
        } else if (ast->nodes[node->b].kind == AK_Undefined) {
            value = (IrValue){.kind = IR_VALUE_NONE};
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
        ir_add_local(ir,
                     function_index,
                     local->lowered_symbol_handle,
                     local_type,
                     value,
                     local_type);
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

    if (node->kind != AK_Statement) {
        return IR_STMT_FALLTHROUGH;
    }

    const AstNode* expr            = &ast->nodes[node->a];
    u32            expr_root_index = node->a;
    if (expr->kind == AK_Expression) {
        expr_root_index = expr->a;
        expr            = &ast->nodes[expr_root_index];
    }

    u32 first_instruction = (u32)array_count(ir->instructions);

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

    if (ir_instructions_include_runtime_string(ir, first_instruction)) {
        ir_add_string_reset(ir);
    }

    return IR_STMT_FALLTHROUGH;
}

//------------------------------------------------------------------------------
// Lower one top-level constant binding into the generated init function.

internal void ir_generate_global_init(const Lexer*    lex,
                                      const Ast*      ast,
                                      const Sema*     sema,
                                      u32             decl_index,
                                      const SemaDecl* decl,
                                      u64*            next_value_index,
                                      Ir*             ir)
{
    Array(IrValue) node_values    = ir_make_node_values(ast);
    u32 previous_global_init_decl = g_ir_current_global_init_decl;
    g_ir_current_global_init_decl = decl_index;
    if (ir_decl_uses_implicit_array_slice(ast, sema, decl)) {
        u32 backing_type = ir_decl_implicit_array_slice_type(ast, sema, decl);
        u32 backing_symbol =
            ir_hidden_backing_symbol((Lexer*)lex, decl->symbol_handle);
        u32            array_node_index = decl->value_node_index;
        const AstNode* array_node       = &ast->nodes[array_node_index];
        if (array_node->kind == AK_Expression) {
            array_node_index = array_node->a;
            array_node       = &ast->nodes[array_node_index];
        }
        ASSERT(array_node->kind == AK_Array,
               "Expected implicit slice backing to come from array literal");

        IrValue slice_value = {
            .kind          = IR_VALUE_VARIABLE,
            .type          = decl->type_index,
            .value.integer = (i64)(*next_value_index)++,
        };
        ir_add_slice(ir,
                     slice_value,
                     decl->type_index,
                     (IrValue){.kind          = IR_VALUE_SYMBOL,
                               .type          = backing_type,
                               .value.integer = backing_symbol},
                     backing_type,
                     ir_unset_value(),
                     sema_no_type(),
                     ir_unset_value(),
                     sema_no_type());
        ir_add_assign(ir,
                      (IrValue){.kind          = IR_VALUE_SYMBOL,
                                .type          = decl->type_index,
                                .value.integer = decl->symbol_handle},
                      decl->type_index,
                      slice_value,
                      decl->type_index);

        Array(IrValue) items = NULL;
        for (u32 i = 0; i < array_node->b; ++i) {
            array_push(items,
                       ir_lower_node(lex,
                                     ast,
                                     sema,
                                     ast->tuple_items[array_node->a + i],
                                     ir_no_control_labels(),
                                     node_values,
                                     next_value_index,
                                     ir));
        }
        IrValue backing_value = {
            .kind          = IR_VALUE_VARIABLE,
            .type          = backing_type,
            .value.integer = (i64)(*next_value_index)++,
        };
        ir_add_array(ir, backing_value, backing_type, items);
        array_free(items);
        ir_add_assign(ir,
                      (IrValue){.kind          = IR_VALUE_SYMBOL,
                                .type          = backing_type,
                                .value.integer = backing_symbol},
                      backing_type,
                      backing_value,
                      backing_type);
        array_free(node_values);
        g_ir_current_global_init_decl = previous_global_init_decl;
        return;
    }
    IrValue result = {
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
    g_ir_current_global_init_decl = previous_global_init_decl;
}

internal u32 ir_decl_implicit_array_slice_node_index(const Ast*      ast,
                                                     const SemaDecl* decl)
{
    if (decl->value_node_index == sema_no_decl()) {
        return sema_no_decl();
    }
    u32 node_index = decl->value_node_index;
    if (ast->nodes[node_index].kind == AK_Expression) {
        node_index = ast->nodes[node_index].a;
    }
    return node_index;
}

internal bool ir_decl_uses_implicit_array_slice(const Ast*      ast,
                                                const Sema*     sema,
                                                const SemaDecl* decl)
{
    u32 node_index = ir_decl_implicit_array_slice_node_index(ast, decl);
    return node_index != sema_no_decl() &&
           node_index < array_count(sema->node_implicit_array_type_indices) &&
           sema->node_implicit_array_type_indices[node_index] != sema_no_type();
}

internal u32 ir_decl_implicit_array_slice_type(const Ast*      ast,
                                               const Sema*     sema,
                                               const SemaDecl* decl)
{
    u32 node_index = ir_decl_implicit_array_slice_node_index(ast, decl);
    if (node_index == sema_no_decl()) {
        return sema_no_type();
    }
    return sema->node_implicit_array_type_indices[node_index];
}

internal u32 ir_hidden_backing_symbol(Lexer* lexer, u32 symbol_handle)
{
    string backing_name =
        string_format(&temp_arena,
                      STRINGP "$backing",
                      STRINGV(lex_symbol(lexer, symbol_handle)));
    InternAddResult ignored = {0};
    return lex_add_symbol(lexer, backing_name, &ignored);
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
                     sema->locals[local_index].lowered_symbol_handle,
                     sema->locals[local_index].type_index);
    }

    bool needs_string_runtime = false;
    for (u32 i = fn_def_node->a; i < fn_start_node->b; ++i) {
        if (ast->nodes[i].kind == AK_InterpolatedString) {
            string ignored = {0};
            if (!ir_eval_string_constant(lex, ast, sema, ir, i, &ignored)) {
                needs_string_runtime = true;
                break;
            }
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
        bool         has_explicit_return = false;
        IrDeferStack defers              = {0};

        for (u32 i = fn_def_node->a + 1; i < fn_start_node->b; ++i) {
            if (ast->nodes[i].kind == AK_FnStart) {
                i = ast->nodes[i].b - 1;
                continue;
            }
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
                                      &defers,
                                      ir);
            if (result == IR_STMT_RETURN) {
                has_explicit_return = true;
                break;
            }
            i = ast_block_statement_end_exclusive(ast, i) - 1;
        }

        if (!has_explicit_return) {
            ir_emit_defers(lex,
                           ast,
                           sema,
                           function_index,
                           ir_no_control_labels(),
                           node_values,
                           &next_value_index,
                           0,
                           &defers,
                           ir);
            u32  return_type = sema->types[type_index].return_type;
            bool is_main = string_eq(lex_symbol(lex, symbol_handle), s("main"));
            if (sema->types[return_type].kind == STK_Void && !is_main) {
                ir_add_return(
                    ir, (IrValue){.kind = IR_VALUE_NONE}, return_type);
            } else {
                ir_add_return(ir,
                              (IrValue){.kind = IR_VALUE_INTEGER,
                                        .type = ir_builtin_type(sema, STK_I32),
                                        .value.integer = 0},
                              ir_builtin_type(sema, STK_I32));
            }
        }
        array_free(defers.statements);
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
    u32 symbol_handle = decl->symbol_handle;
    if (decl->value_node_index != sema_no_decl() &&
        decl->value_node_index <
            array_count(sema->node_lowered_symbol_handles) &&
        sema->node_lowered_symbol_handles[decl->value_node_index] != U32_MAX) {
        symbol_handle =
            sema->node_lowered_symbol_handles[decl->value_node_index];
    }
    ir_generate_function_body(lex,
                              ast,
                              sema,
                              symbol_handle,
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
    for (u32 i = 0; i < array_count(sema->type_param_values); ++i) {
        array_push(ir.type_param_values, sema->type_param_values[i]);
    }

    for (u32 i = 0; i < array_count(sema->generic_fn_instantiations); ++i) {
        const SemaGenericFnInstantiation* inst =
            &sema->generic_fn_instantiations[i];
        Sema inst_sema               = *sema;
        inst_sema.node_decl_indices  = inst->node_decl_indices;
        inst_sema.node_local_indices = inst->node_local_indices;
        inst_sema.node_scope_indices = inst->node_scope_indices;
        inst_sema.node_lowered_symbol_handles =
            inst->node_lowered_symbol_handles;
        inst_sema.node_type_indices = inst->node_type_indices;
        inst_sema.node_method_call_decl_indices =
            inst->node_method_call_decl_indices;
        inst_sema.node_method_call_receiver_refs =
            inst->node_method_call_receiver_refs;
        inst_sema.node_method_call_receiver_derefs =
            inst->node_method_call_receiver_derefs;
        ir_generate_function_body(lex,
                                  ast,
                                  &inst_sema,
                                  inst->symbol_handle,
                                  inst->type_index,
                                  inst->fn_node_index,
                                  inst->root_scope_index,
                                  &ir);
    }

    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        const SemaDecl* decl = &sema->decls[sema->ordered_decl_indices[i]];
        if (decl->kind == SK_FfiFunction) {
            const Lexer*      ffi_lexer = lex;
            const Ast*        ffi_ast   = ast;
            const Sema*       ffi_sema  = sema;
            const AstFfiInfo* ffi_info  = NULL;
            if (!ir_decl_ffi_info(lex,
                                  ast,
                                  sema,
                                  decl,
                                  &ffi_lexer,
                                  &ffi_ast,
                                  &ffi_sema,
                                  &ffi_info)) {
                continue;
            }

            string library = {0};
            if (!ir_eval_string_constant(ffi_lexer,
                                         ffi_ast,
                                         ffi_sema,
                                         &ir,
                                         ffi_info->library_node_index,
                                         &library)) {
                error_ice("Could not evaluate FFI library string");
            }
            array_push(ir.externs,
                       (IrExtern){
                           .symbol = sema_import_symbol_handle(
                               (Lexer*)lex,
                               ffi_lexer,
                               ffi_info->foreign_symbol_handle),
                           .type    = decl->type_index,
                           .library = library,
                       });
        }
        if (ir_decl_requires_runtime(sema, decl)) {
            has_constants = true;
            ir_add_global(&ir, decl->symbol_handle, decl->type_index);
            if (ir_decl_uses_implicit_array_slice(ast, sema, decl)) {
                ir_add_global(
                    &ir,
                    ir_hidden_backing_symbol((Lexer*)lex, decl->symbol_handle),
                    ir_decl_implicit_array_slice_type(ast, sema, decl));
            }
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

        if (decl->kind != SK_Function && decl->kind != SK_GenericFunction &&
            fn_value_node != sema_no_decl() &&
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
                ir_generate_global_init(lex,
                                        ast,
                                        sema,
                                        sema->ordered_decl_indices[i],
                                        decl,
                                        &next_global_value_index,
                                        &ir);
            }
        }
        ir_add_init_end(&ir);
    }
    array_free(g_ir_local_substitutions);
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
    array_free(ir->dynarray_ops);
    array_free(ir->strings);
    array_free(ir->types);
    array_free(ir->type_param_types);
    array_free(ir->type_param_symbols);
    array_free(ir->type_param_values);
    if (ir->arena.data != NULL) {
        arena_done(&ir->arena);
    }
    *ir = (Ir){0};
}

//------------------------------------------------------------------------------
