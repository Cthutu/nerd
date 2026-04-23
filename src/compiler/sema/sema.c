//------------------------------------------------------------------------------
// Semantic analysis implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------
// Return sentinel semantic indices used for unresolved entries.

u32 sema_no_decl(void) { return U32_MAX; }

u32 sema_no_local(void) { return U32_MAX; }

internal u32 sema_no_scope(void) { return U32_MAX; }

//------------------------------------------------------------------------------
// Return the sentinel type index used for unresolved entries.

u32 sema_no_type(void) { return U32_MAX; }

//------------------------------------------------------------------------------
// Predeclare the current built-in runtime functions.

internal u32 sema_type_index_for_name(Sema* sema, string name);

internal u32 sema_find_symbol_handle_by_name(const Lexer* lexer, string name)
{
    for (u32 i = 0; i < array_count(lexer->symbol_handles); ++i) {
        u32 handle = lexer->symbol_handles[i];
        if (string_eq(lex_symbol(lexer, handle), name)) {
            return handle;
        }
    }
    return sema_no_decl();
}

//------------------------------------------------------------------------------
// Add one semantic type row and return its canonical index.

internal u32 sema_add_type(Sema* sema, SemaType type)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (memcmp(&sema->types[i], &type, sizeof(type)) == 0) {
            return i;
        }
    }

    u32 index = (u32)array_count(sema->types);
    array_push(sema->types, type);
    return index;
}

internal u32 sema_add_function_type(Sema* sema,
                                    Array(u32) param_types,
                                    u32 return_type)
{
    for (u32 i = 0; i < array_count(sema->types); ++i) {
        const SemaType* existing = &sema->types[i];
        if (existing->kind != STK_Function ||
            existing->param_count != array_count(param_types) ||
            existing->return_type != return_type) {
            continue;
        }

        bool matches = true;
        for (u32 j = 0; j < existing->param_count; ++j) {
            if (sema->type_param_types[existing->first_param_type + j] !=
                param_types[j]) {
                matches = false;
                break;
            }
        }
        if (matches) {
            return i;
        }
    }

    u32 first_param = (u32)array_count(sema->type_param_types);
    for (u32 i = 0; i < array_count(param_types); ++i) {
        array_push(sema->type_param_types, param_types[i]);
    }

    return sema_add_type(sema,
                         (SemaType){
                             .kind             = STK_Function,
                             .param_count      = (u16)array_count(param_types),
                             .first_param_type = first_param,
                             .return_type      = return_type,
                         });
}

//------------------------------------------------------------------------------
// Return the canonical type index for one built-in type.

internal u32 sema_builtin_type(Sema* sema, SemaTypeKind kind)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind             = kind,
                             .param_count      = 0,
                             .first_param_type = 0,
                             .return_type      = sema_no_type(),
                         });
}

//------------------------------------------------------------------------------
// Return whether one type kind is integer-like.

internal bool sema_type_kind_is_integer(SemaTypeKind kind)
{
    switch (kind) {
    case STK_UntypedInteger:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_Isize:
    case STK_Usize:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Return whether one semantic type is integer-like.

bool sema_type_is_integer(const Sema* sema, u32 type_index)
{
    return type_index != sema_no_type() &&
           sema_type_kind_is_integer(sema->types[type_index].kind);
}

//------------------------------------------------------------------------------
// Return whether one semantic type is a concrete integer type.

bool sema_type_is_concrete_integer(const Sema* sema, u32 type_index)
{
    return sema_type_is_integer(sema, type_index) &&
           sema->types[type_index].kind != STK_UntypedInteger;
}

//------------------------------------------------------------------------------
// Materialise abstract semantic types for runtime-facing uses.

u32 sema_materialise_type(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return type_index;
    }

    if (sema->types[type_index].kind != STK_UntypedInteger) {
        return type_index;
    }

    for (u32 i = 0; i < array_count(sema->types); ++i) {
        if (sema->types[i].kind == STK_I32) {
            return i;
        }
    }

    return type_index;
}

//------------------------------------------------------------------------------
// Render one semantic type as source-facing text.

string sema_type_name(const Sema* sema, Arena* arena, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return s("<unknown>");
    }

    const SemaType* type = &sema->types[type_index];
    switch (type->kind) {
    case STK_Void:
        return s("void");
    case STK_UntypedInteger:
        return s("untyped integer");
    case STK_String:
        return s("string");
    case STK_Bool:
        return s("bool");
    case STK_I8:
        return s("i8");
    case STK_I16:
        return s("i16");
    case STK_I32:
        return s("i32");
    case STK_I64:
        return s("i64");
    case STK_U8:
        return s("u8");
    case STK_U16:
        return s("u16");
    case STK_U32:
        return s("u32");
    case STK_U64:
        return s("u64");
    case STK_F32:
        return s("f32");
    case STK_F64:
        return s("f64");
    case STK_Isize:
        return s("isize");
    case STK_Usize:
        return s("usize");
    case STK_Function:
        {
            StringBuilder sb = {0};
            sb_init(&sb, arena);
            sb_append_cstr(&sb, "fn (");
            for (u32 i = 0; i < type->param_count; ++i) {
                if (i > 0) {
                    sb_append_cstr(&sb, ", ");
                }
                sb_append_string(
                    &sb,
                    sema_type_name(
                        sema,
                        arena,
                        sema->type_param_types[type->first_param_type + i]));
            }
            sb_append_cstr(&sb, ") -> ");
            sb_append_string(&sb,
                             sema_type_name(sema, arena, type->return_type));
            return sb_to_string(&sb);
        }
    default:
        return s("<unknown>");
    }
}

internal const AstFnSignature*
sema_ast_signature(const Ast* ast, const AstNode* signature_owner)
{
    return &ast->fn_signatures[signature_owner->a];
}

//------------------------------------------------------------------------------
// Predeclare the current built-in runtime functions.

internal void sema_add_builtin_decls(const Lexer* lexer, Sema* sema)
{
    u32 pr_symbol     = sema_find_symbol_handle_by_name(lexer, s("pr"));
    u32 prn_symbol    = sema_find_symbol_handle_by_name(lexer, s("prn"));
    u32 string_type   = sema_builtin_type(sema, STK_String);
    u32 void_type     = sema_builtin_type(sema, STK_Void);
    Array(u32) params = NULL;
    array_push(params, string_type);
    u32 print_type = sema_add_function_type(sema, params, void_type);
    array_free(params);

    if (pr_symbol != sema_no_decl()) {
        array_push(sema->decls,
                   (SemaDecl){
                       .kind             = SK_BuiltinFunction,
                       .symbol_handle    = pr_symbol,
                       .bind_node_index  = sema_no_decl(),
                       .type_node_index  = sema_no_type(),
                       .value_node_index = sema_no_decl(),
                       .type_index       = print_type,
                   });
    }

    if (prn_symbol != sema_no_decl()) {
        array_push(sema->decls,
                   (SemaDecl){
                       .kind             = SK_BuiltinFunction,
                       .symbol_handle    = prn_symbol,
                       .bind_node_index  = sema_no_decl(),
                       .type_node_index  = sema_no_type(),
                       .value_node_index = sema_no_decl(),
                       .type_index       = print_type,
                   });
    }
}

//------------------------------------------------------------------------------
// Return whether one node already has a known folded constant result.

internal bool sema_try_get_constant(const Ast*  ast,
                                    const Sema* sema,
                                    u32         node_index,
                                    i64*        out)
{
    if (node_index >= array_count(sema->node_const_known) ||
        !sema->node_const_known[node_index] ||
        !ast_has_flag(&ast->nodes[node_index], ANF_ConstKnown)) {
        return false;
    }

    *out = sema->node_const_values[node_index];
    return true;
}

//------------------------------------------------------------------------------
// Store one folded constant result for an AST node.

internal void sema_set_constant(Ast* ast, Sema* sema, u32 node_index, i64 value)
{
    sema->node_const_known[node_index]  = true;
    sema->node_const_values[node_index] = value;
    ast_set_flag(&ast->nodes[node_index], ANF_ConstKnown);
}

//------------------------------------------------------------------------------
// Compute the source span covered by an AST node's main token.

internal ErrorSpan sema_node_span(const Lexer* lexer, const AstNode* node)
{
    const Token* token = &lexer->tokens[node->token_index];
    return (ErrorSpan){.start = token->offset,
                       .end   = lex_token_end_offset(lexer, token)};
}

//------------------------------------------------------------------------------
// Look up the source span for a collected declaration binding.

internal ErrorSpan sema_decl_span(const Lexer*    lexer,
                                  const Ast*      ast,
                                  const SemaDecl* decl)
{
    if (decl->bind_node_index == sema_no_decl()) {
        return (ErrorSpan){0};
    }
    return sema_node_span(lexer, &ast->nodes[decl->bind_node_index]);
}

internal ErrorSpan sema_local_span(const Lexer*     lexer,
                                   const Ast*       ast,
                                   const SemaLocal* local)
{
    if (local->decl_node_index == sema_no_decl()) {
        return sema_node_span(lexer, &ast->nodes[local->type_node_index]);
    }
    return sema_node_span(lexer, &ast->nodes[local->decl_node_index]);
}

internal ErrorSpan sema_token_span(const Lexer* lexer, u32 token_index)
{
    const Token* token = &lexer->tokens[token_index];
    return (ErrorSpan){.start = token->offset,
                       .end   = lex_token_end_offset(lexer, token)};
}

//------------------------------------------------------------------------------
// Find a top-level declaration by its bound symbol handle.

internal u32 sema_find_decl(const Sema* sema, u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].symbol_handle == symbol_handle) {
            return i;
        }
    }
    return sema_no_decl();
}

typedef enum : u8 {
    SEMA_ALIAS_UNSEEN,
    SEMA_ALIAS_RESOLVING,
    SEMA_ALIAS_DONE,
} SemaAliasState;

internal bool sema_try_classify_type_alias(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          owner_decl_index,
                                           u32          decl_index,
                                           Array(u8) alias_states,
                                           bool* out_is_type,
                                           u32*  out_type_index);

//------------------------------------------------------------------------------
// Record a dependency edge if it is not already present.

internal void sema_add_dep(Sema* sema, u32 from_decl_index, u32 to_decl_index)
{
    for (u32 i = 0; i < array_count(sema->deps); ++i) {
        const SemaDeclDep* dep = &sema->deps[i];
        if (dep->from_decl_index == from_decl_index &&
            dep->to_decl_index == to_decl_index) {
            return;
        }
    }

    array_push(sema->deps,
               (SemaDeclDep){
                   .from_decl_index = from_decl_index,
                   .to_decl_index   = to_decl_index,
               });
}

//------------------------------------------------------------------------------
// Collect top-level bindings into a compact declaration table.

