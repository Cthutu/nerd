//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>
#if OS_POSIX
#    include <sys/stat.h>
#endif
#include <stdio.h>

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/llvm/llvm.h>

//------------------------------------------------------------------------------

#define GENERATED_C_WARNINGS                                                   \
    "-Wall -Wextra -Werror -Wno-unused-function -Wno-unused-label "            \
    "-Wno-unused-variable -Wno-unused-but-set-variable "                       \
    "-Wno-unused-parameter -Wno-format-security"

static const char g_llvm_runtime_prelude[] = {
#embed "../../../../_obj/llvm/prelude.ll"
    , 0};

typedef struct {
    const FrontEndState*      front_end_results;
    const NerdArtifactConfig* artifacts;
    bool                      verbose;
    BackEndState              results;
} BackEndContext;

typedef struct {
    Lexer lexer;
    Sema  sema;
    Ir    ir;
} ProgramBackEndMerge;

typedef struct {
    u32 source_symbol;
    u32 merged_symbol;
} ProgramBackEndSymbolRemap;

internal string back_end_copy_string(Arena* arena, string text)
{
    u8* copy = (u8*)arena_alloc(arena, text.count);
    memcpy(copy, text.data, text.count);
    return string_from(copy, text.count);
}

internal cstr back_end_cstr(Arena* arena, string text)
{
    char* copy = (char*)arena_alloc(arena, text.count + 1);
    memcpy(copy, text.data, text.count);
    copy[text.count] = '\0';
    return copy;
}

internal u32 back_end_import_ir_string(ProgramBackEndMerge* merge, string text)
{
    for (u32 i = 0; i < array_count(merge->ir.strings); ++i) {
        if (string_eq(merge->ir.strings[i], text)) {
            return i;
        }
    }

    array_push(merge->ir.strings, back_end_copy_string(&merge->ir.arena, text));
    return (u32)array_count(merge->ir.strings) - 1;
}

internal u32 back_end_find_builtin_type(const Sema* sema, SemaTypeKind kind)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == kind) {
            return i;
        }
    }
    return sema_no_type();
}

internal bool back_end_symbol_is_runtime_helper(const Lexer* lexer,
                                                u32          symbol_handle)
{
    string name = lex_symbol(lexer, symbol_handle);
    bool   is_to_string =
        name.count >= 10 && memcmp(name.data, "to_string$", 10) == 0;
    return string_eq_cstr(name, "string_eq") ||
           string_eq_cstr(name, "string_slice") ||
           string_eq_cstr(name, "string_builder_reset") ||
           string_eq_cstr(name, "string_builder_mark") ||
           string_eq_cstr(name, "string_builder_append_string") ||
           string_eq_cstr(name, "string_builder_finish") || is_to_string;
}

internal bool back_end_symbol_is_extern(const Ir* ir, u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(ir->externs); ++i) {
        if (ir->externs[i].symbol == symbol_handle) {
            return true;
        }
    }
    return false;
}

internal bool back_end_function_symbol_conflicts(const ProgramInfo* program,
                                                 u32 module_index,
                                                 u32 symbol)
{
    const ModuleInfo* module  = &program->modules[module_index];
    string            name    = lex_symbol(&module->front_end.lexer, symbol);
    u32               matches = 0;
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        const ModuleInfo* other = &program->modules[i];
        const Ir*         ir    = &other->front_end.ir;
        for (u32 fn = 0; fn < array_count(ir->functions); ++fn) {
            if (string_eq(name,
                          lex_symbol(&other->front_end.lexer,
                                     ir->functions[fn].symbol))) {
                matches++;
            }
        }
    }
    return matches > 1;
}

internal u32 back_end_module_qualified_symbol(ProgramBackEndMerge* merge,
                                              const ModuleInfo*    module,
                                              u32 source_symbol)
{
    string source_name = lex_symbol(&module->front_end.lexer, source_symbol);
    StringBuilder sb   = {0};
    sb_init(&sb, &merge->ir.arena);
    for (usize i = 0; i < module->qualified_name.count; ++i) {
        u8 ch = module->qualified_name.data[i];
        sb_append_char(&sb, ch == '.' ? '$' : (char)ch);
    }
    sb_append_char(&sb, '$');
    sb_append_string(&sb, source_name);
    InternAddResult ignored = {0};
    return lex_add_symbol(&merge->lexer, sb_to_string(&sb), &ignored);
}

internal u32 back_end_remap_function_symbol(const Lexer*         src_lexer,
                                            u32                  source_symbol,
                                            ProgramBackEndMerge* merge,
                                            Array(ProgramBackEndSymbolRemap)
                                                remaps)
{
    for (u32 i = 0; i < array_count(remaps); ++i) {
        if (remaps[i].source_symbol == source_symbol) {
            return remaps[i].merged_symbol;
        }
    }
    return sema_import_symbol_handle(&merge->lexer, src_lexer, source_symbol);
}

