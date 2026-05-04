//------------------------------------------------------------------------------
// Whole-program front-end loading
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
#include <compiler/internal.h>
#include <compiler/modules/modules.h>

//------------------------------------------------------------------------------

internal string program_copy_string(Arena* arena, string value)
{
    u8* copy = (u8*)arena_alloc(arena, value.count);
    memcpy(copy, value.data, value.count);
    return (string){.data = copy, .count = value.count};
}

internal cstr program_copy_cstr(Arena* arena, cstr value)
{
    usize len  = strlen(value);
    char* copy = (char*)arena_alloc(arena, len + 1);
    memcpy(copy, value, len + 1);
    return copy;
}

internal cstr program_copy_cstr_from_string(Arena* arena, string value)
{
    char* copy = (char*)arena_alloc(arena, value.count + 1);
    memcpy(copy, value.data, value.count);
    copy[value.count] = '\0';
    return copy;
}

internal void program_rebind_sema_programs(ProgramInfo* program)
{
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        program->modules[i].front_end.sema.program = program;
    }
}

internal bool
program_run_timed(Timing* timing, cstr phase, bool (*run)(void*), void* data)
{
    if (timing == NULL) {
        return run(data);
    }

    ThreadTimePoint start  = thread_time_now();
    bool            result = run(data);
    ThreadTimePoint end    = thread_time_now();
    timing_add(timing,
               COMPILER_STAGE_FRONT_END,
               phase,
               thread_time_elapsed(start, end));
    return result;
}

typedef struct {
    NerdSource      source;
    FrontEndOptions options;
    FrontEndState*  front_end;
} ProgramFrontEndContext;

internal bool program_front_end_lex(void* data)
{
    ProgramFrontEndContext* ctx = data;
    return lex(ctx->source, &ctx->front_end->lexer);
}

internal bool program_front_end_parse(void* data)
{
    ProgramFrontEndContext* ctx = data;
    ctx->front_end->ast         = ast_parse(&ctx->front_end->lexer);

    if (array_count(ctx->front_end->lexer.tokens) > 0 &&
        array_count(ctx->front_end->ast.nodes) == 0) {
        return false;
    }
    return true;
}

internal bool program_front_end_sema(void* data)
{
    ProgramFrontEndContext* ctx = data;
    return sema_analyse(&ctx->front_end->lexer,
                        &ctx->front_end->ast,
                        &ctx->options,
                        &ctx->front_end->sema);
}

internal bool program_front_end_ir(void* data)
{
    ProgramFrontEndContext* ctx = data;
    ctx->front_end->ir          = ir_generate(
        &ctx->front_end->lexer, &ctx->front_end->ast, &ctx->front_end->sema);
    return true;
}

internal bool program_front_end_parse_only(NerdSource             source,
                                           const FrontEndOptions* options,
                                           Timing*                timing,
                                           FrontEndState*         front_end)
{
    ProgramFrontEndContext ctx = {
        .source    = source,
        .options   = options ? *options : (FrontEndOptions){0},
        .front_end = front_end,
    };

    bool result = program_run_timed(
        timing, COMPILER_PHASE_LEX, program_front_end_lex, &ctx);
    if (result && ctx.options.verbose) {
        lex_dump(&ctx.front_end->lexer);
    }

    if (result) {
        result = program_run_timed(
            timing, COMPILER_PHASE_PARSE, program_front_end_parse, &ctx);
        if (result && ctx.options.verbose) {
            ast_dump(&ctx.front_end->ast, &ctx.front_end->lexer);
        }
    }

    return result;
}

internal bool program_front_end_finish(ProgramInfo*           program,
                                       u32                    module_index,
                                       const FrontEndOptions* options,
                                       Timing*                timing,
                                       bool require_entry_point)
{
    ModuleInfo*     module         = &program->modules[module_index];
    FrontEndOptions module_options = options ? *options : (FrontEndOptions){0};
    module_options.program         = program;
    module_options.current_module_index = module_index;
    module_options.require_entry_point  = require_entry_point;

    ProgramFrontEndContext ctx          = {
        .source =
            {
                .source      = module->front_end.lexer.source.source,
                .source_path = module->front_end.lexer.source.source_path,
            },
        .options   = module_options,
        .front_end = &module->front_end,
    };

    bool result = program_run_timed(
        timing, COMPILER_PHASE_SEMA, program_front_end_sema, &ctx);
    return result;
}