internal bool sema_node_is_inside_function_body(const Ast* ast, u32 node_index)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_FnDef) {
            continue;
        }

        const AstNode* fn_start = &ast->nodes[node->a];
        if (node_index > node->a && node_index < fn_start->b) {
            return true;
        }
    }

    return false;
}

internal bool sema_try_classify_type_node(const Lexer* lexer,
                                          const Ast*   ast,
                                          Sema*        sema,
                                          u32          owner_decl_index,
                                          u32          node_index,
                                          Array(u8) alias_states,
                                          bool* out_is_type,
                                          u32*  out_type_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 builtin =
                sema_type_index_for_name(sema, lex_symbol(lexer, node->a));
            if (builtin != sema_no_type()) {
                *out_is_type    = true;
                *out_type_index = builtin;
                return true;
            }

            u32 decl_index = sema_find_decl(sema, node->a);
            if (decl_index == sema_no_decl()) {
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }

            return sema_try_classify_type_alias(lexer,
                                                ast,
                                                sema,
                                                owner_decl_index,
                                                decl_index,
                                                alias_states,
                                                out_is_type,
                                                out_type_index);
        }

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            Array(u32) param_types          = NULL;

            for (u32 i = 0; i < signature->param_count; ++i) {
                bool param_is_type = false;
                u32  param_type    = sema_no_type();
                if (!sema_try_classify_type_node(
                        lexer,
                        ast,
                        sema,
                        owner_decl_index,
                        ast->params[signature->first_param + i].type_node_index,
                        alias_states,
                        &param_is_type,
                        &param_type)) {
                    array_free(param_types);
                    return false;
                }
                if (!param_is_type) {
                    array_free(param_types);
                    *out_is_type    = false;
                    *out_type_index = sema_no_type();
                    return true;
                }
                array_push(param_types, param_type);
            }

            bool return_is_type = false;
            u32  return_type    = sema_no_type();
            if (!sema_try_classify_type_node(lexer,
                                             ast,
                                             sema,
                                             owner_decl_index,
                                             signature->return_type_node_index,
                                             alias_states,
                                             &return_is_type,
                                             &return_type)) {
                array_free(param_types);
                return false;
            }
            if (!return_is_type) {
                array_free(param_types);
                *out_is_type    = false;
                *out_type_index = sema_no_type();
                return true;
            }

            *out_is_type = true;
            *out_type_index =
                sema_add_function_type(sema, param_types, return_type);
            array_free(param_types);
            return true;
        }

    default:
        *out_is_type    = false;
        *out_type_index = sema_no_type();
        return true;
    }
}

internal bool sema_try_classify_type_alias(const Lexer* lexer,
                                           const Ast*   ast,
                                           Sema*        sema,
                                           u32          owner_decl_index,
                                           u32          decl_index,
                                           Array(u8) alias_states,
                                           bool* out_is_type,
                                           u32*  out_type_index)
{
    SemaDecl* decl = &sema->decls[decl_index];
    if (decl->kind == SK_TypeAlias) {
        *out_is_type    = true;
        *out_type_index = decl->type_index;
        return true;
    }
    if (decl->kind == SK_Variable || decl->kind == SK_Function ||
        decl->kind == SK_BuiltinFunction) {
        *out_is_type    = false;
        *out_type_index = sema_no_type();
        return true;
    }
    if (decl->type_node_index != sema_no_type() ||
        decl->value_node_index == sema_no_decl()) {
        *out_is_type    = false;
        *out_type_index = sema_no_type();
        return true;
    }

    switch ((SemaAliasState)alias_states[decl_index]) {
    case SEMA_ALIAS_DONE:
        *out_is_type    = decl->kind == SK_TypeAlias;
        *out_type_index = decl->type_index;
        return true;
    case SEMA_ALIAS_RESOLVING:
        return error_0309_type_alias_cycle(
            lexer->source,
            sema_decl_span(lexer, ast, &sema->decls[owner_decl_index]),
            lex_symbol(lexer, sema->decls[owner_decl_index].symbol_handle),
            sema_decl_span(lexer, ast, decl),
            lex_symbol(lexer, decl->symbol_handle));
    case SEMA_ALIAS_UNSEEN:
        break;
    }

    alias_states[decl_index] = SEMA_ALIAS_RESOLVING;
    bool rhs_is_type         = false;
    u32  rhs_type            = sema_no_type();
    if (!sema_try_classify_type_node(lexer,
                                     ast,
                                     sema,
                                     decl_index,
                                     decl->value_node_index,
                                     alias_states,
                                     &rhs_is_type,
                                     &rhs_type)) {
        return false;
    }

    if (rhs_is_type) {
        decl->kind       = SK_TypeAlias;
        decl->type_index = rhs_type;
    }

    alias_states[decl_index] = SEMA_ALIAS_DONE;
    *out_is_type             = rhs_is_type;
    *out_type_index          = rhs_type;
    return true;
}

internal bool sema_collect_decls(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind != AK_Bind && node->kind != AK_Variable) {
            continue;
        }
        if (sema_node_is_inside_function_body(ast, i)) {
            continue;
        }

        u32 existing_decl_index = sema_find_decl(sema, ast_get_symbol(node));
        if (existing_decl_index != sema_no_decl()) {
            const SemaDecl* existing_decl = &sema->decls[existing_decl_index];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, node),
                lex_symbol(lexer, ast_get_symbol(node)),
                existing_decl->bind_node_index == sema_no_decl()
                    ? sema_node_span(lexer, node)
                    : sema_node_span(
                          lexer, &ast->nodes[existing_decl->bind_node_index]));
        }

        u32            type_node_index  = sema_no_type();
        u32            value_node_index = node->b;
        const AstNode* value            = &ast->nodes[value_node_index];
        if (value->kind == AK_AnnotatedValue) {
            type_node_index  = value->a;
            value_node_index = value->b;
            value            = &ast->nodes[value_node_index];
        } else if (value->kind == AK_ZeroInit) {
            type_node_index  = value->a;
            value_node_index = sema_no_decl();
        }

        SemaDeclKind kind = SK_Constant;
        if (node->kind == AK_Variable) {
            kind = SK_Variable;
        } else if (value->kind == AK_FnDef) {
            kind = SK_Function;
        }

        array_push(sema->decls,
                   (SemaDecl){
                       .kind             = kind,
                       .symbol_handle    = ast_get_symbol(node),
                       .bind_node_index  = i,
                       .type_node_index  = type_node_index,
                       .value_node_index = value_node_index,
                       .type_index       = sema_no_type(),
                   });
    }

    return true;
}

internal bool
sema_classify_type_aliases(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    Array(u8) alias_states = NULL;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        array_push(alias_states, SEMA_ALIAS_UNSEEN);
    }

    bool ok = true;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        bool ignored_is_type = false;
        u32  ignored_type    = sema_no_type();
        if (!sema_try_classify_type_alias(lexer,
                                          ast,
                                          sema,
                                          i,
                                          i,
                                          alias_states,
                                          &ignored_is_type,
                                          &ignored_type)) {
            ok = false;
            break;
        }
    }

    array_free(alias_states);
    return ok;
}

//------------------------------------------------------------------------------
// Mark one type-annotation subtree so normal symbol resolution can skip it.

internal void
sema_mark_type_expr_nodes(const Ast* ast, Sema* sema, u32 node_index)
{
    if (node_index >= array_count(sema->node_is_type_expr) ||
        sema->node_is_type_expr[node_index]) {
        return;
    }

    sema->node_is_type_expr[node_index] = true;
    const AstNode* node                 = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_TypeFn:
        {
            const AstFnSignature* signature = &ast->fn_signatures[node->a];
            for (u32 i = 0; i < signature->param_count; ++i) {
                sema_mark_type_expr_nodes(
                    ast,
                    sema,
                    ast->params[signature->first_param + i].type_node_index);
            }
            if (signature->return_type_node_index != U32_MAX) {
                sema_mark_type_expr_nodes(
                    ast, sema, signature->return_type_node_index);
            }
        }
        break;
    default:
        break;
    }
}

internal u32 sema_add_scope(Sema* sema, u32 owner_decl_index, u32 parent_scope)
{
    array_push(sema->scopes,
               (SemaScope){
                   .owner_decl_index   = owner_decl_index,
                   .parent_scope_index = parent_scope,
                   .first_local        = (u32)array_count(sema->locals),
                   .local_count        = 0,
               });
    return (u32)array_count(sema->scopes) - 1;
}

internal u32 sema_find_local_in_scope(const Sema* sema,
                                      u32         scope_index,
                                      u32         symbol_handle)
{
    if (scope_index == sema_no_scope()) {
        return sema_no_local();
    }

    const SemaScope* scope = &sema->scopes[scope_index];
    u32              end   = scope->first_local + scope->local_count;
    for (u32 i = end; i-- > scope->first_local;) {
        const SemaLocal* local = &sema->locals[i];
        if (local->symbol_handle == symbol_handle) {
            return i;
        }
    }
    return sema_no_local();
}

internal u32 sema_lookup_local(const Sema* sema,
                               u32         scope_index,
                               u32         symbol_handle)
{
    for (u32 current = scope_index; current != sema_no_scope();
         current     = sema->scopes[current].parent_scope_index) {
        u32 local_index =
            sema_find_local_in_scope(sema, current, symbol_handle);
        if (local_index != sema_no_local()) {
            return local_index;
        }
    }
    return sema_no_local();
}

internal u32 sema_find_decl_local_in_scope(const Sema* sema,
                                           u32         scope_index,
                                           u32         symbol_handle)
{
    if (scope_index == sema_no_scope()) {
        return sema_no_local();
    }

    const SemaScope* scope = &sema->scopes[scope_index];
    u32              end   = scope->first_local + scope->local_count;
    for (u32 i = end; i-- > scope->first_local;) {
        const SemaLocal* local = &sema->locals[i];
        if ((local->kind == SLK_Constant || local->kind == SLK_Function ||
             local->kind == SLK_TypeAlias) &&
            local->symbol_handle == symbol_handle) {
            return i;
        }
    }

    return sema_no_local();
}

internal u32 sema_lookup_decl_local(const Sema* sema,
                                    u32         scope_index,
                                    u32         symbol_handle)
{
    for (u32 current = scope_index; current != sema_no_scope();
         current     = sema->scopes[current].parent_scope_index) {
        u32 local_index =
            sema_find_decl_local_in_scope(sema, current, symbol_handle);
        if (local_index != sema_no_local()) {
            return local_index;
        }
    }
    return sema_no_local();
}

internal bool sema_local_is_runtime_value(const SemaLocal* local)
{
    return local->kind == SLK_Param || local->kind == SLK_Variable;
}