internal Array(ProgramBackEndSymbolRemap)
    back_end_module_function_remaps(const ProgramInfo*   program,
                                    u32                  module_index,
                                    ProgramBackEndMerge* merge)
{
    Array(ProgramBackEndSymbolRemap) remaps = NULL;
    if (module_index == program->root_module_index) {
        return remaps;
    }
    const ModuleInfo* module = &program->modules[module_index];
    const Ir*         ir     = &module->front_end.ir;
    for (u32 i = 0; i < array_count(ir->functions); ++i) {
        u32 symbol = ir->functions[i].symbol;
        if (!back_end_function_symbol_conflicts(
                program, module_index, symbol)) {
            continue;
        }
        array_push(remaps,
                   (ProgramBackEndSymbolRemap){
                       .source_symbol = symbol,
                       .merged_symbol = back_end_module_qualified_symbol(
                           merge, module, symbol),
                   });
    }
    return remaps;
}

internal IrValue back_end_remap_ir_value(const IrValue*       value,
                                         const Ir*            module_ir,
                                         const u32*           type_map,
                                         const u32*           string_map,
                                         ProgramBackEndMerge* merge,
                                         const Lexer*         module_lexer,
                                         Array(ProgramBackEndSymbolRemap)
                                             function_remaps)
{
    IrValue remapped = *value;
    if (remapped.type != sema_no_type()) {
        remapped.type = type_map[remapped.type];
    }

    switch (remapped.kind) {
    case IR_VALUE_LOCAL:
        remapped.value.integer = sema_import_symbol_handle(
            &merge->lexer, module_lexer, (u32)remapped.value.integer);
        break;
    case IR_VALUE_SYMBOL:
        remapped.value.integer = back_end_remap_function_symbol(
            module_lexer, (u32)remapped.value.integer, merge, function_remaps);
        break;
    case IR_VALUE_BUILTIN:
        remapped.value.integer = back_end_remap_function_symbol(
            module_lexer, (u32)remapped.value.integer, merge, function_remaps);
        if (!back_end_symbol_is_extern(module_ir, (u32)value->value.integer) &&
            !back_end_symbol_is_runtime_helper(module_lexer,
                                               (u32)value->value.integer)) {
            remapped.kind = IR_VALUE_SYMBOL;
        }
        break;
    case IR_VALUE_STRING:
        remapped.value.integer = string_map[(u32)remapped.value.integer];
        break;
    case IR_VALUE_VARIABLE:
    case IR_VALUE_INTEGER:
    case IR_VALUE_FLOAT:
    case IR_VALUE_NONE:
        break;
    }

    return remapped;
}

internal void back_end_collect_module_postorder(const ProgramInfo* program,
                                                u32                module_index,
                                                Array(bool) visited,
                                                Array(u32) * out_order)
{
    if (visited[module_index]) {
        return;
    }
    visited[module_index]    = true;

    const ModuleInfo* module = &program->modules[module_index];
    for (u32 i = 0; i < array_count(module->imported_module_indices); ++i) {
        back_end_collect_module_postorder(
            program, module->imported_module_indices[i], visited, out_order);
    }

    array_push(*out_order, module_index);
}

internal void back_end_copy_module_types(ProgramBackEndMerge* merge,
                                         const Lexer*         module_lexer,
                                         const Sema*          module_sema,
                                         Array(u32) * out_type_map)
{
    for (u32 i = 0; i < array_count(module_sema->types); ++i) {
        array_push(*out_type_map, (u32)array_count(merge->sema.types));
        array_push(merge->sema.types, module_sema->types[i]);
    }

    for (u32 i = 0; i < array_count(module_sema->types); ++i) {
        SemaType*       dst = &merge->sema.types[(*out_type_map)[i]];
        const SemaType* src = &module_sema->types[i];

        switch (src->kind) {
        case STK_Array:
        case STK_Slice:
        case STK_DynamicArray:
        case STK_Pointer:
            dst->first_param_type =
                src->first_param_type == sema_no_type()
                    ? sema_no_type()
                    : (*out_type_map)[src->first_param_type];
            break;
        case STK_Function:
            {
                u32 first = (u32)array_count(merge->sema.type_param_types);
                for (u32 param = 0; param < src->param_count; ++param) {
                    array_push(
                        merge->sema.type_param_types,
                        (*out_type_map)[module_sema->type_param_types
                                            [src->first_param_type + param]]);
                    array_push(merge->sema.type_param_symbols, U32_MAX);
                    array_push(merge->sema.type_param_values, 0);
                }
                dst->first_param_type = first;
                dst->return_type      = (*out_type_map)[src->return_type];
            }
            break;
        case STK_Module:
        case STK_Tuple:
        case STK_Plex:
        case STK_Union:
        case STK_Enum:
            {
                u32 first = (u32)array_count(merge->sema.type_param_types);
                for (u32 param = 0; param < src->param_count; ++param) {
                    u32 param_type =
                        module_sema
                            ->type_param_types[src->first_param_type + param];
                    array_push(merge->sema.type_param_types,
                               param_type == sema_no_type()
                                   ? sema_no_type()
                                   : (*out_type_map)[param_type]);
                    u32 symbol =
                        module_sema
                            ->type_param_symbols[src->first_param_type + param];
                    array_push(merge->sema.type_param_symbols,
                               symbol == U32_MAX
                                   ? U32_MAX
                                   : sema_import_symbol_handle(
                                         &merge->lexer, module_lexer, symbol));
                    array_push(
                        merge->sema.type_param_values,
                        module_sema
                            ->type_param_values[src->first_param_type + param]);
                }
                dst->first_param_type = first;
            }
            break;
        default:
            break;
        }
    }
}