internal bool program_front_end_generate_ir(ProgramInfo*           program,
                                            const FrontEndOptions* options,
                                            Timing*                timing)
{
    FrontEndOptions effective_options =
        options ? *options : (FrontEndOptions){0};
    if (effective_options.skip_ir_generation) {
        return true;
    }

    for (u32 i = 0; i < array_count(program->modules); ++i) {
        ModuleInfo*            module = &program->modules[i];
        ProgramFrontEndContext ctx    = {
            .source =
                {
                    .source      = module->front_end.lexer.source.source,
                    .source_path = module->front_end.lexer.source.source_path,
                },
            .options   = effective_options,
            .front_end = &module->front_end,
        };
        if (!program_run_timed(
                timing, COMPILER_PHASE_IR_GEN, program_front_end_ir, &ctx)) {
            return false;
        }
        if (effective_options.verbose) {
            ir_dump(&module->front_end.ir, &module->front_end.lexer);
        }
    }

    return true;
}

internal bool program_keyword_is_defined(const FrontEndOptions* options,
                                         string                 name)
{
    if (!options->release && string_eq_cstr(name, "debug")) {
        return true;
    }
#if OS_WINDOWS
    if (string_eq_cstr(name, "windows")) {
        return true;
    }
#endif
#if OS_LINUX
    if (string_eq_cstr(name, "linux")) {
        return true;
    }
#endif
#if OS_MACOS
    if (string_eq_cstr(name, "macos")) {
        return true;
    }
#endif
#if OS_BSD
    if (string_eq_cstr(name, "bsd")) {
        return true;
    }
#endif
#if OS_POSIX
    if (string_eq_cstr(name, "posix")) {
        return true;
    }
#endif

    for (u32 i = 0; i < array_count(options->keywords); ++i) {
        if (string_eq(name, options->keywords[i])) {
            return true;
        }
    }
    return false;
}

internal bool program_top_on_is_enabled(const FrontEndOptions* options,
                                        const Lexer*           lexer,
                                        const Ast*             ast,
                                        const AstNode*         node)
{
    ASSERT(node->kind == AK_TopOn, "Expected top-level on node");
    const AstTopOnInfo* info    = &ast->top_ons[node->a];
    bool                enabled = program_keyword_is_defined(
        options, lex_symbol(lexer, info->symbol_handle));
    return info->is_negated ? !enabled : enabled;
}

internal u32 program_find_module_by_path(const ProgramInfo* program,
                                         cstr               resolved_path)
{
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        if (strcmp(program->modules[i].resolved_path, resolved_path) == 0) {
            return i;
        }
    }
    return U32_MAX;
}

internal bool program_source_is_folder_module(NerdSource source)
{
    Arena temp = {0};
    arena_init(&temp);
    cstr file_path = module_source_file_path(&temp, source);
    bool result    = false;
    if (file_path != NULL) {
        result = string_eq_cstr(path_filename(s(file_path)), "mod.n");
    }
    arena_done(&temp);
    return result;
}

internal bool program_path_is_module_part_file(cstr path)
{
    string filename = path_filename(s(path));
    if (string_eq_cstr(filename, "mod.n") ||
        !path_has_extension(filename, ".n")) {
        return false;
    }
    return true;
}

internal int program_compare_cstr_ptr(const void* lhs, const void* rhs)
{
    const cstr* a = lhs;
    const cstr* b = rhs;
    return strcmp(*a, *b);
}

internal Array(cstr)
    program_collect_implicit_part_paths(Arena* arena, cstr module_dir)
{
    Array(cstr) part_paths = NULL;
    DirIter iter           = {0};
    if (!dir_iter_init(&iter, module_dir)) {
        return NULL;
    }

    cstr path         = NULL;
    bool is_directory = false;
    while (dir_iter_next(&iter, arena, &path, &is_directory)) {
        if (is_directory || !program_path_is_module_part_file(path)) {
            continue;
        }
        array_push(part_paths, path);
    }
    dir_iter_done(&iter);

    if (array_count(part_paths) > 1) {
        qsort(part_paths,
              array_count(part_paths),
              sizeof(part_paths[0]),
              program_compare_cstr_ptr);
    }

    return part_paths;
}

