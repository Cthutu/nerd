//------------------------------------------------------------------------------
// Whole-program front-end loading
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/build/front/front.h>
#include <compiler/error/error.h>
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
}

internal bool
program_collect_module_dependencies(ProgramInfo*           program,
                                    const FrontEndOptions* options,
                                    Timing*                timing,
                                    const NerdSource*      root_source,
                                    const Lexer*           lexer,
                                    const Ast*             ast,
                                    u32                    first_node,
                                    u32                    end_node);

internal bool program_load_module_by_path(ProgramInfo*           program,
                                          const FrontEndOptions* options,
                                          Timing*                timing,
                                          const NerdSource*      root_source,
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
    FrontEndOptions module_options     = *options;
    module_options.require_entry_point = false;

    if (!front_end(
            module_source, &module_options, timing, &current->front_end)) {
        current->state = MODULE_Failed;
        return false;
    }

    program_collect_module_exports(current);
    if (!program_collect_module_dependencies(
            program,
            options,
            timing,
            root_source,
            &current->front_end.lexer,
            &current->front_end.ast,
            0,
            (u32)array_count(current->front_end.ast.nodes))) {
        current->state = MODULE_Failed;
        return false;
    }

    current->state = MODULE_Loaded;
    return true;
}

internal bool
program_collect_module_dependencies(ProgramInfo*           program,
                                    const FrontEndOptions* options,
                                    Timing*                timing,
                                    const NerdSource*      root_source,
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
                                                     root_source,
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
            &temp, *root_source, lexer, ast, path, &resolved);
        if (status != MRS_Found) {
            arena_done(&temp);
            continue;
        }

        string qualified_name = s(resolved.qualified_name);
        bool   ok             = program_load_module_by_path(program,
                                              options,
                                              timing,
                                              root_source,
                                              qualified_name,
                                              resolved.resolved_path);
        arena_done(&temp);
        if (!ok) {
            return false;
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

    if (!front_end(source,
                   &effective_options,
                   timing,
                   &program.modules[program.root_module_index].front_end)) {
        program.modules[program.root_module_index].state = MODULE_Failed;
        program_info_done(&program);
        return false;
    }

    program_collect_module_exports(&program.modules[program.root_module_index]);
    if (!program_collect_module_dependencies(
            &program,
            &effective_options,
            timing,
            &program.root_source,
            &program.modules[program.root_module_index].front_end.lexer,
            &program.modules[program.root_module_index].front_end.ast,
            0,
            (u32)array_count(program.modules[program.root_module_index]
                                 .front_end.ast.nodes))) {
        program.modules[program.root_module_index].state = MODULE_Failed;
        program_info_done(&program);
        return false;
    }

    program.modules[program.root_module_index].state = MODULE_Loaded;

    if (out_program != NULL) {
        *out_program = program;
    } else {
        program_info_done(&program);
    }
    return true;
}

void program_info_done(ProgramInfo* program)
{
    for (u32 i = 0; i < array_count(program->modules); ++i) {
        ModuleInfo* module = &program->modules[i];
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
