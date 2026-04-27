//------------------------------------------------------------------------------
// Back-end orchestration
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/internal.h>
#if OS_POSIX
#    include <sys/stat.h>
#endif

#include <compiler/build/back/back.h>
#include <compiler/build/front/front.h>
#include <compiler/error/error.h>

//------------------------------------------------------------------------------

#define GENERATED_C_WARNINGS                                                   \
    "-Wall -Wextra -Werror -Wno-unused-function -Wno-unused-label "            \
    "-Wno-unused-variable -Wno-format-security"

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

internal string back_end_copy_string(Arena* arena, string text)
{
    u8* copy = (u8*)arena_alloc(arena, text.count);
    memcpy(copy, text.data, text.count);
    return string_from(copy, text.count);
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

internal IrValue back_end_remap_ir_value(const IrValue*       value,
                                         const Ir*            module_ir,
                                         const u32*           type_map,
                                         const u32*           string_map,
                                         ProgramBackEndMerge* merge,
                                         const Lexer*         module_lexer)
{
    IrValue remapped = *value;
    if (remapped.type != sema_no_type()) {
        remapped.type = type_map[remapped.type];
    }

    switch (remapped.kind) {
    case IR_VALUE_LOCAL:
    case IR_VALUE_SYMBOL:
        remapped.value.integer = sema_import_symbol_handle(
            &merge->lexer, module_lexer, (u32)remapped.value.integer);
        break;
    case IR_VALUE_BUILTIN:
        remapped.value.integer = sema_import_symbol_handle(
            &merge->lexer, module_lexer, (u32)remapped.value.integer);
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

        Array(u32) type_map               = NULL;
        for (u32 i = 0; i < array_count(front_end->sema.types); ++i) {
            array_push(type_map,
                       sema_import_type(&merge.lexer,
                                        &merge.sema,
                                        &front_end->lexer,
                                        &front_end->sema,
                                        i));
        }

        Array(u32) string_map = NULL;
        for (u32 i = 0; i < array_count(module_ir->strings); ++i) {
            array_push(
                string_map,
                back_end_import_ir_string(&merge, module_ir->strings[i]));
        }

        u32 first_call_arg = (u32)array_count(merge.ir.call_args);
        for (u32 i = 0; i < array_count(module_ir->call_args); ++i) {
            IrCallArg arg = module_ir->call_args[i];
            arg.type      = type_map[arg.type];
            arg.value     = back_end_remap_ir_value(&arg.value,
                                                    module_ir,
                                                    type_map,
                                                    string_map,
                                                    &merge,
                                                    &front_end->lexer);
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
                                                  &front_end->lexer);
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
                                                        &front_end->lexer);
            slice.start       = back_end_remap_ir_value(&slice.start,
                                                        module_ir,
                                                        type_map,
                                                        string_map,
                                                        &merge,
                                                        &front_end->lexer);
            slice.end         = back_end_remap_ir_value(&slice.end,
                                                        module_ir,
                                                        type_map,
                                                        string_map,
                                                        &merge,
                                                        &front_end->lexer);
            array_push(merge.ir.slices, slice);
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
            function.symbol     = sema_import_symbol_handle(
                &merge.lexer, &front_end->lexer, function.symbol);
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
            local.type = type_map[local.type];
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
                                                      &front_end->lexer);
            instr.rvalue[0] = back_end_remap_ir_value(&instr.rvalue[0],
                                                      module_ir,
                                                      type_map,
                                                      string_map,
                                                      &merge,
                                                      &front_end->lexer);
            instr.rvalue[1] = back_end_remap_ir_value(&instr.rvalue[1],
                                                      module_ir,
                                                      type_map,
                                                      string_map,
                                                      &merge,
                                                      &front_end->lexer);

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
        .binary_path    = "a.out",
        .ir_path        = "_a.ir",
        .c_path         = "_a.gen.c",
        .emit_ir_file   = false,
        .emit_c_file    = false,
        .compile_binary = true,
        .release        = false,
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