internal void back_end_merge_program_done(ProgramBackEndMerge* merge)
{
    ir_done(&merge->ir);
    merge->sema.types              = NULL;
    merge->sema.type_param_types   = NULL;
    merge->sema.type_param_symbols = NULL;
    merge->sema.type_param_values  = NULL;
    sema_done(&merge->sema);
    lex_done(&merge->lexer);
    *merge = (ProgramBackEndMerge){0};
}

internal bool back_end_merge_program(const ProgramInfo*   program,
                                     ProgramBackEndMerge* out_merge)
{
    ProgramBackEndMerge merge = {0};
    arena_init(&merge.ir.arena);
    merge.lexer.source  = program->root_source;

    Array(bool) visited = NULL;
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        array_push(visited, false);
    }

    Array(u32) module_order = NULL;
    back_end_collect_module_postorder(
        program, program->root_module_index, visited, &module_order);
    array_free(visited);

    Array(IrInstruction) merged_init_instructions = NULL;

    for (u32 module_order_index = 0;
         module_order_index < array_count(module_order);
         ++module_order_index) {
        u32                  module_index = module_order[module_order_index];
        const ModuleInfo*    module       = &program->modules[module_index];
        const FrontEndState* front_end    = &module->front_end;
        const Ir*            module_ir    = &front_end->ir;
        Array(ProgramBackEndSymbolRemap) function_remaps =
            back_end_module_function_remaps(program, module_index, &merge);

        Array(u32) type_map = NULL;
        back_end_copy_module_types(
            &merge, &front_end->lexer, &front_end->sema, &type_map);

        Array(u32) string_map = NULL;
        for (u32 i = 0; i < array_count(module_ir->strings); ++i) {
            array_push(
                string_map,
                back_end_import_ir_string(&merge, module_ir->strings[i]));
        }

        u32 first_call_arg = (u32)array_count(merge.ir.call_args);
        for (u32 i = 0; i < array_count(module_ir->call_args); ++i) {
            IrCallArg arg = module_ir->call_args[i];
            if (arg.type != sema_no_type()) {
                arg.type = type_map[arg.type];
            }
            arg.value = back_end_remap_ir_value(&arg.value,
                                                module_ir,
                                                type_map,
                                                string_map,
                                                &merge,
                                                &front_end->lexer,
                                                function_remaps);
            array_push(merge.ir.call_args, arg);
        }

        u32 first_call = (u32)array_count(merge.ir.calls);
        for (u32 i = 0; i < array_count(module_ir->calls); ++i) {
            IrCallInfo call = module_ir->calls[i];
            call.first_arg += first_call_arg;
            array_push(merge.ir.calls, call);
        }

        u32 first_tuple_item = (u32)array_count(merge.ir.tuple_items);
        for (u32 i = 0; i < array_count(module_ir->tuple_items); ++i) {
            IrTupleItem item = module_ir->tuple_items[i];
            item.type        = type_map[item.type];
            item.symbol = item.symbol == U32_MAX
                              ? U32_MAX
                              : sema_import_symbol_handle(&merge.lexer,
                                                          &front_end->lexer,
                                                          item.symbol);
            item.value  = back_end_remap_ir_value(&item.value,
                                                  module_ir,
                                                  type_map,
                                                  string_map,
                                                  &merge,
                                                  &front_end->lexer,
                                                  function_remaps);
            array_push(merge.ir.tuple_items, item);
        }

        u32 first_tuple = (u32)array_count(merge.ir.tuples);
        for (u32 i = 0; i < array_count(module_ir->tuples); ++i) {
            IrTupleInfo tuple = module_ir->tuples[i];
            tuple.first_item += first_tuple_item;
            array_push(merge.ir.tuples, tuple);
        }

        u32 first_slice = (u32)array_count(merge.ir.slices);
        for (u32 i = 0; i < array_count(module_ir->slices); ++i) {
            IrSliceInfo slice = module_ir->slices[i];
            slice.target_type = type_map[slice.target_type];
            slice.start_type  = slice.start_type == sema_no_type()
                                    ? sema_no_type()
                                    : type_map[slice.start_type];
            slice.end_type    = slice.end_type == sema_no_type()
                                    ? sema_no_type()
                                    : type_map[slice.end_type];
            slice.target      = back_end_remap_ir_value(&slice.target,
                                                        module_ir,
                                                        type_map,
                                                        string_map,
                                                        &merge,
                                                        &front_end->lexer,
                                                        function_remaps);
            slice.start       = back_end_remap_ir_value(&slice.start,
                                                        module_ir,
                                                        type_map,
                                                        string_map,
                                                        &merge,
                                                        &front_end->lexer,
                                                        function_remaps);
            slice.end         = back_end_remap_ir_value(&slice.end,
                                                        module_ir,
                                                        type_map,
                                                        string_map,
                                                        &merge,
                                                        &front_end->lexer,
                                                        function_remaps);
            array_push(merge.ir.slices, slice);
        }

        u32 first_dynarray_op = (u32)array_count(merge.ir.dynarray_ops);
        for (u32 i = 0; i < array_count(module_ir->dynarray_ops); ++i) {
            IrDynamicArrayOpInfo op_info = module_ir->dynarray_ops[i];
            op_info.target_type          = op_info.target_type == sema_no_type()
                                               ? sema_no_type()
                                               : type_map[op_info.target_type];
            op_info.arg_type             = op_info.arg_type == sema_no_type()
                                               ? sema_no_type()
                                               : type_map[op_info.arg_type];
            op_info.dynarray_type        = type_map[op_info.dynarray_type];
            op_info.target = back_end_remap_ir_value(&op_info.target,
                                                     module_ir,
                                                     type_map,
                                                     string_map,
                                                     &merge,
                                                     &front_end->lexer,
                                                     function_remaps);
            op_info.arg    = back_end_remap_ir_value(&op_info.arg,
                                                     module_ir,
                                                     type_map,
                                                     string_map,
                                                     &merge,
                                                     &front_end->lexer,
                                                     function_remaps);
            if (op_info.field_symbol != U32_MAX) {
                op_info.field_symbol = sema_import_symbol_handle(
                    &merge.lexer, &front_end->lexer, op_info.field_symbol);
            }
            array_push(merge.ir.dynarray_ops, op_info);
        }

        for (u32 i = 0; i < array_count(module_ir->globals); ++i) {
            IrGlobal global = module_ir->globals[i];
            global.symbol   = sema_import_symbol_handle(
                &merge.lexer, &front_end->lexer, global.symbol);
            global.type = type_map[global.type];
            array_push(merge.ir.globals, global);
        }

        for (u32 i = 0; i < array_count(module_ir->externs); ++i) {
            IrExtern extern_decl = module_ir->externs[i];
            extern_decl.symbol   = sema_import_symbol_handle(
                &merge.lexer, &front_end->lexer, extern_decl.symbol);
            extern_decl.type = type_map[extern_decl.type];
            extern_decl.library =
                back_end_copy_string(&merge.ir.arena, extern_decl.library);
            array_push(merge.ir.externs, extern_decl);
        }

        u32 first_function = (u32)array_count(merge.ir.functions);
        for (u32 i = 0; i < array_count(module_ir->functions); ++i) {
            IrFunction function = module_ir->functions[i];
            function.symbol     = back_end_remap_function_symbol(
                &front_end->lexer, function.symbol, &merge, function_remaps);
            function.type = type_map[function.type];
            function.first_instruction +=
                (u32)array_count(merge.ir.instructions);
            function.one_past_last_instruction +=
                (u32)array_count(merge.ir.instructions);
            function.first_local += (u32)array_count(merge.ir.locals);
            array_push(merge.ir.functions, function);
        }

        for (u32 i = 0; i < array_count(module_ir->locals); ++i) {
            IrLocal local = module_ir->locals[i];
            local.symbol  = sema_import_symbol_handle(
                &merge.lexer, &front_end->lexer, local.symbol);
            if (local.type == sema_no_type()) {
                u32 module_void_type =
                    back_end_find_builtin_type(&front_end->sema, STK_Void);
                ASSERT(module_void_type != sema_no_type(),
                       "Expected builtin void type in module sema");
                local.type = type_map[module_void_type];
            } else {
                local.type = type_map[local.type];
            }
            local.function_index += first_function;
            array_push(merge.ir.locals, local);
        }

        bool in_module_init = false;
        for (u32 i = 0; i < array_count(module_ir->instructions); ++i) {
            IrInstruction instr = module_ir->instructions[i];
            if (instr.op == IR_OP_INIT_START) {
                in_module_init = true;
                continue;
            }
            if (instr.op == IR_OP_INIT_END) {
                in_module_init = false;
                continue;
            }
            instr.lvalue    = back_end_remap_ir_value(&instr.lvalue,
                                                      module_ir,
                                                      type_map,
                                                      string_map,
                                                      &merge,
                                                      &front_end->lexer,
                                                      function_remaps);
            instr.rvalue[0] = back_end_remap_ir_value(&instr.rvalue[0],
                                                      module_ir,
                                                      type_map,
                                                      string_map,
                                                      &merge,
                                                      &front_end->lexer,
                                                      function_remaps);
            instr.rvalue[1] = back_end_remap_ir_value(&instr.rvalue[1],
                                                      module_ir,
                                                      type_map,
                                                      string_map,
                                                      &merge,
                                                      &front_end->lexer,
                                                      function_remaps);

            switch (instr.op) {
            case IR_OP_CALL:
                instr.rvalue[1].value.integer += first_call;
                break;
            case IR_OP_TUPLE:
            case IR_OP_ARRAY:
            case IR_OP_PLEX:
                instr.rvalue[0].value.integer += first_tuple;
                break;
            case IR_OP_SLICE:
                instr.rvalue[0].value.integer += first_slice;
                break;
            case IR_OP_DYNARRAY_RESERVE:
            case IR_OP_DYNARRAY_PUSH:
            case IR_OP_DYNARRAY_APPEND:
            case IR_OP_DYNARRAY_CLEAR:
            case IR_OP_DYNARRAY_FREE:
                instr.lvalue.value.integer += first_dynarray_op;
                break;
            case IR_OP_DYNARRAY_POP:
                instr.rvalue[0].value.integer += first_dynarray_op;
                break;
            case IR_OP_SIZE:
                if (module_ir->instructions[i].rvalue[0].value.integer !=
                    sema_no_type()) {
                    instr.rvalue[0].value.integer = type_map
                        [module_ir->instructions[i].rvalue[0].value.integer];
                }
                break;
            case IR_OP_FIELD:
                instr.rvalue[1].value.integer = sema_import_symbol_handle(
                    &merge.lexer,
                    &front_end->lexer,
                    (u32)module_ir->instructions[i].rvalue[1].value.integer);
                break;
            case IR_OP_STORE_FIELD:
                instr.rvalue[0].value.integer = sema_import_symbol_handle(
                    &merge.lexer,
                    &front_end->lexer,
                    (u32)module_ir->instructions[i].rvalue[0].value.integer);
                break;
            case IR_OP_ADDRESS_OF_FIELD_PTR:
                instr.rvalue[1].value.integer = sema_import_symbol_handle(
                    &merge.lexer,
                    &front_end->lexer,
                    (u32)module_ir->instructions[i].rvalue[1].value.integer);
                break;
            default:
                break;
            }

            if (in_module_init) {
                array_push(merged_init_instructions, instr);
            } else {
                array_push(merge.ir.instructions, instr);
            }
        }

        array_free(type_map);
        array_free(string_map);
        array_free(function_remaps);
    }

    if (array_count(merged_init_instructions) > 0) {
        array_push(merge.ir.instructions,
                   (IrInstruction){
                       .op = IR_OP_INIT_START,
                   });
        for (u32 i = 0; i < array_count(merged_init_instructions); ++i) {
            array_push(merge.ir.instructions, merged_init_instructions[i]);
        }
        array_push(merge.ir.instructions,
                   (IrInstruction){
                       .op = IR_OP_INIT_END,
                   });
    }
    array_free(merged_init_instructions);

    merge.ir.types                = merge.sema.types;
    merge.ir.type_param_types     = merge.sema.type_param_types;
    merge.ir.type_param_symbols   = merge.sema.type_param_symbols;
    merge.ir.type_param_values    = merge.sema.type_param_values;
    merge.sema.types              = NULL;
    merge.sema.type_param_types   = NULL;
    merge.sema.type_param_symbols = NULL;
    merge.sema.type_param_values  = NULL;

    array_free(module_order);
    *out_merge = merge;
    return true;
}