internal bool program_path_matches_source(NerdSource source, cstr path)
{
    Arena temp = {0};
    arena_init(&temp);
    cstr source_path = module_source_file_path(&temp, source);
    bool result      = source_path != NULL && strcmp(source_path, path) == 0;
    arena_done(&temp);
    return result;
}

internal bool program_mod_ref_matches_path(const Lexer*         lexer,
                                           const Ast*           ast,
                                           const AstModulePath* module_path,
                                           cstr                 path)
{
    if (module_path->symbol_count != 1) {
        return false;
    }

    string module_name =
        lex_symbol(lexer, ast->module_path_symbols[module_path->first_symbol]);
    return string_eq(module_name, path_stem(s(path)));
}

internal bool program_mod_explicitly_uses_child_path(const Lexer* lexer,
                                                     const Ast*   ast,
                                                     cstr         path)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_ModRef) {
            continue;
        }
        if (program_mod_ref_matches_path(
                lexer, ast, &ast->module_paths[node->a], path)) {
            return true;
        }
    }
    return false;
}

internal bool
program_parse_module_header(NerdSource source, Lexer* out_lexer, Ast* out_ast)
{
    if (!lex(source, out_lexer)) {
        return false;
    }

    *out_ast = ast_parse(out_lexer);
    return array_count(out_ast->nodes) > 0 ||
           array_count(out_lexer->tokens) == 0;
}

internal bool program_expand_part_root(ProgramInfo* program,
                                       NerdSource   source,
                                       NerdSource*  out_source)
{
    Arena temp = {0};
    arena_init(&temp);

    cstr source_path = module_source_file_path(&temp, source);
    if (source_path == NULL || !program_path_is_module_part_file(source_path)) {
        arena_done(&temp);
        *out_source = source;
        return true;
    }

    cstr module_dir = path_dirname(&temp, source_path);
    cstr mod_path   = path_join(&temp, module_dir, "mod.n");
    if (!path_exists(mod_path) || path_is_directory(mod_path)) {
        arena_done(&temp);
        *out_source = source;
        return true;
    }

    FileMap mod_map    = {0};
    string  mod_source = filemap_load(mod_path, &mod_map);
    if (mod_source.data == NULL) {
        arena_done(&temp);
        *out_source = source;
        return true;
    }

    Lexer mod_lexer = {0};
    Ast   mod_ast   = {0};
    if (!program_parse_module_header(
            (NerdSource){.source = mod_source, .source_path = s(mod_path)},
            &mod_lexer,
            &mod_ast)) {
        ast_done(&mod_ast);
        lex_done(&mod_lexer);
        filemap_unload(&mod_map);
        arena_done(&temp);
        *out_source = source;
        return true;
    }
    if (program_mod_explicitly_uses_child_path(
            &mod_lexer, &mod_ast, source_path)) {
        ast_done(&mod_ast);
        lex_done(&mod_lexer);
        filemap_unload(&mod_map);
        arena_done(&temp);
        *out_source = source;
        return true;
    }

    StringBuilder sb = {0};
    sb_init(&sb, &program->arena);
    sb_append_string(&sb, mod_source);
    if (mod_source.count == 0 ||
        mod_source.data[mod_source.count - 1] != '\n') {
        sb_append_char(&sb, '\n');
    }

    Array(cstr) part_paths =
        program_collect_implicit_part_paths(&temp, module_dir);

    for (u32 i = 0; i < array_count(part_paths); ++i) {
        if (program_mod_explicitly_uses_child_path(
                &mod_lexer, &mod_ast, part_paths[i])) {
            continue;
        }

        sb_append_char(&sb, '\n');
        if (program_path_matches_source(source, part_paths[i])) {
            sb_append_string(&sb, source.source);
            if (source.source.count == 0 ||
                source.source.data[source.source.count - 1] != '\n') {
                sb_append_char(&sb, '\n');
            }
            continue;
        }

        FileMap part_map    = {0};
        string  part_source = filemap_load(part_paths[i], &part_map);
        if (part_source.data == NULL) {
            continue;
        }
        sb_append_string(&sb, part_source);
        if (part_source.count == 0 ||
            part_source.data[part_source.count - 1] != '\n') {
            sb_append_char(&sb, '\n');
        }
        filemap_unload(&part_map);
    }

    string expanded = sb_to_string(&sb);
    *out_source     = (NerdSource){
        .source      = expanded,
        .source_path = source.source_path,
    };

    array_free(part_paths);
    ast_done(&mod_ast);
    lex_done(&mod_lexer);
    filemap_unload(&mod_map);
    arena_done(&temp);
    return true;
}