internal bool sema_local_is_decl_binding(const SemaLocal* local)
{
    return local->kind == SLK_Constant || local->kind == SLK_Function ||
           local->kind == SLK_TypeAlias;
}

internal u32 sema_mangle_child_function_symbol(const Lexer* lexer,
                                               u32    parent_symbol_handle,
                                               string child_name)
{
    string owner   = lex_symbol(lexer, parent_symbol_handle);
    string mangled = string_format(
        &temp_arena, STRINGP "$" STRINGP, STRINGV(owner), STRINGV(child_name));
    InternAddResult ignored = 0;
    return lex_add_symbol((Lexer*)lexer, mangled, &ignored);
}

internal u32 sema_mangle_nested_function_symbol(const Lexer* lexer,
                                                u32 parent_symbol_handle,
                                                u32 symbol_handle)
{
    return sema_mangle_child_function_symbol(
        lexer, parent_symbol_handle, lex_symbol(lexer, symbol_handle));
}

internal u32 sema_mangle_anonymous_function_symbol(const Lexer* lexer,
                                                   u32 parent_symbol_handle,
                                                   u32 fn_node_index)
{
    string suffix = string_format(&temp_arena, "anon%u", fn_node_index);
    return sema_mangle_child_function_symbol(
        lexer, parent_symbol_handle, suffix);
}

internal void
sema_mark_fn_signature_type_nodes(const Ast* ast, Sema* sema, u32 fn_node_index)
{
    const AstNode*        fn_def    = &ast->nodes[fn_node_index];
    const AstNode*        fn_start  = &ast->nodes[fn_def->a];
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
    for (u32 i = 0; i < signature->param_count; ++i) {
        sema_mark_type_expr_nodes(
            ast, sema, ast->params[signature->first_param + i].type_node_index);
    }
    if (signature->return_type_node_index != sema_no_type()) {
        sema_mark_type_expr_nodes(ast, sema, signature->return_type_node_index);
    }
}

internal bool sema_resolve_node_refs(const Lexer* lexer,
                                     const Ast*   ast,
                                     u32          owner_decl_index,
                                     u32          current_function_symbol,
                                     u32          capture_scope_index,
                                     u32          scope_index,
                                     u32          node_index,
                                     Sema*        sema);
internal bool sema_resolve_type_node(const Lexer* lexer,
                                     const Ast*   ast,
                                     Sema*        sema,
                                     u32          node_index,
                                     u32*         out_type_index);

internal bool sema_collect_block_statements(const Lexer* lexer,
                                            const Ast*   ast,
                                            u32          owner_decl_index,
                                            u32   current_function_symbol,
                                            u32   scope_index,
                                            u32   first_node,
                                            u32   end_node,
                                            Sema* sema)
{
    for (u32 i = first_node; i < end_node;) {
        if (sema->node_is_type_expr[i]) {
            i++;
            continue;
        }

        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_FnStart) {
            i = node->b + 2;
            continue;
        }
        if (node->kind == AK_Block) {
            i = node->b;
            continue;
        }
        if (node->kind != AK_Bind) {
            i++;
            continue;
        }

        u32            value_node_index = node->b;
        u32            type_node_index  = sema_no_type();
        const AstNode* value            = &ast->nodes[value_node_index];
        if (value->kind == AK_AnnotatedValue) {
            type_node_index  = value->a;
            value_node_index = value->b;
            value            = &ast->nodes[value_node_index];
        }

        u32 duplicate_local =
            sema_find_local_in_scope(sema, scope_index, node->a);
        if (duplicate_local != sema_no_local()) {
            const SemaLocal* previous = &sema->locals[duplicate_local];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_node_span(lexer, node),
                lex_symbol(lexer, node->a),
                sema_local_span(lexer, ast, previous));
        }

        SemaLocalKind kind = ast->nodes[value_node_index].kind == AK_FnDef
                                 ? SLK_Function
                                 : SLK_Constant;
        u32           lowered_symbol_handle =
            kind == SLK_Function ? sema_mangle_nested_function_symbol(
                                       lexer, current_function_symbol, node->a)
                                           : node->a;

        if (type_node_index != sema_no_type()) {
            sema_mark_type_expr_nodes(ast, sema, type_node_index);
        }

        array_push(sema->locals,
                   (SemaLocal){
                       .kind                  = kind,
                       .symbol_handle         = node->a,
                       .owner_decl_index      = owner_decl_index,
                       .scope_index           = scope_index,
                       .decl_node_index       = i,
                       .type_node_index       = type_node_index,
                       .value_node_index      = value_node_index,
                       .type_index            = sema_no_type(),
                       .lowered_symbol_handle = lowered_symbol_handle,
                   });
        sema->node_local_indices[i] = (u32)array_count(sema->locals) - 1;
        if (kind == SLK_Function) {
            sema->node_lowered_symbol_handles[value_node_index] =
                lowered_symbol_handle;
            sema_mark_fn_signature_type_nodes(ast, sema, value_node_index);
        }
        sema->scopes[scope_index].local_count++;
        i++;
    }

    for (u32 i = first_node; i < end_node;) {
        if (sema->node_is_type_expr[i]) {
            i++;
            continue;
        }
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Block) {
            u32 child_scope =
                sema_add_scope(sema, owner_decl_index, scope_index);
            if (!sema_collect_block_statements(lexer,
                                               ast,
                                               owner_decl_index,
                                               current_function_symbol,
                                               child_scope,
                                               node->a,
                                               node->b,
                                               sema)) {
                return false;
            }
            i = node->b;
            continue;
        }

        if (node->kind == AK_Bind) {
            u32 local_index = sema->node_local_indices[i];
            ASSERT(local_index != sema_no_local(), "Expected local bind");
            SemaLocal* local = &sema->locals[local_index];

            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        scope_index,
                                        scope_index,
                                        local->value_node_index,
                                        sema)) {
                return false;
            }
            i++;
            continue;
        }

        if (node->kind == AK_Variable) {
            u32 duplicate_index =
                sema_find_local_in_scope(sema, scope_index, node->a);
            if (duplicate_index != sema_no_local()) {
                const SemaLocal* previous = &sema->locals[duplicate_index];
                return error_0301_duplicate_binding(
                    lexer->source,
                    sema_node_span(lexer, node),
                    lex_symbol(lexer, node->a),
                    sema_local_span(lexer, ast, previous));
            }

            u32            type_node_index  = sema_no_type();
            u32            value_node_index = node->b;
            const AstNode* payload          = &ast->nodes[value_node_index];
            if (payload->kind == AK_AnnotatedValue) {
                type_node_index  = payload->a;
                value_node_index = payload->b;
            } else if (payload->kind == AK_ZeroInit) {
                type_node_index  = payload->a;
                value_node_index = sema_no_decl();
            }

            if (type_node_index != sema_no_type()) {
                sema_mark_type_expr_nodes(ast, sema, type_node_index);
            }
            if (value_node_index != sema_no_decl() &&
                !sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        value_node_index,
                                        sema)) {
                return false;
            }

            array_push(sema->locals,
                       (SemaLocal){
                           .kind                  = SLK_Variable,
                           .symbol_handle         = node->a,
                           .owner_decl_index      = owner_decl_index,
                           .scope_index           = scope_index,
                           .decl_node_index       = i,
                           .type_node_index       = type_node_index,
                           .value_node_index      = value_node_index,
                           .type_index            = sema_no_type(),
                           .lowered_symbol_handle = node->a,
                       });
            sema->node_local_indices[i] = (u32)array_count(sema->locals) - 1;
            sema->scopes[scope_index].local_count++;
            i++;
            continue;
        }

        if (node->kind == AK_Assign) {
            u32 local_index = sema_lookup_local(sema, scope_index, node->a);
            if (local_index != sema_no_local()) {
                if (!sema_local_is_runtime_value(&sema->locals[local_index])) {
                    return error_0305_invalid_assignment_target(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                sema->node_local_indices[i] = local_index;
            } else {
                u32 decl_index = sema_find_decl(sema, node->a);
                if (decl_index == sema_no_decl()) {
                    return error_0300_unknown_symbol(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                sema->node_decl_indices[i] = decl_index;
            }
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        sema_no_scope(),
                                        scope_index,
                                        node->b,
                                        sema)) {
                return false;
            }
            i++;
            continue;
        }

        if (node->kind != AK_Return && node->kind != AK_Statement) {
            i++;
            continue;
        }

        if (!sema_resolve_node_refs(lexer,
                                    ast,
                                    owner_decl_index,
                                    current_function_symbol,
                                    sema_no_scope(),
                                    scope_index,
                                    i,
                                    sema)) {
            return false;
        }
        i++;
    }

    return true;
}

internal bool sema_collect_function_locals(const Lexer* lexer,
                                           const Ast*   ast,
                                           u32          owner_decl_index,
                                           u32          current_function_symbol,
                                           u32          capture_scope_index,
                                           u32          fn_node_index,
                                           Sema*        sema)
{
    const AstNode* fn_def = &ast->nodes[fn_node_index];
    if (fn_def->kind != AK_FnDef) {
        return true;
    }

    const AstNode* fn_start = &ast->nodes[fn_def->a];
    u32 scope_index = sema_add_scope(sema, owner_decl_index, sema_no_scope());
    sema->node_scope_indices[fn_node_index] = scope_index;
    if (sema->node_lowered_symbol_handles[fn_node_index] == U32_MAX) {
        sema->node_lowered_symbol_handles[fn_node_index] =
            current_function_symbol;
    }
    const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];

    for (u32 i = 0; i < signature->param_count; ++i) {
        const AstParam* param = &ast->params[signature->first_param + i];
        u32             duplicate_index =
            sema_find_local_in_scope(sema, scope_index, param->symbol_handle);
        if (duplicate_index != sema_no_local()) {
            const SemaLocal* previous = &sema->locals[duplicate_index];
            return error_0301_duplicate_binding(
                lexer->source,
                sema_token_span(lexer, param->token_index),
                lex_symbol(lexer, param->symbol_handle),
                sema_local_span(lexer, ast, previous));
        }

        sema_mark_type_expr_nodes(ast, sema, param->type_node_index);
        u32 param_type = sema_no_type();
        if (!sema_resolve_type_node(
                lexer, ast, sema, param->type_node_index, &param_type)) {
            return false;
        }
        array_push(sema->locals,
                   (SemaLocal){
                       .kind                  = SLK_Param,
                       .symbol_handle         = param->symbol_handle,
                       .owner_decl_index      = owner_decl_index,
                       .scope_index           = scope_index,
                       .decl_node_index       = sema_no_decl(),
                       .type_node_index       = param->type_node_index,
                       .value_node_index      = sema_no_decl(),
                       .type_index            = param_type,
                       .lowered_symbol_handle = param->symbol_handle,
                   });
        sema->scopes[scope_index].local_count++;
    }

    if (fn_def->b != AFK_Block) {
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      ast->nodes[fn_def->a].b - 1,
                                      sema);
    }

    return sema_collect_block_statements(lexer,
                                         ast,
                                         owner_decl_index,
                                         current_function_symbol,
                                         scope_index,
                                         fn_def->a + 1,
                                         fn_start->b,
                                         sema);
}