internal NerdArtifactConfig compiler_default_artifacts(void)
{
    return (NerdArtifactConfig){
        .binary_path      = "a.out",
        .hir_path         = "_a.hir",
        .ir_path          = "_a.ir",
        .llvm_path        = "_a.ll",
        .c_path           = "_a.gen.c",
        .emit_hir_file    = false,
        .emit_ir_file     = false,
        .emit_llvm_file   = false,
        .emit_c_file      = false,
        .use_llvm_backend = false,
        .compile_binary   = true,
        .release          = false,
    };
}

internal bool back_end_timing_add(Timing* timing,
                                  cstr    phase,
                                  bool (*run)(BackEndContext*),
                                  BackEndContext* ctx)
{
    if (timing == NULL) {
        return run(ctx);
    }

    ThreadTimePoint start  = thread_time_now();
    bool            result = run(ctx);
    ThreadTimePoint end    = thread_time_now();
    timing_add(timing,
               COMPILER_STAGE_BACK_END,
               phase,
               thread_time_elapsed(start, end));
    return result;
}

internal bool back_end_cgen(BackEndContext* ctx)
{
    ctx->results.cgen = cgen_init(&ctx->front_end_results->ir,
                                  &ctx->front_end_results->lexer,
                                  &ctx->front_end_results->sema);
    return true;
}