internal bool program_expand_module_parts(ProgramInfo* program,
                                          NerdSource   source,
                                          const Lexer* lexer,
                                          const Ast*   ast,
                                          NerdSource*  out_source)
{
    if (!program_source_is_folder_module(source)) {
        *out_source = source;
        return true;
    }

    Arena temp = {0};
    arena_init(&temp);

    cstr source_path = module_source_file_path(&temp, source);
    if (source_path == NULL) {
        arena_done(&temp);
        *out_source = source;
        return true;
    }
    cstr module_dir = path_dirname(&temp, source_path);
    Array(cstr) part_paths =
        program_collect_implicit_part_paths(&temp, module_dir);

    if (array_count(part_paths) == 0) {
        array_free(part_paths);
        arena_done(&temp);
        *out_source = source;
        return true;
    }

    StringBuilder sb = {0};
    sb_init(&sb, &program->arena);
    sb_append_string(&sb, source.source);
    if (source.source.count == 0 ||
        source.source.data[source.source.count - 1] != '\n') {
        sb_append_char(&sb, '\n');
    }

    for (u32 i = 0; i < array_count(part_paths); ++i) {
        if (program_mod_explicitly_uses_child_path(lexer, ast, part_paths[i])) {
            continue;
        }

        FileMap part_map    = {0};
        string  part_source = filemap_load(part_paths[i], &part_map);
        if (part_source.data == NULL) {
            cstr missing = program_copy_cstr(&program->arena, part_paths[i]);
            array_free(part_paths);
            arena_done(&temp);
            return error_runtime("Failed to load module part: %s", missing);
        }

        sb_append_char(&sb, '\n');
        sb_append_string(&sb, part_source);
        if (part_source.count == 0 ||
            part_source.data[part_source.count - 1] != '\n') {
            sb_append_char(&sb, '\n');
        }
        filemap_unload(&part_map);
    }

    string expanded = sb_to_string(&sb);
    array_free(part_paths);
    arena_done(&temp);

    *out_source = (NerdSource){
        .source      = expanded,
        .source_path = source.source_path,
    };
    return true;
}

internal bool program_front_end_parse_module(ProgramInfo*           program,
                                             NerdSource             source,
                                             const FrontEndOptions* options,
                                             Timing*                timing,
                                             FrontEndState*         front_end)
{
    if (!program_front_end_parse_only(source, options, timing, front_end)) {
        return false;
    }

    NerdSource expanded_source = {0};
    if (!program_expand_module_parts(program,
                                     source,
                                     &front_end->lexer,
                                     &front_end->ast,
                                     &expanded_source)) {
        return false;
    }

    if (expanded_source.source.data == source.source.data &&
        expanded_source.source.count == source.source.count) {
        return true;
    }

    front_end_results_done(front_end);
    return program_front_end_parse_only(
        expanded_source, options, timing, front_end);
}

internal void program_collect_module_exports(ModuleInfo* module)
{
    const Ast*  ast  = &module->front_end.ast;
    const Sema* sema = &module->front_end.sema;

    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->bind_node_index == sema_no_decl()) {
            continue;
        }
        if (decl->bind_node_index >= array_count(ast->nodes)) {
            continue;
        }
        const AstNode* bind = &ast->nodes[decl->bind_node_index];
        if (!ast_has_flag(bind, ANF_Public)) {
            continue;
        }
        array_push(module->export_decl_indices, i);
    }

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Use || !ast_has_flag(node, ANF_Public)) {
            continue;
        }
        if (node->a >= array_count(sema->node_type_indices)) {
            continue;
        }

        u32 module_type = sema->node_type_indices[node->a];
        if (module_type == sema_no_type() ||
            sema->types[module_type].kind != STK_Module) {
            continue;
        }

        const SemaType* use_module          = &sema->types[module_type];
        u32             import_module_index = use_module->return_type;
        if (module->front_end.sema.program == NULL ||
            import_module_index >=
                array_count(module->front_end.sema.program->modules)) {
            continue;
        }

        const ModuleInfo* import_module =
            &module->front_end.sema.program->modules[import_module_index];
        for (u32 j = 0; j < use_module->param_count; ++j) {
            u32 symbol =
                sema->type_param_symbols[use_module->first_param_type + j];
            u32 import_decl_index =
                j < array_count(import_module->export_decl_indices)
                    ? import_module->export_decl_indices[j]
                    : sema_no_decl();

            for (u32 decl_index = 0; decl_index < array_count(sema->decls);
                 ++decl_index) {
                const SemaDecl* decl = &sema->decls[decl_index];
                if (decl->symbol_handle == symbol &&
                    decl->bind_node_index == sema_no_decl() &&
                    decl->import_module_index == import_module_index &&
                    decl->import_decl_index == import_decl_index) {
                    array_push(module->export_decl_indices, decl_index);
                    break;
                }
            }
        }
    }
}

