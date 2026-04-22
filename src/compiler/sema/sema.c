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

//------------------------------------------------------------------------------
// Return the sentinel type index used for unresolved entries.

u32 sema_no_type(void) { return U32_MAX; }

//------------------------------------------------------------------------------
// Predeclare the current built-in runtime functions.

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

//------------------------------------------------------------------------------
// Return the canonical type index for one built-in type.

internal u32 sema_builtin_type(Sema* sema, SemaTypeKind kind)
{
    return sema_add_type(sema,
                         (SemaType){
                             .kind        = kind,
                             .param_count = 0,
                             .a           = sema_no_type(),
                             .b           = sema_no_type(),
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
        if (type->param_count == 0) {
            return string_format(arena,
                                 "fn () -> " STRINGP,
                                 STRINGV(sema_type_name(sema, arena, type->b)));
        }
        if (type->param_count == 1) {
            return string_format(
                arena,
                "fn (" STRINGP ") -> " STRINGP,
                STRINGV(sema_type_name(sema, arena, type->a)),
                STRINGV(sema_type_name(sema, arena, type->b)));
        }
        return s("fn (...)");
    default:
        return s("<unknown>");
    }
}

//------------------------------------------------------------------------------
// Predeclare the current built-in runtime functions.

internal void sema_add_builtin_decls(const Lexer* lexer, Sema* sema)
{
    u32 pr_symbol  = sema_find_symbol_handle_by_name(lexer, s("pr"));
    u32 prn_symbol = sema_find_symbol_handle_by_name(lexer, s("prn"));
    u32 string_type = sema_builtin_type(sema, STK_String);
    u32 void_type   = sema_builtin_type(sema, STK_Void);
    u32 print_type  = sema_add_type(sema,
                                   (SemaType){
                                       .kind        = STK_Function,
                                       .param_count = 1,
                                       .a           = string_type,
                                       .b           = void_type,
                                   });

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
    return sema_node_span(lexer, &ast->nodes[local->decl_node_index]);
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

//------------------------------------------------------------------------------
// Mark one type-annotation subtree so normal symbol resolution can skip it.

internal void sema_mark_type_expr_nodes(const Ast* ast, Sema* sema, u32 node_index)
{
    if (node_index >= array_count(sema->node_is_type_expr) ||
        sema->node_is_type_expr[node_index]) {
        return;
    }

    sema->node_is_type_expr[node_index] = true;
    const AstNode* node                 = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_TypeFn:
        sema_mark_type_expr_nodes(ast, sema, node->a);
        break;
    default:
        break;
    }
}

internal u32 sema_find_local(const Sema* sema,
                             u32         owner_decl_index,
                             u32         symbol_handle)
{
    for (u32 i = array_count(sema->locals); i-- > 0;) {
        const SemaLocal* local = &sema->locals[i];
        if (local->owner_decl_index == owner_decl_index &&
            local->symbol_handle == symbol_handle) {
            return i;
        }
    }
    return sema_no_local();
}

internal bool sema_resolve_node_refs(const Lexer* lexer,
                                     const Ast*   ast,
                                     u32          owner_decl_index,
                                     u32          node_index,
                                     Sema*        sema);

internal bool sema_collect_function_locals(const Lexer* lexer,
                                           const Ast*   ast,
                                           u32          owner_decl_index,
                                           u32          fn_node_index,
                                           Sema*        sema)
{
    const AstNode* fn_def = &ast->nodes[fn_node_index];
    if (fn_def->kind != AK_FnDef || fn_def->b != AFK_Block) {
        return true;
    }

    const AstNode* fn_start = &ast->nodes[fn_def->a];
    for (u32 i = fn_def->a + 1; i < fn_start->b; ++i) {
        const AstNode* node = &ast->nodes[i];
        if (node->kind == AK_Variable) {
            if (sema_find_local(sema, owner_decl_index, node->a) != sema_no_local()) {
                const SemaLocal* previous =
                    &sema->locals[sema_find_local(sema, owner_decl_index, node->a)];
                return error_0301_duplicate_binding(lexer->source,
                                                    sema_node_span(lexer, node),
                                                    lex_symbol(lexer, node->a),
                                                    sema_local_span(
                                                        lexer, ast, previous));
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

            array_push(sema->locals,
                       (SemaLocal){
                           .symbol_handle    = node->a,
                           .owner_decl_index = owner_decl_index,
                           .decl_node_index  = i,
                           .type_node_index  = type_node_index,
                           .value_node_index = value_node_index,
                           .type_index       = sema_no_type(),
                       });
            sema->node_local_indices[i] = (u32)array_count(sema->locals) - 1;
            if (type_node_index != sema_no_type()) {
                sema_mark_type_expr_nodes(ast, sema, type_node_index);
            }
            if (value_node_index != sema_no_decl() &&
                !sema_resolve_node_refs(
                    lexer, ast, owner_decl_index, value_node_index, sema)) {
                return false;
            }
            continue;
        }

        if (node->kind == AK_Assign) {
            u32 local_index = sema_find_local(sema, owner_decl_index, node->a);
            if (local_index != sema_no_local()) {
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
            if (!sema_resolve_node_refs(
                    lexer, ast, owner_decl_index, node->b, sema)) {
                return false;
            }
            continue;
        }

        if (!sema_resolve_node_refs(lexer, ast, owner_decl_index, i, sema)) {
            return false;
        }
    }

    return true;
}

internal bool sema_resolve_node_refs(const Lexer* lexer,
                                     const Ast*   ast,
                                     u32          owner_decl_index,
                                     u32          node_index,
                                     Sema*        sema)
{
    const AstNode* node = &ast->nodes[node_index];

    switch (node->kind) {
    case AK_IntegerLiteral:
    case AK_StringLiteral:
    case AK_ZeroInit:
        return true;
    case AK_StringConcat:
    case AK_IntegerPlus:
    case AK_IntegerMinus:
    case AK_IntegerMultiply:
    case AK_IntegerDivide:
    case AK_IntegerModulo:
    case AK_Call:
        return sema_resolve_node_refs(lexer, ast, owner_decl_index, node->a, sema) &&
               sema_resolve_node_refs(lexer, ast, owner_decl_index, node->b, sema);
    case AK_IntegerNegate:
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
        return sema_resolve_node_refs(lexer, ast, owner_decl_index, node->a, sema);
    case AK_SymbolRef:
        if (sema->node_is_type_expr[node_index]) {
            return true;
        }
        {
            u32 local_index = sema_find_local(sema, owner_decl_index, node->a);
            if (local_index != sema_no_local()) {
                sema->node_local_indices[node_index] = local_index;
                return true;
            }

            u32 decl_index = sema_find_decl(sema, node->a);
            if (decl_index == sema_no_decl()) {
                return error_0300_unknown_symbol(lexer->source,
                                                 sema_node_span(lexer, node),
                                                 lex_symbol(lexer, node->a));
            }
            sema->node_decl_indices[node_index] = decl_index;
            return true;
        }
    case AK_FnDef:
        if (node->b == AFK_Block) {
            return sema_collect_function_locals(
                lexer, ast, owner_decl_index, node_index, sema);
        }
        return sema_resolve_node_refs(
            lexer, ast, owner_decl_index, ast->nodes[node->a].b - 1, sema);
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
    case AK_StringConcat:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        return;
    case AK_SymbolRef:
        {
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
    case AK_Expression:
    case AK_Statement:
    case AK_Return:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        return;
    case AK_Assign:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
        if (sema->node_local_indices[node_index] == sema_no_local() &&
            sema->node_decl_indices[node_index] != sema_no_decl()) {
            sema_add_dep(
                out_sema, owner_decl_index, sema->node_decl_indices[node_index]);
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
    case AK_Call:
        sema_collect_node_deps(ast, sema, owner_decl_index, node->a, out_sema);
        sema_collect_node_deps(ast, sema, owner_decl_index, node->b, out_sema);
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
                if (ast->nodes[i].kind == AK_Statement ||
                    (node->b == AFK_Expr &&
                     ast->nodes[i].kind == AK_Expression)) {
                    sema_collect_node_deps(
                        ast, sema, owner_decl_index, i, out_sema);
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
        if (!sema_resolve_node_refs(lexer, ast, i, decl->value_node_index, sema)) {
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
            u32 return_type = sema_no_type();
            if (!sema_resolve_type_node(lexer, ast, sema, node->a, &return_type)) {
                return false;
            }

            u32 type_index = sema_add_type(sema,
                                           (SemaType){
                                               .kind        = STK_Function,
                                               .param_count = 0,
                                               .a           = sema_no_type(),
                                               .b           = return_type,
                                           });
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

internal bool sema_type_matches(const Sema* sema, u32 expected_type, u32 actual_type)
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

internal bool sema_type_is_variable_storage(const Sema* sema, u32 type_index)
{
    return sema_type_is_concrete_integer(sema, type_index);
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
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                type_index =
                    sema->locals[sema->node_local_indices[node_index]].type_index;
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

    case AK_IntegerNegate:
        if (!sema_infer_node_type(
                lexer, ast, sema, node->a, expected_type, &type_index)) {
            return false;
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
            u32 arg_type    = sema_no_type();
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

            const SemaType* fn_type = &sema->types[callee_type];
            u32 arg_expected        =
                fn_type->param_count == 1 ? fn_type->a : sema_no_type();
            if (!sema_infer_node_type(
                    lexer, ast, sema, node->b, arg_expected, &arg_type)) {
                return false;
            }
            if (fn_type->param_count == 1 &&
                !sema_type_matches(sema, fn_type->a, arg_type)) {
                return error_0304_type_mismatch(
                    lexer->source,
                    sema_node_span(lexer, &ast->nodes[node->b]),
                    sema_type_name(sema, &temp_arena, fn_type->a),
                    sema_type_name(sema, &temp_arena, arg_type));
            }

            type_index = fn_type->b;
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
                        sema_node_span(lexer, &ast->nodes[local->decl_node_index]),
                        sema_type_name(sema, &temp_arena, annotated));
                }
                local->type_index = annotated;
                type_index        = annotated;
                break;
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
                    sema_node_span(lexer, &ast->nodes[local->decl_node_index]),
                    sema_type_name(sema, &temp_arena, type_index));
            }
            local->type_index = type_index;
        }
        break;

    case AK_Assign:
        {
            u32 target_type = sema_no_type();
            if (sema->node_local_indices[node_index] != sema_no_local()) {
                target_type =
                    sema->locals[sema->node_local_indices[node_index]].type_index;
            } else {
                u32 decl_index = sema->node_decl_indices[node_index];
                ASSERT(decl_index != sema_no_decl(), "Expected resolved target");
                if (sema->decls[decl_index].kind != SK_Variable) {
                    return error_0305_invalid_assignment_target(
                        lexer->source,
                        sema_node_span(lexer, node),
                        lex_symbol(lexer, node->a));
                }
                target_type = sema->decls[decl_index].type_index;
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
            const AstNode* fn_start = &ast->nodes[node->a];
            u32            return_type =
                sema_builtin_type(sema, node->b == AFK_Block ? STK_I32
                                                             : STK_UntypedInteger);

            if (node->b == AFK_Expr) {
                if (!sema_infer_node_type(
                        lexer, ast, sema, fn_start->b - 1, sema_no_type(), &return_type)) {
                    return false;
                }
                return_type = sema_materialise_type(sema, return_type);
            } else {
                for (u32 i = node->a + 1; i < fn_start->b; ++i) {
                    if (ast->nodes[i].kind == AK_Variable ||
                        ast->nodes[i].kind == AK_Assign) {
                        u32 ignored = sema_no_type();
                        if (!sema_infer_node_type(
                                lexer, ast, sema, i, sema_no_type(), &ignored)) {
                            return false;
                        }
                        continue;
                    }
                    if (ast->nodes[i].kind != AK_Return) {
                        continue;
                    }
                    if (!sema_infer_node_type(
                            lexer, ast, sema, ast->nodes[i].a, sema_no_type(), &return_type)) {
                        return false;
                    }
                    return_type = sema_materialise_type(sema, return_type);
                    break;
                }
            }

            type_index = sema_add_type(sema,
                                       (SemaType){
                                           .kind        = STK_Function,
                                           .param_count = 0,
                                           .a           = sema_no_type(),
                                           .b           = return_type,
                                       });
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

internal bool sema_assign_decl_types(const Lexer* lexer, const Ast* ast, Sema* sema)
{
    for (u32 i = 0; i < array_count(sema->ordered_decl_indices); ++i) {
        SemaDecl* decl          = &sema->decls[sema->ordered_decl_indices[i]];
        u32       annotated     = sema_no_type();
        u32       inferred_type = sema_no_type();

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
            decl->type_index = annotated != sema_no_type()
                                   ? annotated
                                   : (decl->kind == SK_Function
                                          ? inferred_type
                                          : sema_materialise_type(
                                                sema, inferred_type));
        }

        if (decl->bind_node_index != sema_no_decl()) {
            sema->node_type_indices[decl->bind_node_index] = decl->type_index;
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
        ok = false;
        break;
    case AK_StringConcat:
        ok = false;
        break;

    case AK_SymbolRef:
        {
            if (out_sema->node_local_indices[node_index] != sema_no_local()) {
                ok = false;
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
                sema_push_fold_frame(&stack, node->a);
                break;

            case AK_Call:
                sema_push_fold_frame(&stack, node->b);
                sema_push_fold_frame(&stack, node->a);
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
    for (u32 i = 0; i < array_count(ast->nodes); ++i) {
        if (ast->nodes[i].kind == AK_AnnotatedValue ||
            ast->nodes[i].kind == AK_ZeroInit) {
            sema_mark_type_expr_nodes(ast, &sema, ast->nodes[i].a);
        }
    }
    for (u32 i = 0; i < array_count(sema.decls); ++i) {
        if (sema.decls[i].type_node_index != sema_no_type()) {
            sema_mark_type_expr_nodes(ast, &sema, sema.decls[i].type_node_index);
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

    sema_fold_constants(lexer, ast, &sema);

    *out_sema = sema;
    return true;
}

//------------------------------------------------------------------------------
// Free the semantic analysis tables.

void sema_done(Sema* sema)
{
    array_free(sema->types);
    array_free(sema->decls);
    array_free(sema->locals);
    array_free(sema->deps);
    array_free(sema->ordered_decl_indices);
    array_free(sema->node_decl_indices);
    array_free(sema->node_local_indices);
    array_free(sema->node_type_indices);
    array_free(sema->node_is_type_expr);
    array_free(sema->node_const_known);
    array_free(sema->node_const_values);
    *sema = (Sema){0};
}

//------------------------------------------------------------------------------