internal bool back_end_save_c(BackEndContext* ctx)
{
    if (!ctx->artifacts->emit_c_file && !ctx->artifacts->compile_binary) {
        return true;
    }

    return cgen_save(&ctx->results.cgen, ctx->artifacts->c_path);
}

internal bool back_end_compile_c(BackEndContext* ctx)
{
    if (!ctx->artifacts->compile_binary) {
        return true;
    }

    Arena arena = {0};
    arena_init(&arena);

    cstr          c_path     = ctx->artifacts->c_path;
    cstr          exe_path   = ctx->artifacts->binary_path;
    StringBuilder link_flags = {0};
    sb_init(&link_flags, &arena);
    const Ir* ir = &ctx->front_end_results->ir;
    for (u32 i = 0; i < array_count(ir->externs); ++i) {
        string library = ir->externs[i].library;
        if (string_eq(library, s("c"))) {
            continue;
        }
#if OS_WINDOWS
        if (string_eq(library, s("m"))) {
            continue;
        }
#endif

        bool already_added = false;
        for (u32 j = 0; j < i; ++j) {
            string previous = ir->externs[j].library;
            if (string_eq(previous, library)) {
                already_added = true;
                break;
            }
        }
        if (!already_added) {
            sb_format(&link_flags, " -l" STRINGP, STRINGV(library));
        }
    }
#if OS_POSIX
    string command =
        ctx->artifacts->release
            ? string_format(&arena,
                            "clang " GENERATED_C_WARNINGS " -O2 -DNDEBUG -o "
                            "\"%s\" \"%s\"" STRINGP,
                            exe_path,
                            c_path,
                            STRINGV(sb_to_string(&link_flags)))
            : string_format(&arena,
                            "clang " GENERATED_C_WARNINGS " -g -O0 -DDEBUG -o "
                            "\"%s\" \"%s\"" STRINGP,
                            exe_path,
                            c_path,
                            STRINGV(sb_to_string(&link_flags)));
    int compile_result = shell((cstr)command.data);
    if (compile_result != 0) {
        arena_done(&arena);
        return error_runtime(
            "Failed to compile generated C file (exit code %d)",
            compile_result);
    }
    if (chmod(exe_path, 0755) != 0) {
        arena_done(&arena);
        return error_runtime("Failed to make %s executable", exe_path);
    }
#elif OS_WINDOWS
    string command =
        ctx->artifacts->release
            ? string_format(&arena,
                            "clang " GENERATED_C_WARNINGS
                            " -D_CRT_SECURE_NO_WARNINGS -O2 -DNDEBUG -o "
                            "\"%s\" \"%s\"" STRINGP,
                            exe_path,
                            c_path,
                            STRINGV(sb_to_string(&link_flags)))
            : string_format(&arena,
                            "clang " GENERATED_C_WARNINGS
                            " -D_CRT_SECURE_NO_WARNINGS -g -O0 -DDEBUG -o "
                            "\"%s\" \"%s\"" STRINGP,
                            exe_path,
                            c_path,
                            STRINGV(sb_to_string(&link_flags)));
    int compile_result = shell((cstr)command.data);
    if (compile_result != 0) {
        arena_done(&arena);
        return error_runtime(
            "Failed to compile generated C file (exit code %d)",
            compile_result);
    }
#endif

    if (!ctx->artifacts->emit_c_file) {
        path_remove(c_path);
    }

    arena_done(&arena);
    return true;
}