internal bool sema_resolve_node_refs(const Lexer* lexer,
                                     const Ast*   ast,
                                     u32          owner_decl_index,
                                     u32          current_function_symbol,
                                     u32          capture_scope_index,
                                     u32          scope_index,
                                     u32          node_index,
                                     Sema*        sema)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_StringLiteral:
    case AK_ZeroInit:
    case AK_Block:
        return true;
    case AK_InterpPartExpr:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema);
    case AK_InterpolatedString:
        if (owner_decl_index == sema_no_decl() ||
            sema->decls[owner_decl_index].kind != SK_Function) {
            return error_0310_invalid_interpolation_context(
                lexer->source, sema_node_span(lexer, node));
        }
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        i,
                                        sema)) {
                return false;
            }
        }
        return true;
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            return sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          node->a,
                                          sema) &&
                   sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          on->true_expr_node_index,
                                          sema) &&
                   sema_resolve_node_refs(lexer,
                                          ast,
                                          owner_decl_index,
                                          current_function_symbol,
                                          capture_scope_index,
                                          scope_index,
                                          on->false_expr_node_index,
                                          sema);
        }
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema) &&
               sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->b,
                                      sema);
    case AK_Call:
        {
            if (!sema_resolve_node_refs(lexer,
                                        ast,
                                        owner_decl_index,
                                        current_function_symbol,
                                        capture_scope_index,
                                        scope_index,
                                        node->a,
                                        sema)) {
                return false;
            }
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (!sema_resolve_node_refs(lexer,
                                            ast,
                                            owner_decl_index,
                                            current_function_symbol,
                                            capture_scope_index,
                                            scope_index,
                                            ast->call_args[call->first_arg + i],
                                            sema)) {
                    return false;
                }
            }
            return true;
        }
    case AK_IntegerNegate:
    case AK_Cast:
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
        return sema_resolve_node_refs(lexer,
                                      ast,
                                      owner_decl_index,
                                      current_function_symbol,
                                      capture_scope_index,
                                      scope_index,
                                      node->a,
                                      sema);
    case AK_SymbolRef:
        if (sema->node_is_type_expr[node_index]) {
            return true;
        }
        {
            u32 local_index = sema_lookup_local(sema, scope_index, node->a);
            if (local_index != sema_no_local()) {
                sema->node_local_indices[node_index] = local_index;
                return true;
            }

            local_index = sema_lookup_decl_local(sema, scope_index, node->a);
            if (local_index != sema_no_local()) {
                sema->node_local_indices[node_index] = local_index;
                return true;
            }

            u32 decl_index = sema_find_decl(sema, node->a);
            if (decl_index == sema_no_decl()) {
                if (capture_scope_index != sema_no_scope()) {
                    u32 capture_local =
                        sema_lookup_local(sema, capture_scope_index, node->a);
                    if (capture_local == sema_no_local()) {
                        capture_local = sema_lookup_decl_local(
                            sema, capture_scope_index, node->a);
                    }
                    if (capture_local != sema_no_local()) {
                        return error_0317_non_closure_capture(
                            lexer->source,
                            sema_node_span(lexer, node),
                            lex_symbol(lexer, node->a));
                    }
                }
                return error_0300_unknown_symbol(lexer->source,
                                                 sema_node_span(lexer, node),
                                                 lex_symbol(lexer, node->a));
            }
            if (sema->decls[decl_index].kind == SK_TypeAlias) {
                return error_0308_type_used_as_value(
                    lexer->source,
                    sema_node_span(lexer, node),
                    lex_symbol(lexer, node->a));
            }
            sema->node_decl_indices[node_index] = decl_index;
            return true;
        }
    case AK_FnDef:
        if (sema->node_lowered_symbol_handles[node_index] == U32_MAX) {
            sema->node_lowered_symbol_handles[node_index] =
                sema_mangle_anonymous_function_symbol(
                    lexer, current_function_symbol, node_index);
        }
        sema_mark_fn_signature_type_nodes(ast, sema, node_index);
        return sema_collect_function_locals(
            lexer,
            ast,
            owner_decl_index,
            sema->node_lowered_symbol_handles[node_index],
            scope_index,
            node_index,
            sema);
    default:
        return true;
    }
}

//------------------------------------------------------------------------------
// Walk an expression subtree and record declaration dependencies.

internal void sema_collect_node_deps(const Ast*  ast,
                                     const Sema* sema,
                                     u32         owner_decl_index,
                                     u32         node_index,
                                     Sema*       out_sema)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_StringLiteral:
        return;
    case AK_InterpPartExpr:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            sema_collect_node_deps(ast, sema, owner_decl_index, i, out_sema);
        }
        return;
    case AK_StringConcat:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    case AK_On:
        {
            const AstOnInfo* on = &ast->ons[node->b];
            sema_collect_node_deps(
                ast, sema, owner_decl_index, node->a, out_sema);
            sema_collect_node_deps(
                ast, sema, owner_decl_index, on->true_expr_node_index, out_sema);
            sema_collect_node_deps(ast,
                                   sema,
                                   owner_decl_index,
                                   on->false_expr_node_index,
                                   out_sema);
            return;
        }
    case AK_Call:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        {
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                sema_collect_node_deps(ast,
                                       sema,
                                       owner_decl_index,
                                       ast->call_args[call->first_arg + i],
                                       out_sema);
            }
        }
        return;
    case AK_SymbolRef:
        {
            if (sema->node_is_type_expr[node_index]) {
                return;
            }
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                return;
            }
            u32 decl_index = sema->node_decl_indices[node_index];
            ASSERT(decl_index != sema_no_decl(),
                   "Expected resolved symbol reference");
            if (sema->decls[decl_index].kind == SK_BuiltinFunction) {
                return;
            }
            sema_add_dep(out_sema, owner_decl_index, decl_index);
            return;
        }
    case AK_IntegerNegate:
    case AK_Cast:
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_Assign:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        if (sema->node_local_indices[node_index] == sema_no_local() &&
            sema->node_decl_indices[node_index] != sema_no_decl()) {
            sema_add_dep(out_sema,
                         owner_decl_index,
                         sema->node_decl_indices[node_index]);
        }
        return;
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            sema_collect_node_deps(ast, sema, owner_decl_index, i, out_sema);
            if (ast->nodes[i].kind == AK_Block) {
                i = ast->nodes[i].b - 1;
            }
        }
        return;
    case AK_Variable:
        if (node->b < array_count(ast->nodes) &&
            ast->nodes[node->b].kind != AK_ZeroInit) {
            u32 value_node_index = node->b;
            if (ast->nodes[value_node_index].kind == AK_AnnotatedValue) {
                value_node_index = ast->nodes[value_node_index].b;
            }
            sema_collect_node_deps(
                ast, sema, owner_decl_index, value_node_index, out_sema);
        }
        return;
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    case AK_FnDef:
        {
            const AstNode* fn_start = &ast->nodes[node->a];
            ASSERT(fn_start->kind == AK_FnStart,
                   "Expected function start node");
            for (u32 i = node->a + 1; i < fn_start->b; ++i) {
                if (ast->nodes[i].kind == AK_Block ||
                    ast->nodes[i].kind == AK_Statement ||
                    ast->nodes[i].kind == AK_Return ||
                    ast->nodes[i].kind == AK_Variable ||
                    ast->nodes[i].kind == AK_Assign ||
                    (node->b == AFK_Expr &&
                     ast->nodes[i].kind == AK_Expression)) {
                    sema_collect_node_deps(
                        ast, sema, owner_decl_index, i, out_sema);
                    if (ast->nodes[i].kind == AK_Block) {
                        i = ast->nodes[i].b - 1;
                    }
                }
            }
            return;
        }
    default:
        return;
    }
}

//------------------------------------------------------------------------------
// Collect dependency edges for all top-level declarations.

internal void
sema_collect_deps(const Ast* ast, const Sema* sema, Sema* out_sema)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (sema->decls[i].kind == SK_TypeAlias) {
            continue;
        }
        if (sema->decls[i].value_node_index == sema_no_decl()) {
            continue;
        }
        sema_collect_node_deps(
            ast, sema, i, sema->decls[i].value_node_index, out_sema);
    }
}

//------------------------------------------------------------------------------
// Depth-first visit states used while ordering declarations.

typedef enum : u8 {
    SEMA_ORDER_UNSEEN,
    SEMA_ORDER_VISITING,
    SEMA_ORDER_DONE,
} SemaOrderState;

//------------------------------------------------------------------------------
// Visit one declaration and append it after its dependencies.

internal bool sema_order_decl(const Lexer* lexer,
                              const Ast*   ast,
                              const Sema*  sema,
                              u32          decl_index,
                              Array(u8) visit_states,
                              Array(u32) * out_order)
{
    switch ((SemaOrderState)visit_states[decl_index]) {
    case SEMA_ORDER_DONE:
        return true;
    case SEMA_ORDER_VISITING:
        return true;
    case SEMA_ORDER_UNSEEN:
        break;
    }

    visit_states[decl_index] = SEMA_ORDER_VISITING;

    for (u32 i = 0; i < array_count(sema->deps); ++i) {
        const SemaDeclDep* dep = &sema->deps[i];
        if (dep->from_decl_index != decl_index) {
            continue;
        }

        if (visit_states[dep->to_decl_index] == SEMA_ORDER_VISITING) {
            const SemaDecl* decl       = &sema->decls[decl_index];
            const SemaDecl* dependency = &sema->decls[dep->to_decl_index];
            return error_0302_dependency_cycle(
                lexer->source,
                sema_decl_span(lexer, ast, decl),
                lex_symbol(lexer, decl->symbol_handle),
                sema_decl_span(lexer, ast, dependency),
                lex_symbol(lexer, dependency->symbol_handle));
        }

        if (!sema_order_decl(lexer,
                             ast,
                             sema,
                             dep->to_decl_index,
                             visit_states,
                             out_order)) {
            return false;
        }
    }

    visit_states[decl_index] = SEMA_ORDER_DONE;
    array_push(*out_order, decl_index);
    return true;
}

//------------------------------------------------------------------------------
// Build a dependency-safe declaration order.

internal bool sema_order_decls(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    Array(u8) visit_states = NULL;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        array_push(visit_states, SEMA_ORDER_UNSEEN);
    }

    bool ok = true;
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        if (!sema_order_decl(lexer,
                             ast,
                             &*sema,
                             i,
                             visit_states,
                             &sema->ordered_decl_indices)) {
            ok = false;
            break;
        }
    }

    array_free(visit_states);
    return ok;
}