internal bool
program_collect_module_dependencies(ProgramInfo*           program,
                                    const FrontEndOptions* options,
                                    Timing*                timing,
                                    u32                    owner_module_index,
                                    const Lexer*           lexer,
                                    const Ast*             ast,
                                    u32                    first_node,
                                    u32                    end_node);

internal bool program_load_module_by_path(ProgramInfo*           program,
                                          const FrontEndOptions* options,
                                          Timing*                timing,
                                          string                 qualified_name,
                                          cstr                   resolved_path)
{
    u32 existing = program_find_module_by_path(program, resolved_path);
    if (existing != U32_MAX) {
        if (program->modules[existing].state == MODULE_Loading) {
            return error_runtime(
                "Module import cycle detected while loading %s", resolved_path);
        }
        return program->modules[existing].state == MODULE_Loaded;
    }

    ModuleInfo module = {
        .qualified_name = program_copy_string(&program->arena, qualified_name),
        .resolved_path  = program_copy_cstr(&program->arena, resolved_path),
        .state          = MODULE_Loading,
    };
    array_push(program->modules, module);
    u32 module_index    = (u32)array_count(program->modules) - 1;

    ModuleInfo* current = &program->modules[module_index];
    string      source_text =
        filemap_load(current->resolved_path, &current->source_map);
    if (source_text.data == NULL) {
        current->state = MODULE_Failed;
        return error_runtime("Failed to load module source file: %s",
                             current->resolved_path);
    }

    NerdSource module_source = {
        .source      = source_text,
        .source_path = s(current->resolved_path),
    };
    FrontEndOptions module_options = options ? *options : (FrontEndOptions){0};
    module_options.require_entry_point = false;

    if (!program_front_end_parse_module(program,
                                        module_source,
                                        &module_options,
                                        timing,
                                        &current->front_end)) {
        current->state = MODULE_Failed;
        return false;
    }

    Lexer module_lexer = current->front_end.lexer;
    Ast   module_ast   = current->front_end.ast;
    if (!program_collect_module_dependencies(
            program,
            options,
            timing,
            module_index,
            &module_lexer,
            &module_ast,
            0,
            (u32)array_count(module_ast.nodes))) {
        program->modules[module_index].state = MODULE_Failed;
        return false;
    }

    if (!program_front_end_finish(
            program, module_index, options, timing, false)) {
        program->modules[module_index].state = MODULE_Failed;
        return false;
    }

    current = &program->modules[module_index];
    program_collect_module_exports(current);
    current->state = MODULE_Loaded;
    return true;
}