internal void
back_end_append_program_extern_link_flags(StringBuilder*     link_flags,
                                          const ProgramInfo* program)
{
    for (u32 module_index = 0; module_index < array_count(program->modules);
         ++module_index) {
        const Ir* ir = &program->modules[module_index].front_end.ir;
        for (u32 i = 0; i < array_count(ir->externs); ++i) {
            string library = ir->externs[i].library;
            if (string_eq(library, s("c"))) {
                continue;
            }
#if OS_WINDOWS
            if (string_eq(library, s("m"))) {
                continue;
            }
#endif

            bool already_added = false;
            for (u32 previous_module = 0; previous_module <= module_index;
                 ++previous_module) {
                const Ir* previous_ir =
                    &program->modules[previous_module].front_end.ir;
                u32 end = previous_module == module_index
                              ? i
                              : (u32)array_count(previous_ir->externs);
                for (u32 j = 0; j < end; ++j) {
                    if (string_eq(previous_ir->externs[j].library, library)) {
                        already_added = true;
                        break;
                    }
                }
                if (already_added) {
                    break;
                }
            }
            if (!already_added) {
                sb_format(link_flags, " -l" STRINGP, STRINGV(library));
            }
        }
    }
}

internal bool back_end_write_text_file(cstr path, string text)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(text.data, 1, text.count, file);
    bool  close_failed = fclose(file) != 0;
    if (written != text.count || close_failed) {
        return error_runtime("Failed to write file: %s", path);
    }
    return true;
}

internal bool back_end_hir_has_globals(const Hir* hir)
{
    for (u32 i = 0; i < array_count(hir->values); ++i) {
        if (hir->values[i].kind == HIR_VALUE_Global) {
            return true;
        }
    }
    return false;
}