//------------------------------------------------------------------------------
// Resolve AST symbol references to top-level declarations.

internal bool
sema_resolve_symbol_refs(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->decls); ++i) {
        const SemaDecl* decl = &sema->decls[i];
        if (decl->value_node_index == sema_no_decl()) {
            continue;
        }
        if (decl->kind == SK_Function) {
            sema->node_lowered_symbol_handles[decl->value_node_index] =
                decl->symbol_handle;
        }
        if (!sema_resolve_node_refs(lexer,
                                    ast,
                                    i,
                                    decl->symbol_handle,
                                    sema_no_scope(),
                                    sema_no_scope(),
                                    decl->value_node_index,
                                    sema)) {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Resolve one type-name symbol to a built-in semantic type.

internal u32 sema_type_index_for_name(Sema* sema, string name)
{
    if (string_eq(name, s("bool"))) {
        return sema_builtin_type(sema, STK_Bool);
    }
    if (string_eq(name, s("string"))) {
        return sema_builtin_type(sema, STK_String);
    }
    if (string_eq(name, s("i8"))) {
        return sema_builtin_type(sema, STK_I8);
    }
    if (string_eq(name, s("i16"))) {
        return sema_builtin_type(sema, STK_I16);
    }
    if (string_eq(name, s("i32"))) {
        return sema_builtin_type(sema, STK_I32);
    }
    if (string_eq(name, s("i64"))) {
        return sema_builtin_type(sema, STK_I64);
    }
    if (string_eq(name, s("u8"))) {
        return sema_builtin_type(sema, STK_U8);
    }
    if (string_eq(name, s("u16"))) {
        return sema_builtin_type(sema, STK_U16);
    }
    if (string_eq(name, s("u32"))) {
        return sema_builtin_type(sema, STK_U32);
    }
    if (string_eq(name, s("u64"))) {
        return sema_builtin_type(sema, STK_U64);
    }
    if (string_eq(name, s("f32"))) {
        return sema_builtin_type(sema, STK_F32);
    }
    if (string_eq(name, s("f64"))) {
        return sema_builtin_type(sema, STK_F64);
    }
    if (string_eq(name, s("isize"))) {
        return sema_builtin_type(sema, STK_Isize);
    }
    if (string_eq(name, s("usize"))) {
        return sema_builtin_type(sema, STK_Usize);
    }

    return sema_no_type();
}

//------------------------------------------------------------------------------
// Resolve one parsed type node into a semantic type row.

internal bool sema_resolve_type_node(const Lexer* lexer,
                                     const Ast*   ast,
                                     Sema*        sema,
                                     u32          node_index,
                                     u32*         out_type_index)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_SymbolRef:
        {
            u32 type_index =
                sema_type_index_for_name(sema, lex_symbol(lexer, node->a));
            if (type_index == sema_no_type()) {
                u32 decl_index = sema_find_decl(sema, node->a);
                if (decl_index != sema_no_decl() &&
                    sema->decls[decl_index].kind == SK_TypeAlias) {
                    type_index = sema->decls[decl_index].type_index;
                }
            }
            if (type_index == sema_no_type()) {
                return error_0303_unknown_type(lexer->source,
                                               sema_node_span(lexer, node),
                                               lex_symbol(lexer, node->a));
            }
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    case AK_TypeFn:
        {
            const AstFnSignature* signature = sema_ast_signature(ast, node);
            Array(u32) param_types          = NULL;

            for (u32 i = 0; i < signature->param_count; ++i) {
                u32 param_type = sema_no_type();
                if (!sema_resolve_type_node(
                        lexer,
                        ast,
                        sema,
                        ast->params[signature->first_param + i].type_node_index,
                        &param_type)) {
                    array_free(param_types);
                    return false;
                }
                array_push(param_types, param_type);
            }

            u32 return_type = sema_no_type();
            if (!sema_resolve_type_node(lexer,
                                        ast,
                                        sema,
                                        signature->return_type_node_index,
                                        &return_type)) {
                array_free(param_types);
                return false;
            }

            u32 type_index =
                sema_add_function_type(sema, param_types, return_type);
            array_free(param_types);
            sema->node_type_indices[node_index] = type_index;
            *out_type_index                     = type_index;
            return true;
        }

    default:
        return error_0303_unknown_type(
            lexer->source, sema_node_span(lexer, node), s("<expression>"));
    }
}

//------------------------------------------------------------------------------
// Return whether one source type satisfies one target type without casts.

internal bool
sema_type_matches(const Sema* sema, u32 expected_type, u32 actual_type)
{
    if (expected_type == sema_no_type() || actual_type == sema_no_type()) {
        return true;
    }
    if (expected_type == actual_type) {
        return true;
    }

    return sema_type_is_concrete_integer(sema, expected_type) &&
           sema->types[actual_type].kind == STK_UntypedInteger;
}

internal bool sema_type_is_castable_primitive(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_UntypedInteger:
    case STK_Bool:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_F32:
    case STK_F64:
    case STK_Isize:
    case STK_Usize:
        return true;
    default:
        return false;
    }
}

internal bool sema_type_is_variable_storage(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_String:
    case STK_Bool:
    case STK_F32:
    case STK_F64:
    case STK_Function:
        return true;
    default:
        return sema_type_is_concrete_integer(sema, type_index);
    }
}

internal bool sema_type_is_interpolatable(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (sema->types[type_index].kind) {
    case STK_String:
    case STK_Bool:
    case STK_F32:
    case STK_F64:
        return true;
    default:
        return sema_type_is_integer(sema, type_index);
    }
}

internal bool sema_infer_node_type(const Lexer* lexer,
                                   const Ast*   ast,
                                   Sema*        sema,
                                   u32          node_index,
                                   u32          expected_type,
                                   u32*         out_type_index);
internal bool sema_node_contains_interpolation(const Ast* ast, u32 node_index);
internal u32 sema_find_interpolated_string_node(const Ast* ast, u32 node_index);
internal bool sema_validate_interpolated_strings(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          node_index);
internal bool sema_infer_local_binding_type(const Lexer* lexer,
                                            const Ast*   ast,
                                            Sema*        sema,
                                            u32          local_index,
                                            u32*         out_type_index);

internal bool sema_infer_local_binding_type(const Lexer* lexer,
                                            const Ast*   ast,
                                            Sema*        sema,
                                            u32          local_index,
                                            u32*         out_type_index)
{
    SemaLocal* local = &sema->locals[local_index];
    if (local->type_index != sema_no_type()) {
        *out_type_index = local->type_index;
        return true;
    }

    AstNode* bind_node = &ast->nodes[local->decl_node_index];
    if (ast_has_flag(bind_node, ANF_ConstBusy)) {
        return error_0302_dependency_cycle(
            lexer->source,
            sema_local_span(lexer, ast, local),
            lex_symbol(lexer, local->symbol_handle),
            sema_local_span(lexer, ast, local),
            lex_symbol(lexer, local->symbol_handle));
    }

    ast_set_flag(bind_node, ANF_ConstBusy);

    u32  annotated = sema_no_type();
    u32  inferred  = sema_no_type();
    bool ok        = true;

    if (local->type_node_index != sema_no_type() &&
        !sema_resolve_type_node(
            lexer, ast, sema, local->type_node_index, &annotated)) {
        ok = false;
    }

    if (ok && local->value_node_index != sema_no_decl() &&
        !sema_infer_node_type(
            lexer, ast, sema, local->value_node_index, annotated, &inferred)) {
        ok = false;
    }

    if (ok) {
        local->type_index = annotated != sema_no_type()
                                ? annotated
                                : (local->kind == SLK_Function
                                       ? inferred
                                       : sema_materialise_type(sema, inferred));
        sema->node_type_indices[local->decl_node_index] = local->type_index;
        *out_type_index                                 = local->type_index;
    }

    ast_clear_flag(bind_node, ANF_ConstBusy);
    return ok;
}

internal bool sema_infer_block_statements(const Lexer* lexer,
                                          const Ast*   ast,
                                          Sema*        sema,
                                          u32          first_node,
                                          u32          end_node,
                                          u32*         in_out_return_type,
                                          bool*        out_has_return)
{
    for (u32 i = first_node; i < end_node; ++i) {
        const AstNode* stmt = &ast->nodes[i];

        if (stmt->kind == AK_Block) {
            if (!sema_infer_block_statements(lexer,
                                             ast,
                                             sema,
                                             stmt->a,
                                             stmt->b,
                                             in_out_return_type,
                                             out_has_return)) {
                return false;
            }
            if (*out_has_return) {
                return true;
            }
            i = stmt->b - 1;
            continue;
        }

        if (stmt->kind == AK_Variable || stmt->kind == AK_Assign) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, i, sema_no_type(), &ignored)) {
                return false;
            }
            continue;
        }

        if (stmt->kind == AK_Statement) {
            u32 ignored = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, i, sema_no_type(), &ignored)) {
                return false;
            }
            if (sema_node_contains_interpolation(ast, i) &&
                !sema_validate_interpolated_strings(lexer, ast, sema, i)) {
                return false;
            }
            continue;
        }

        if (stmt->kind != AK_Return) {
            continue;
        }

        if (sema_node_contains_interpolation(ast, stmt->a)) {
            u32 interp_index = sema_find_interpolated_string_node(ast, stmt->a);
            return error_0312_interpolated_string_escapes(
                lexer->source,
                sema_node_span(lexer,
                               &ast->nodes[interp_index == sema_no_decl()
                                               ? stmt->a
                                               : interp_index]));
        }

        if (!sema_infer_node_type(lexer,
                                  ast,
                                  sema,
                                  stmt->a,
                                  sema_no_type(),
                                  in_out_return_type)) {
            return false;
        }
        *in_out_return_type = sema_materialise_type(sema, *in_out_return_type);
        *out_has_return     = true;
        return true;
    }

    return true;
}

internal bool sema_node_contains_interpolation(const Ast* ast, u32 node_index)
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
        return sema_node_contains_interpolation(ast, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            return sema_node_contains_interpolation(ast, node->a) ||
                   sema_node_contains_interpolation(ast,
                                                    on->true_expr_node_index) ||
                   sema_node_contains_interpolation(
                       ast, on->false_expr_node_index);
        }
        return sema_node_contains_interpolation(ast, node->a) ||
               sema_node_contains_interpolation(ast, node->b);
    case AK_Call:
        if (sema_node_contains_interpolation(ast, node->a)) {
            return true;
        }
        {
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (sema_node_contains_interpolation(
                        ast, ast->call_args[call->first_arg + i])) {
                    return true;
                }
            }
            return false;
        }
    case AK_Variable:
    case AK_Assign:
    case AK_AnnotatedValue:
        return sema_node_contains_interpolation(ast, node->b);
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            if (sema_node_contains_interpolation(ast, i)) {
                return true;
            }
            if (ast->nodes[i].kind == AK_Block) {
                i = ast->nodes[i].b - 1;
            }
        }
        return false;
    case AK_TypeFn:
        return false;
    case AK_FnDef:
        {
            const AstNode* fn_start = &ast->nodes[node->a];
            for (u32 i = node->a + 1; i < fn_start->b; ++i) {
                if (sema_node_contains_interpolation(ast, i)) {
                    return true;
                }
                if (ast->nodes[i].kind == AK_Block) {
                    i = ast->nodes[i].b - 1;
                }
            }
            return false;
        }
    default:
        return false;
    }
}