internal bool
program_collect_module_dependencies(ProgramInfo*           program,
                                    const FrontEndOptions* options,
                                    Timing*                timing,
                                    u32                    owner_module_index,
                                    const Lexer*           lexer,
                                    const Ast*             ast,
                                    u32                    first_node,
                                    u32                    end_node)
{
    for (u32 i = first_node; i < end_node; ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_TopOn) {
            const AstTopOnInfo* info = &ast->top_ons[node->a];
            const AstNode*      body = &ast->nodes[info->body_node_index];
            ASSERT(body->kind == AK_Block, "Expected top-level on body block");
            if (program_top_on_is_enabled(options, lexer, ast, node) &&
                !program_collect_module_dependencies(program,
                                                     options,
                                                     timing,
                                                     owner_module_index,
                                                     lexer,
                                                     ast,
                                                     body->a,
                                                     body->b)) {
                return false;
            }
            i = body->b - 1;
            continue;
        }

        if (node->kind != AK_ModRef) {
            continue;
        }

        const AstModulePath* path = &ast->module_paths[node->a];
        Arena                temp = {0};
        arena_init(&temp);
        ModuleResolveResult resolved = {0};
        ModuleResolveStatus status   = module_resolve_path(
            &temp, program->root_source, lexer, ast, path, &resolved);
        if (status != MRS_Found) {
            arena_done(&temp);
            continue;
        }

        string qualified_name = s(resolved.qualified_name);
        bool   ok             = program_load_module_by_path(
            program, options, timing, qualified_name, resolved.resolved_path);
        u32 loaded_index =
            program_find_module_by_path(program, resolved.resolved_path);
        arena_done(&temp);
        if (!ok) {
            return false;
        }
        if (loaded_index != U32_MAX) {
            ModuleInfo* owner = &program->modules[owner_module_index];
            bool        already_recorded = false;
            for (u32 j = 0; j < array_count(owner->imported_module_indices);
                 ++j) {
                if (owner->imported_module_indices[j] == loaded_index) {
                    already_recorded = true;
                    break;
                }
            }
            if (!already_recorded) {
                array_push(owner->imported_module_indices, loaded_index);
            }
        }
    }

    return true;
}

bool front_end_program(NerdSource             source,
                       const FrontEndOptions* options,
                       Timing*                timing,
                       ProgramInfo*           out_program)
{
    FrontEndOptions effective_options =
        options ? *options : (FrontEndOptions){0};
    if (options == NULL) {
        effective_options.require_entry_point = true;
    }

    ProgramInfo program = {
        .root_source       = source,
        .root_module_index = 0,
    };
    arena_init(&program.arena);

    NerdSource effective_source = {0};
    if (!program_expand_part_root(&program, source, &effective_source)) {
        program_info_done(&program);
        return false;
    }
    source                 = effective_source;
    program.root_source    = source;

    ModuleInfo root_module = {
        .qualified_name =
            source.source_path.count > 0
                ? program_copy_string(&program.arena,
                                      path_stem(source.source_path))
                : s("root"),
        .resolved_path = source.source_path.count > 0
                             ? program_copy_cstr_from_string(&program.arena,
                                                             source.source_path)
                             : "<memory>",
        .state         = MODULE_Loading,
    };
    array_push(program.modules, root_module);

    if (!program_front_end_parse_module(
            &program,
            source,
            &effective_options,
            timing,
            &program.modules[program.root_module_index].front_end)) {
        program.modules[program.root_module_index].state = MODULE_Failed;
        program_info_done(&program);
        return false;
    }

    Lexer root_lexer =
        program.modules[program.root_module_index].front_end.lexer;
    Ast root_ast = program.modules[program.root_module_index].front_end.ast;

    if (!program_collect_module_dependencies(
            &program,
            &effective_options,
            timing,
            program.root_module_index,
            &root_lexer,
            &root_ast,
            0,
            (u32)array_count(root_ast.nodes))) {
        program.modules[program.root_module_index].state = MODULE_Failed;
        program_info_done(&program);
        return false;
    }

    if (!program_front_end_finish(&program,
                                  program.root_module_index,
                                  &effective_options,
                                  timing,
                                  effective_options.require_entry_point)) {
        program.modules[program.root_module_index].state = MODULE_Failed;
        if (effective_options.keep_partial_results && out_program != NULL) {
            *out_program = program;
            program_rebind_sema_programs(out_program);
            return false;
        }
        program_info_done(&program);
        return false;
    }

    program_collect_module_exports(&program.modules[program.root_module_index]);
    program.modules[program.root_module_index].state = MODULE_Loaded;

    if (!program_front_end_generate_ir(&program, &effective_options, timing)) {
        program.modules[program.root_module_index].state = MODULE_Failed;
        program_info_done(&program);
        return false;
    }

    if (out_program != NULL) {
        *out_program = program;
        program_rebind_sema_programs(out_program);
    } else {
        program_info_done(&program);
    }
    return true;
}

void program_info_done(ProgramInfo* program)
{
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        ModuleInfo* module = &program->modules[i];
        array_free(module->imported_module_indices);
        array_free(module->export_decl_indices);
        front_end_results_done(&module->front_end);
        if (module->source_map.data != NULL) {
            filemap_unload(&module->source_map);
        }
    }
    array_free(program->modules);
    arena_done(&program->arena);
    *program = (ProgramInfo){0};
}

//------------------------------------------------------------------------------