internal cstr back_end_module_llvm_path(Arena*                    arena,
                                        const NerdArtifactConfig* artifacts,
                                        u32                       module_index)
{
    if (module_index == 0) {
        return artifacts->llvm_path;
    }
    return back_end_cstr(
        arena,
        string_format(
            arena, "%s.m%u.ll", artifacts->binary_path, module_index));
}

internal bool back_end_root_main_returns_void(const FrontEndState* root)
{
    const Hir*   hir   = &root->hir;
    const Lexer* lexer = &root->lexer;
    const Sema*  sema  = &root->sema;
    for (u32 i = 0; i < array_count(hir->bindings); ++i) {
        const HirBinding* binding = &hir->bindings[i];
        if (binding->kind != HIR_BINDING_Function ||
            binding->target_index >= array_count(hir->functions) ||
            !string_eq_cstr(lex_symbol(lexer, binding->symbol_handle),
                            "main")) {
            continue;
        }

        const HirFunction* function = &hir->functions[binding->target_index];
        if (function->type_index >= array_count(sema->types) ||
            sema->types[function->type_index].kind != STK_Function) {
            return false;
        }
        u32 return_type = sema->types[function->type_index].return_type;
        return return_type < array_count(sema->types) &&
               sema->types[return_type].kind == STK_Void;
    }
    return false;
}

internal bool back_end_compile_llvm_program(const ProgramInfo*        program,
                                            const NerdArtifactConfig* artifacts)
{
    if (!artifacts->compile_binary && !artifacts->emit_llvm_file) {
        return true;
    }
    if (program->root_module_index >= array_count(program->modules)) {
        return false;
    }

    Arena arena = {0};
    arena_init(&arena);

    Array(cstr) llvm_paths         = NULL;
    Array(u32) init_module_indices = NULL;
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        const FrontEndState* front_end = &program->modules[i].front_end;
        cstr llvm_path = back_end_module_llvm_path(&arena, artifacts, i);
        if (!llvm_save_hir(&front_end->hir,
                           &front_end->lexer,
                           &front_end->sema,
                           llvm_path)) {
            array_free(llvm_paths);
            array_free(init_module_indices);
            arena_done(&arena);
            return false;
        }
        array_push(llvm_paths, llvm_path);
        if (back_end_hir_has_globals(&front_end->hir)) {
            array_push(init_module_indices, i);
        }
    }
    if (!artifacts->compile_binary) {
        array_free(llvm_paths);
        array_free(init_module_indices);
        arena_done(&arena);
        return true;
    }

    cstr runtime_prelude_path = back_end_cstr(
        &arena,
        string_format(&arena, "%s.prelude.ll", artifacts->binary_path));
    cstr runtime_epilogue_path = back_end_cstr(
        &arena,
        string_format(&arena, "%s.epilogue.ll", artifacts->binary_path));
    cstr init_ll_path = back_end_cstr(
        &arena, string_format(&arena, "%s.init.ll", artifacts->binary_path));

    const FrontEndState* root =
        &program->modules[program->root_module_index].front_end;
    bool   root_main_returns_void = back_end_root_main_returns_void(root);
    string runtime_epilogue =
        root_main_returns_void
            ? s("declare void @init()\n"
                "declare void @$main()\n"
                "\n"
                "define i32 @main() {\n"
                "  call void @init()\n"
                "  call void @$main()\n"
                "  ret i32 0\n"
                "}\n")
            : s("declare void @init()\n"
                "declare i32 @$main()\n"
                "\n"
                "define i32 @main() {\n"
                "  call void @init()\n"
                "  %result = call i32 @$main()\n"
                "  ret i32 %result\n"
                "}\n");
    StringBuilder init_ll_builder = {0};
    sb_init(&init_ll_builder, &arena);
    for (u32 i = 0; i < array_count(init_module_indices); ++i) {
        sb_format(&init_ll_builder,
                  "declare void @m%u.init()\n",
                  init_module_indices[i]);
    }
    if (array_count(init_module_indices) > 0) {
        sb_append_char(&init_ll_builder, '\n');
    }
    sb_append_cstr(&init_ll_builder, "define void @init() {\n");
    for (u32 i = 0; i < array_count(init_module_indices); ++i) {
        sb_format(&init_ll_builder,
                  "  call void @m%u.init()\n",
                  init_module_indices[i]);
    }
    sb_append_cstr(&init_ll_builder, "  ret void\n}\n");
    string init_ll = sb_to_string(&init_ll_builder);
    if (!back_end_write_text_file(runtime_prelude_path,
                                  s(g_llvm_runtime_prelude)) ||
        !back_end_write_text_file(runtime_epilogue_path, runtime_epilogue) ||
        !back_end_write_text_file(init_ll_path, init_ll)) {
        array_free(llvm_paths);
        array_free(init_module_indices);
        arena_done(&arena);
        return false;
    }

    string opt_flags = artifacts->release ? s("-O2") : s("-g -O0");
    StringBuilder link_flags = {0};
    sb_init(&link_flags, &arena);
    back_end_append_program_extern_link_flags(&link_flags, program);
    StringBuilder command_builder = {0};
    sb_init(&command_builder, &arena);
    sb_format(&command_builder,
              "clang -Wno-override-module " STRINGP
              " -o \"%s\" \"%s\"",
              STRINGV(opt_flags),
              artifacts->binary_path,
              runtime_prelude_path);
    for (u32 i = 0; i < array_count(llvm_paths); ++i) {
        sb_format(&command_builder, " \"%s\"", llvm_paths[i]);
    }
    sb_format(&command_builder, " \"%s\"", runtime_epilogue_path);
    sb_format(&command_builder, " \"%s\"", init_ll_path);
    sb_append_string(&command_builder, sb_to_string(&link_flags));
    string command        = sb_to_string(&command_builder);
    int    compile_result = shell(back_end_cstr(&arena, command));
    if (compile_result != 0) {
        array_free(llvm_paths);
        array_free(init_module_indices);
        arena_done(&arena);
        return error_runtime(
            "Failed to compile generated LLVM file (exit code %d)",
            compile_result);
    }