internal u32 sema_find_interpolated_string_node(const Ast* ast, u32 node_index)
{
    const AstNode* node = &ast->nodes[node_index];

    if (node->kind == AK_InterpolatedString) {
        return node_index;
    }

    switch (node->kind) {
    case AK_InterpPartExpr:
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
    case AK_IntegerNegate:
    case AK_Cast:
        return sema_find_interpolated_string_node(ast, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            u32 found = sema_find_interpolated_string_node(ast, node->a);
            if (found != sema_no_decl()) {
                return found;
            }
            found = sema_find_interpolated_string_node(
                ast, on->true_expr_node_index);
            return found != sema_no_decl()
                       ? found
                       : sema_find_interpolated_string_node(
                             ast, on->false_expr_node_index);
        }
        {
            u32 left = sema_find_interpolated_string_node(ast, node->a);
            return left != sema_no_decl()
                       ? left
                       : sema_find_interpolated_string_node(ast, node->b);
        }
    case AK_Call:
        {
            u32 found = sema_find_interpolated_string_node(ast, node->a);
            if (found != sema_no_decl()) {
                return found;
            }

            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                found = sema_find_interpolated_string_node(
                    ast, ast->call_args[call->first_arg + i]);
                if (found != sema_no_decl()) {
                    return found;
                }
            }
            return sema_no_decl();
        }
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            u32 found = sema_find_interpolated_string_node(ast, i);
            if (found != sema_no_decl()) {
                return found;
            }
            if (ast->nodes[i].kind == AK_Block) {
                i = ast->nodes[i].b - 1;
            }
        }
        return sema_no_decl();
    default:
        return sema_no_decl();
    }
}

internal bool sema_validate_interpolated_strings(const Lexer* lexer,
                                                 const Ast*   ast,
                                                 Sema*        sema,
                                                 u32          node_index)
{
    const AstNode* node = &ast->nodes[node_index];

    if (node->kind == AK_InterpolatedString) {
        u32 ignored = sema_no_type();
        return sema_infer_node_type(
            lexer, ast, sema, node_index, sema_no_type(), &ignored);
    }

    switch (node->kind) {
    case AK_InterpPartExpr:
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
    case AK_IntegerNegate:
    case AK_Cast:
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a);
    case AK_StringConcat:
    case AK_On:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        if (node->kind == AK_On) {
            const AstOnInfo* on = &ast->ons[node->b];
            return sema_validate_interpolated_strings(lexer, ast, sema, node->a) &&
                   sema_validate_interpolated_strings(
                       lexer, ast, sema, on->true_expr_node_index) &&
                   sema_validate_interpolated_strings(
                       lexer, ast, sema, on->false_expr_node_index);
        }
        return sema_validate_interpolated_strings(lexer, ast, sema, node->a) &&
               sema_validate_interpolated_strings(lexer, ast, sema, node->b);
    case AK_Call:
        if (!sema_validate_interpolated_strings(lexer, ast, sema, node->a)) {
            return false;
        }
        {
            const AstCallInfo* call = &ast->calls[node->b];
            for (u32 i = 0; i < call->arg_count; ++i) {
                if (!sema_validate_interpolated_strings(
                        lexer,
                        ast,
                        sema,
                        ast->call_args[call->first_arg + i])) {
                    return false;
                }
            }
            return true;
        }
    case AK_Block:
        for (u32 i = node->a; i < node->b; ++i) {
            if (!sema_validate_interpolated_strings(lexer, ast, sema, i)) {
                return false;
            }
            if (ast->nodes[i].kind == AK_Block) {
                i = ast->nodes[i].b - 1;
            }
        }
        return true;
    default:
        return true;
    }
}

//------------------------------------------------------------------------------
// Infer one AST node type, optionally using an expected context type.

internal bool sema_infer_node_type(const Lexer* lexer,
                                   const Ast*   ast,
                                   Sema*        sema,
                                   u32          node_index,
                                   u32          expected_type,
                                   u32*         out_type_index)
{
    const AstNode* node       = &ast->nodes[node_index];
    u32            type_index = sema_no_type();

    switch (node->kind) {
    case AK_IntegerLiteral:
        type_index = sema_type_is_concrete_integer(sema, expected_type)
                         ? expected_type
                         : sema_builtin_type(sema, STK_UntypedInteger);
        break;

    case AK_StringLiteral:
    case AK_StringConcat:
        type_index = sema_builtin_type(sema, STK_String);
        break;

    case AK_InterpPartExpr:
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_InterpolatedString:
        for (u32 i = node->a; i < node->b; ++i) {
            const AstNode* part      = &ast->nodes[i];
            u32            part_type = sema_no_type();

            if (part->kind == AK_StringLiteral) {
                continue;
            }
            ASSERT(part->kind == AK_InterpPartExpr,
                   "Expected interpolated string part wrapper");
            if (!sema_infer_node_type(
                    lexer, ast, sema, part->a, sema_no_type(), &part_type)) {
                return false;
            }
            part_type                  = sema_materialise_type(sema, part_type);
            sema->node_type_indices[i] = part_type;
            if (!sema_type_is_interpolatable(sema, part_type)) {
                return error_0311_invalid_interpolation_type(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[part->a]),
                    sema_type_name(sema, &temp_arena, part_type));
            }
        }
        type_index = sema_builtin_type(sema, STK_String);
        break;

    case AK_Expression:
    case AK_Statement:
    case AK_Return:
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_SymbolRef:
        {
            if (sema->node_is_type_expr[node_index]) {
                if (!sema_resolve_type_node(
                        lexer, ast, sema, node_index, &type_index)) {
                    return false;
                }
                break;
            }
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                u32 local_index        = sema->node_local_indices[node_index];
                const SemaLocal* local = &sema->locals[local_index];
                if (sema_local_is_decl_binding(local) &&
                    local->type_index == sema_no_type()) {
                    if (!sema_infer_local_binding_type(
                            lexer, ast, sema, local_index, &type_index)) {
                        return false;
                    }
                } else {
                    type_index = local->type_index;
                }
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                ASSERT(decl_index != sema_no_decl(),
                       "Expected resolved symbol reference");
                type_index = sema->decls[decl_index].type_index;
            }
            if (sema_type_is_concrete_integer(sema, expected_type) &&
                type_index != sema_no_type() &&
                sema->types[type_index].kind == STK_UntypedInteger) {
                type_index = expected_type;
            }
        }
        break;

    case AK_Bind:
        {
            u32 local_index = sema->node_local_indices[node_index];
            ASSERT(local_index != sema_no_local(),
                   "Expected resolved local bind");
            if (!sema_infer_local_binding_type(
                    lexer, ast, sema, local_index, &type_index)) {
                return false;
            }
        }
        break;

    case AK_IntegerNegate:
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
        }
        break;

    case AK_Cast:
        {
            u32 source_type = sema_no_type();
            u32 target_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &source_type)) {
                return false;
            }
            if (!sema_resolve_type_node(
                    lexer, ast, sema, node->b, &target_type)) {
                return false;
            }

            if (!(sema_type_is_castable_primitive(sema, source_type) &&
                  sema_type_is_castable_primitive(sema, target_type) &&
                  sema->types[target_type].kind != STK_UntypedInteger)) {
                return error_0307_invalid_cast(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(sema, &temp_arena, source_type),
                    sema_type_name(sema, &temp_arena, target_type));
            }

            type_index = target_type;
        }
        break;

    case AK_On:
        {
            const AstOnInfo* on        = &ast->ons[node->b];
            u32              bool_type = sema_builtin_type(sema, STK_Bool);
            u32              condition_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &condition_type)) {
                return false;
            }
            if (condition_type != bool_type) {
                return error_0319_invalid_on_condition(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->a]),
                    sema_type_name(sema, &temp_arena, condition_type));
            }

            u32 true_type = sema_no_type();
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      on->true_expr_node_index,
                                      expected_type,
                                      &true_type)) {
                return false;
            }

            u32 false_expected = expected_type;
            if (false_expected == sema_no_type() &&
                sema_type_is_concrete_integer(sema, true_type)) {
                false_expected = true_type;
            }

            u32 false_type = sema_no_type();
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      on->false_expr_node_index,
                                      false_expected,
                                      &false_type)) {
                return false;
            }

            if (sema_type_is_concrete_integer(sema, true_type) &&
                false_type != sema_no_type() &&
                sema->types[false_type].kind == STK_UntypedInteger) {
                false_type = true_type;
            } else if (sema_type_is_concrete_integer(sema, false_type) &&
                       true_type != sema_no_type() &&
                       sema->types[true_type].kind == STK_UntypedInteger) {
                true_type = false_type;
            }

            if (true_type != false_type) {
                return error_0320_on_branch_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[on->true_expr_node_index]),
                    sema_type_name(sema, &temp_arena, true_type),
                    sema_node_span(lexer, &ast->nodes[on->false_expr_node_index]),
                    sema_type_name(sema, &temp_arena, false_type));
            }

            type_index = true_type;
        }
        break;

    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        {
            u32 lhs_type = sema_no_type();
            u32 rhs_type = sema_no_type();

            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, expected_type, &lhs_type)) {
                return false;
            }
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->b, lhs_type, &rhs_type)) {
                return false;
            }

            if (sema_type_is_concrete_integer(sema, lhs_type) &&
                rhs_type != sema_no_type() &&
                sema->types[rhs_type].kind == STK_UntypedInteger) {
                rhs_type = lhs_type;
            } else if (sema_type_is_concrete_integer(sema, rhs_type) &&
                       lhs_type != sema_no_type() &&
                       sema->types[lhs_type].kind == STK_UntypedInteger) {
                lhs_type = rhs_type;
            }

            if (lhs_type != rhs_type) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    sema_type_name(sema, &temp_arena, lhs_type),
                    sema_type_name(sema, &temp_arena, rhs_type));
            }
            type_index = lhs_type;
        }
        break;

    case AK_Call:
        {
            u32 callee_type = sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->a, sema_no_type(), &callee_type)) {
                return false;
            }
            if (callee_type == sema_no_type() ||
                sema->types[callee_type].kind != STK_Function) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    s("function"),
                    sema_type_name(sema, &temp_arena, callee_type));
            }

            const SemaType*    fn_type = &sema->types[callee_type];
            const AstCallInfo* call    = &ast->calls[node->b];
            if (fn_type->param_count != call->arg_count) {
                return error_0313_argument_count_mismatch(
                    lexer->source,
                    sema_node_span(lexer, node),
                    fn_type->param_count,
                    call->arg_count);
            }

            for (u32 i = 0; i < call->arg_count; ++i) {
                u32 arg_node = ast->call_args[call->first_arg + i];
                u32 expected_arg =
                    sema->type_param_types[fn_type->first_param_type + i];
                u32 arg_type = sema_no_type();
                if (!sema_infer_node_type(
                        lexer, ast, sema, arg_node, expected_arg, &arg_type)) {
                    return false;
                }
                if (!sema_type_matches(sema, expected_arg, arg_type)) {
                    return error_0304_type_mismatch(
                        lexer->source,
                        sema_node_span(lexer, &ast->nodes[arg_node]),
                        sema_type_name(sema, &temp_arena, expected_arg),
                        sema_type_name(sema, &temp_arena, arg_type));
                }
            }

            type_index = fn_type->return_type;
        }
        break;

    case AK_Variable:
        {
            u32 local_index = sema->node_local_indices[node_index];
            ASSERT(local_index != sema_no_local(), "Expected resolved local");
            SemaLocal* local = &sema->locals[local_index];
            if (local->type_index != sema_no_type()) {
                type_index = local->type_index;
                break;
            }

            u32 annotated = sema_no_type();
            if (local->type_node_index != sema_no_type() &&
                !sema_resolve_type_node(
                    lexer, ast, sema, local->type_node_index, &annotated)) {
                return false;
            }

            if (local->value_node_index == sema_no_decl()) {
                if (!sema_type_is_variable_storage(sema, annotated)) {
                    return error_0306_invalid_variable_type(
                        lexer->source,
                        sema_local_span(lexer, ast, local),
                        sema_type_name(sema, &temp_arena, annotated));
                }
                local->type_index = annotated;
                type_index        = annotated;
                break;
            }

            if (sema_node_contains_interpolation(ast,
                                                 local->value_node_index)) {
                u32 interp_index = sema_find_interpolated_string_node(
                    ast, local->value_node_index);
                return error_0312_interpolated_string_escapes(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[interp_index == sema_no_decl()
                                                   ? local->value_node_index
                                                   : interp_index]));
            }

            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      local->value_node_index,
                                      annotated,
                                      &type_index)) {
                return false;
            }

            type_index = annotated != sema_no_type()
                             ? annotated
                             : sema_materialise_type(sema, type_index);
            if (!sema_type_is_variable_storage(sema, type_index)) {
                return error_0306_invalid_variable_type(
                    lexer->source,
                    sema_local_span(lexer, ast, local),
                    sema_type_name(sema, &temp_arena, type_index));
            }
            local->type_index = type_index;
        }
        break;

    case AK_Assign:
        {
            u32 target_type = sema_no_type();
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                target_type = sema->locals[sema->node_local_indices[node_index]]
                                  .type_index;
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                ASSERT(decl_index != sema_no_decl(),
                       "Expected resolved target");
                if (sema->decls[decl_index].kind != SK_Variable) {
                    return error_0305_invalid_assignment_target(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                target_type = sema->decls[decl_index].type_index;
            }

            if (sema_node_contains_interpolation(ast, node->b)) {
                u32 interp_index =
                    sema_find_interpolated_string_node(ast, node->b);
                return error_0312_interpolated_string_escapes(
                    lexer->source,
                    sema_node_span(lexer,
                                   &ast->nodes[interp_index == sema_no_decl()
                                                   ? node->b
                                                   : interp_index]));
            }

            if (!sema_infer_node_type(
                    lexer, ast, sema, node->b, target_type, &type_index)) {
                return false;
            }
            type_index = target_type;
        }
        break;

    case AK_FnDef:
        {
            const AstNode*        fn_start  = &ast->nodes[node->a];
            const AstFnSignature* signature = &ast->fn_signatures[fn_start->a];
            bool                  has_explicit_return_type =
                signature->return_type_node_index != U32_MAX;

            if (node->b == AFK_Expr && has_explicit_return_type) {
                return error_0318_mixed_function_return_style(
                    lexer->source, sema_node_span(lexer, node));
            }

            u32 return_type = sema_builtin_type(
                sema, node->b == AFK_Block ? STK_I32 : STK_UntypedInteger);

            if (has_explicit_return_type &&
                !sema_resolve_type_node(lexer,
                                        ast,
                                        sema,
                                        signature->return_type_node_index,
                                        &return_type)) {
                return false;
            }

            if (node->b == AFK_Expr) {
                if (sema_node_contains_interpolation(ast, fn_start->b - 1)) {
                    u32 interp_index = sema_find_interpolated_string_node(
                        ast, fn_start->b - 1);
                    return error_0312_interpolated_string_escapes(
                        lexer->source,
                        sema_node_span(
                            lexer,
                            &ast->nodes[interp_index == sema_no_decl()
                                            ? fn_start->b - 1
                                            : interp_index]));
                }
                if (!sema_infer_node_type(lexer,
                                          ast,
                                          sema,
                                          fn_start->b - 1,
                                          sema_no_type(),
                                          &return_type)) {
                    return false;
                }
                return_type = sema_materialise_type(sema, return_type);
            } else {
                bool has_return = false;
                if (!sema_infer_block_statements(lexer,
                                                 ast,
                                                 sema,
                                                 node->a + 1,
                                                 fn_start->b,
                                                 &return_type,
                                                 &has_return)) {
                    return false;
                }
                if (has_explicit_return_type && !has_return) {
                    return error_0314_missing_return(
                        lexer->source,
                        sema_node_span(lexer, node),
                        sema_type_name(sema, &temp_arena, return_type));
                }
            }

            Array(u32) param_types = NULL;
            for (u32 i = 0; i < signature->param_count; ++i) {
                u32 param_type = sema_no_type();
                if (!sema_resolve_type_node(
                        lexer,
                        ast,
                        sema,
                        ast->params[signature->first_param + i].type_node_index,
                        &param_type)) {
                    array_free(param_types);
                    return false;
                }
                array_push(param_types, param_type);
            }
            type_index = sema_add_function_type(sema, param_types, return_type);
            array_free(param_types);
        }
        break;

    default:
        type_index = sema_no_type();
        break;
    }

    if (expected_type != sema_no_type() &&
        !sema_type_matches(sema, expected_type, type_index)) {
        return error_0304_type_mismatch(
            lexer->source,
            sema_node_span(lexer, node),
            sema_type_name(sema, &temp_arena, expected_type),
            sema_type_name(sema, &temp_arena, type_index));
    }

    sema->node_type_indices[node_index] = type_index;
    *out_type_index                     = type_index;
    return true;
}

//------------------------------------------------------------------------------
// Resolve type annotations and infer declaration/value types.

internal bool
sema_assign_decl_types(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        SemaDecl* decl          = &sema->decls[sema->ordered_decl_indices[i]];
        u32       annotated     = sema_no_type();
        u32       inferred_type = sema_no_type();

        if (decl->kind == SK_TypeAlias) {
            if (decl->bind_node_index != sema_no_decl()) {
                sema->node_type_indices[decl->bind_node_index] =
                    decl->type_index;
            }
            continue;
        }

        if (decl->kind == SK_BuiltinFunction) {
            continue;
        }

        if (decl->type_node_index != sema_no_type() &&
            !sema_resolve_type_node(
                lexer, ast, sema, decl->type_node_index, &annotated)) {
            return false;
        }

        if (decl->value_node_index != sema_no_decl()) {
            if (!sema_infer_node_type(lexer,
                                      ast,
                                      sema,
                                      decl->value_node_index,
                                      annotated,
                                      &inferred_type)) {
                return false;
            }
        }

        if (decl->kind == SK_Variable) {
            decl->type_index = annotated != sema_no_type()
                                   ? annotated
                                   : sema_materialise_type(sema, inferred_type);
            if (!sema_type_is_variable_storage(sema, decl->type_index)) {
                return error_0306_invalid_variable_type(
                    lexer->source,
                    sema_decl_span(lexer, ast, decl),
                    sema_type_name(sema, &temp_arena, decl->type_index));
            }
        } else {
            decl->type_index =
                annotated != sema_no_type()
                    ? annotated
                    : (decl->kind == SK_Function
                           ? inferred_type
                           : sema_materialise_type(sema, inferred_type));
        }

        if (decl->bind_node_index != sema_no_decl()) {
            sema->node_type_indices[decl->bind_node_index] = decl->type_index;
        }
    }

    return true;
}