#if OS_POSIX
    if (chmod(artifacts->binary_path, 0755) != 0) {
        array_free(llvm_paths);
        array_free(init_module_indices);
        arena_done(&arena);
        return error_runtime("Failed to make %s executable",
                             artifacts->binary_path);
    }
#endif

    path_remove(runtime_prelude_path);
    path_remove(runtime_epilogue_path);
    path_remove(init_ll_path);
    if (!artifacts->emit_llvm_file) {
        for (u32 i = 0; i < array_count(llvm_paths); ++i) {
            path_remove(llvm_paths[i]);
        }
    }

    array_free(llvm_paths);
    array_free(init_module_indices);
    arena_done(&arena);
    return true;
}

bool back_end(const FrontEndState*      front_end_results,
              const NerdArtifactConfig* artifacts,
              bool                      verbose,
              Timing*                   timing,
              BackEndState*             out_results)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    if (artifacts->emit_ir_file) {
        if (!ir_save(&front_end_results->ir,
                     &front_end_results->lexer,
                     artifacts->ir_path)) {
            return false;
        }
    }

    BackEndContext ctx = {.front_end_results = front_end_results,
                          .artifacts         = artifacts,
                          .verbose           = verbose,
                          .results           = {0}};
    bool           result =
        back_end_timing_add(timing, COMPILER_PHASE_C_GEN, back_end_cgen, &ctx);

    if (result && verbose) {
        cgen_dump(&ctx.results.cgen);
    }

    if (result) {
        result = back_end_timing_add(
            timing, COMPILER_PHASE_C_SAVE, back_end_save_c, &ctx);
    }

    if (result) {
        result = back_end_timing_add(
            timing, COMPILER_PHASE_C_COMPILE, back_end_compile_c, &ctx);
    }

    if (out_results != NULL) {
        *out_results = ctx.results;
    } else {
        back_end_results_done(&ctx.results);
    }

    return result;
}

bool back_end_program(const ProgramInfo*        program,
                      const NerdArtifactConfig* artifacts,
                      bool                      verbose,
                      Timing*                   timing)
{
    NerdArtifactConfig default_artifacts = compiler_default_artifacts();
    if (!artifacts) {
        artifacts = &default_artifacts;
    }

    if (artifacts->emit_hir_file &&
        program->root_module_index < array_count(program->modules)) {
        const FrontEndState* root =
            &program->modules[program->root_module_index].front_end;
        if (!hir_save(
                &root->hir, &root->lexer, &root->sema, artifacts->hir_path)) {
            return false;
        }
    }

    if (artifacts->emit_llvm_file &&
        program->root_module_index < array_count(program->modules)) {
        const FrontEndState* root =
            &program->modules[program->root_module_index].front_end;
        if (!llvm_save_hir(
                &root->hir, &root->lexer, &root->sema, artifacts->llvm_path)) {
            return false;
        }
    }

    if (artifacts->use_llvm_backend) {
        return back_end_compile_llvm_program(program, artifacts);
    }

    ProgramBackEndMerge merge = {0};
    if (!back_end_merge_program(program, &merge)) {
        back_end_merge_program_done(&merge);
        return false;
    }

    FrontEndState merged_front_end = {
        .lexer = merge.lexer,
        .sema  = merge.sema,
        .ir    = merge.ir,
    };

    bool result = back_end(&merged_front_end, artifacts, verbose, timing, NULL);

    back_end_merge_program_done(&merge);
    return result;
}

void back_end_results_done(BackEndState* results)
{
    cgen_done(&results->cgen);
    results->cgen = (CGen){0};

    *results      = (BackEndState){0};
}

//------------------------------------------------------------------------------