internal bool
sema_assign_local_types(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->locals); ++i) {
        SemaLocal* local = &sema->locals[i];
        if (!sema_local_is_decl_binding(local)) {
            continue;
        }

        u32 ignored = sema_no_type();
        if (!sema_infer_local_binding_type(lexer, ast, sema, i, &ignored)) {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Stack-machine phases used while folding constant expressions.

typedef enum : u8 {
    SEMA_FOLD_ENTER,
    SEMA_FOLD_REDUCE,
} SemaFoldPhase;

//------------------------------------------------------------------------------
// One explicit evaluator frame for the constant-folder VM.

typedef struct {
    u32           node_index;
    SemaFoldPhase phase;
} SemaFoldFrame;

//------------------------------------------------------------------------------
// Clear AST-local fold flags before a fresh semantic pass.

internal void sema_clear_fold_flags(Ast* ast)
{
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        ast->nodes[i].flags = ANF_None;
    }
}

//------------------------------------------------------------------------------
// Push one AST node onto the folding stack for later evaluation.

internal void sema_push_fold_frame(Array(SemaFoldFrame) * stack, u32 node_index)
{
    array_push(*stack,
               (SemaFoldFrame){
                   .node_index = node_index,
                   .phase      = SEMA_FOLD_ENTER,
               });
}

//------------------------------------------------------------------------------
// Reduce one AST node after its children have already been visited.

internal bool sema_reduce_folded_node(const Lexer* lex,
                                      Ast*         ast,
                                      const Sema*  sema,
                                      u32          node_index,
                                      Sema*        out_sema,
                                      i64*         out_value)
{
    AstNode* node  = &ast->nodes[node_index];
    i64      value = 0;
    bool     ok    = false;

    switch (node->kind) {
    case AK_IntegerLiteral:
        value = (i64)ast_get_integer(lex, node);
        ok    = true;
        break;

    case AK_StringLiteral:
    case AK_InterpPartExpr:
    case AK_InterpolatedString:
        ok = false;
        break;
    case AK_StringConcat:
        ok = false;
        break;

    case AK_SymbolRef:
        {
            if (out_sema->node_local_indices[node_index] != sema_no_local()) {
                const SemaLocal* local =
                    &sema->locals[out_sema->node_local_indices[node_index]];
                if (local->kind == SLK_Constant) {
                    ok = sema_try_get_constant(
                        ast, out_sema, local->value_node_index, &value);
                } else {
                    ok = false;
                }
                break;
            }
            u32 decl_index = sema->node_decl_indices[node_index];
            ASSERT(decl_index != sema_no_decl(),
                   "Expected resolved symbol reference");
            const SemaDecl* decl = &sema->decls[decl_index];
            if (decl->kind == SK_Constant) {
                ok = sema_try_get_constant(
                    ast, out_sema, decl->value_node_index, &value);
            }
        }
        break;

    case AK_Expression:
    case AK_Statement:
    case AK_Return:
        ok = sema_try_get_constant(ast, out_sema, node->a, &value);
        break;

    case AK_Call:
    case AK_Cast:
    case AK_FnDef:
    case AK_Variable:
    case AK_Assign:
        ok = false;
        break;

    case AK_IntegerNegate:
        ok = sema_try_get_constant(ast, out_sema, node->a, &value);
        if (ok) {
            value = -value;
        }
        break;

    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
        {
            i64 lhs = 0;
            i64 rhs = 0;
            ok      = sema_try_get_constant(ast, out_sema, node->a, &lhs) &&
                 sema_try_get_constant(ast, out_sema, node->b, &rhs);
            if (!ok) {
                break;
            }

            switch (node->kind) {
            case AK_IntegerPlus:
                value = lhs + rhs;
                break;
            case AK_IntegerMinus:
                value = lhs - rhs;
                break;
            case AK_IntegerMultiply:
                value = lhs * rhs;
                break;
            case AK_IntegerDivide:
                if (rhs == 0) {
                    ok = false;
                    break;
                }
                value = lhs / rhs;
                break;
            case AK_IntegerModulo:
                if (rhs == 0) {
                    ok = false;
                    break;
                }
                value = lhs % rhs;
                break;
            default:
                ok = false;
                break;
            }
        }
        break;

    default:
        ok = false;
        break;
    }

    ast_clear_flag(node, ANF_ConstBusy);
    if (!ok) {
        return false;
    }

    sema_set_constant(ast, out_sema, node_index, value);
    *out_value = value;
    return true;
}

//------------------------------------------------------------------------------
// Fold one AST node to a signed integer using an explicit VM-style stack.

internal bool sema_fold_node(const Lexer* lex,
                             Ast*         ast,
                             const Sema*  sema,
                             u32          root_node_index,
                             Sema*        out_sema,
                             i64*         out_value)
{
    if (root_node_index < array_count(sema->node_is_type_expr) &&
        sema->node_is_type_expr[root_node_index]) {
        return false;
    }

    Array(SemaFoldFrame) stack = NULL;
    sema_push_fold_frame(&stack, root_node_index);

    while (array_count(stack) > 0) {
        SemaFoldFrame* frame = &stack[array_count(stack) - 1];
        AstNode*       node  = &ast->nodes[frame->node_index];
        i64            value = 0;

        if (frame->node_index < array_count(sema->node_is_type_expr) &&
            sema->node_is_type_expr[frame->node_index]) {
            array_pop(stack);
            continue;
        }

        if (frame->phase == SEMA_FOLD_ENTER) {
            if (sema_try_get_constant(
                    ast, out_sema, frame->node_index, &value)) {
                array_pop(stack);
                continue;
            }

            if (ast_has_flag(node, ANF_ConstBusy)) {
                array_pop(stack);
                continue;
            }

            ast_set_flag(node, ANF_ConstBusy);
            frame->phase = SEMA_FOLD_REDUCE;

            switch (node->kind) {
            case AK_SymbolRef:
                {
                    if (out_sema->node_local_indices[frame->node_index] !=
                        sema_no_local()) {
                        const SemaLocal* local =
                            &sema->locals[out_sema->node_local_indices
                                              [frame->node_index]];
                        if (local->kind == SLK_Constant) {
                            sema_push_fold_frame(&stack,
                                                 local->value_node_index);
                        }
                        break;
                    }
                    u32 decl_index = sema->node_decl_indices[frame->node_index];
                    ASSERT(decl_index != sema_no_decl(),
                           "Expected resolved symbol reference");
                    const SemaDecl* decl = &sema->decls[decl_index];
                    if (decl->kind == SK_Constant) {
                        sema_push_fold_frame(&stack, decl->value_node_index);
                    }
                }
                break;

            case AK_Expression:
            case AK_Statement:
            case AK_Return:
            case AK_IntegerNegate:
            case AK_Cast:
            case AK_InterpPartExpr:
                sema_push_fold_frame(&stack, node->a);
                break;

            case AK_Call:
                {
                    const AstCallInfo* call = &ast->calls[node->b];
                    for (u32 i = call->arg_count; i-- > 0;) {
                        sema_push_fold_frame(
                            &stack, ast->call_args[call->first_arg + i]);
                    }
                }
                sema_push_fold_frame(&stack, node->a);
                break;

            case AK_InterpolatedString:
                for (u32 i = node->b; i-- > node->a;) {
                    sema_push_fold_frame(&stack, i);
                }
                break;

            case AK_IntegerPlus:
            case AK_IntegerMinus:
            case AK_IntegerMultiply:
            case AK_IntegerDivide:
            case AK_IntegerModulo:
                sema_push_fold_frame(&stack, node->b);
                sema_push_fold_frame(&stack, node->a);
                break;

            default:
                break;
            }

            continue;
        }

        sema_reduce_folded_node(
            lex, ast, sema, frame->node_index, out_sema, &value);
        array_pop(stack);
    }

    array_free(stack);
    return sema_try_get_constant(ast, out_sema, root_node_index, out_value);
}

//------------------------------------------------------------------------------
// Fold all constant-capable AST nodes into semantic side tables.

internal void sema_fold_constants(const Lexer* lex, Ast* ast, Sema* sema)
{
    sema_clear_fold_flags(ast);

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        i64 ignored = 0;
        sema_fold_node(lex, ast, sema, i, sema, &ignored);
    }
}

internal bool
sema_validate_entry_point(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    u32 main_symbol = sema_find_symbol_handle_by_name(lexer, s("main"));
    if (main_symbol == sema_no_decl()) {
        return error_0315_missing_entry_point(
            lexer->source, (ErrorSpan){.start = 0, .end = 0});
    }

    u32 decl_index = sema_find_decl(sema, main_symbol);
    if (decl_index == sema_no_decl()) {
        return error_0315_missing_entry_point(
            lexer->source, (ErrorSpan){.start = 0, .end = 0});
    }

    const SemaDecl* decl       = &sema->decls[decl_index];
    u32             type_index = decl->type_index;
    if (decl->kind != SK_Function || type_index == sema_no_type() ||
        sema->types[type_index].kind != STK_Function) {
        return error_0316_invalid_entry_point(
            lexer->source,
            sema_decl_span(lexer, ast, decl),
            sema_type_name(sema, &temp_arena, type_index));
    }

    const SemaType* fn_type = &sema->types[type_index];
    if (fn_type->param_count != 0 ||
        !sema_type_is_integer(sema, fn_type->return_type)) {
        return error_0316_invalid_entry_point(
            lexer->source,
            sema_decl_span(lexer, ast, decl),
            sema_type_name(sema, &temp_arena, type_index));
    }

    return true;
}

//------------------------------------------------------------------------------
// Analyse the AST into compact declaration and resolution tables.

bool sema_analyse(const Lexer* lexer, Ast* ast, Sema* out_sema)
{
    Sema sema = {0};

    // Seed commonly-used built-in types so later materialisation can always
    // canonicalise untyped integers to i32.
    sema_builtin_type(&sema, STK_Void);
    sema_builtin_type(&sema, STK_UntypedInteger);
    sema_builtin_type(&sema, STK_String);
    sema_builtin_type(&sema, STK_I32);

    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        array_push(sema.node_decl_indices, sema_no_decl());
        array_push(sema.node_local_indices, sema_no_local());
        array_push(sema.node_scope_indices, sema_no_scope());
        array_push(sema.node_lowered_symbol_handles, U32_MAX);
        array_push(sema.node_type_indices, sema_no_type());
        array_push(sema.node_is_type_expr, false);
        array_push(sema.node_const_known, false);
        array_push(sema.node_const_values, 0);
    }

    sema_add_builtin_decls(lexer, &sema);

    if (!sema_collect_decls(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_classify_type_aliases(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        if (ast->nodes[i].kind == AK_AnnotatedValue ||
            ast->nodes[i].kind == AK_ZeroInit) {
            sema_mark_type_expr_nodes(ast, &sema, ast->nodes[i].a);
        } else if (ast->nodes[i].kind == AK_Cast) {
            sema_mark_type_expr_nodes(ast, &sema, ast->nodes[i].b);
        }
    }
    for (u32 i = 0; i < array_count(sema.decls); ++i) {
        if (sema.decls[i].kind == SK_TypeAlias &&
            sema.decls[i].value_node_index != sema_no_decl()) {
            sema_mark_type_expr_nodes(
                ast, &sema, sema.decls[i].value_node_index);
        }
        if (sema.decls[i].type_node_index != sema_no_type()) {
            sema_mark_type_expr_nodes(
                ast, &sema, sema.decls[i].type_node_index);
        }
    }
    if (!sema_resolve_symbol_refs(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    sema_collect_deps(ast, &sema, &sema);
    if (!sema_order_decls(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_assign_decl_types(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_assign_local_types(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }
    if (!sema_validate_entry_point(lexer, ast, &sema)) {
        sema_done(&sema);
        return false;
    }

    sema_fold_constants(lexer, ast, &sema);

    *out_sema = sema;
    return true;
}

//------------------------------------------------------------------------------
// Free the semantic analysis tables.

void sema_done(Sema* sema)
{
    array_free(sema->types);
    array_free(sema->type_param_types);
    array_free(sema->decls);
    array_free(sema->locals);
    array_free(sema->scopes);
    array_free(sema->deps);
    array_free(sema->ordered_decl_indices);
    array_free(sema->node_decl_indices);
    array_free(sema->node_local_indices);
    array_free(sema->node_scope_indices);
    array_free(sema->node_lowered_symbol_handles);
    array_free(sema->node_type_indices);
    array_free(sema->node_is_type_expr);
    array_free(sema->node_const_known);
    array_free(sema->node_const_values);
    *sema = (Sema){0};
}

//------------------------------------------------------------------------------
