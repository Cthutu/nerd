//------------------------------------------------------------------------------
// Parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>

//------------------------------------------------------------------------------
// Parse one type annotation.

bool ast_parse_type(AstParseState* state, u32* out_node)
{
    ASSERT(out_node != NULL, "Type parser requires an output node");

    if (state->token.kind == TK_Symbol) {
        AstNode node = {
            .kind        = AK_SymbolRef,
            .token_index = state->token.token_index,
            .a           = state->token.value.symbol_handle,
        };
        return ast_emit_node(state, node, out_node);
    }

    if (state->token.kind != TK_fn) {
        return error_0205_expected_declaration_or_expression(
            state->token.source,
            ast_token_span(state, &state->token),
            state->token.kind,
            "Expected a type annotation after ':', but found " STRINGP,
            STRINGV(token_kind_to_string(state->token.kind)));
    }

    AstToken fn_token = state->token;
    if (!ast_expect_token(state, TK_LParen) || !ast_expect_token(state, TK_RParen) ||
        !ast_expect_token(state, TK_ThinArrow) || !ast_next_token(state)) {
        return false;
    }

    u32 return_type = 0;
    if (!ast_parse_type(state, &return_type)) {
        return false;
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_TypeFn,
                             .token_index = fn_token.token_index,
                             .a           = return_type,
                         },
                         out_node);
}

//------------------------------------------------------------------------------
// Classify which top-level parser should handle a token.

ParsingQuery ast_parsing_query_for_token(TokenKind kind)
{
    switch (kind) {
    case TK_Symbol:
    case TK_Integer:
    case TK_String:
    case TK_LParen:
        return PQ_Expresssion;

    case TK_fn:
        return PQ_Declaration;

    default:
        return PQ_Invalid;
    }
}

#define EMIT_NODE(_kind, _token_index, _a, _b, _out_index)                     \
    do {                                                                       \
        AstNode node = {                                                       \
            .kind        = (_kind),                                            \
            .token_index = (_token_index),                                     \
            .a           = (_a),                                               \
            .b           = (_b),                                               \
        };                                                                     \
        if (!ast_emit_node(state, node, &(_out_index))) {                      \
            return false;                                                      \
        }                                                                      \
    } while (0)

//------------------------------------------------------------------------------
// Parse one function block body as a sequence of expression statements.

internal bool ast_parse_fn_block(AstParseState* state, u32 fn_start_index)
{
    ASSERT(state->token.kind == TK_LBrace, "Expected '{' token for fn block");

    if (!ast_next_token(state)) {
        return error_0201_missing_value(state->token.source,
                                        ast_token_span(state, &state->token),
                                        state->token.kind);
    }

    while (state->token.kind != TK_RBrace) {
        u32 statement_kind       = AK_Statement;
        u32 statement_token      = state->token.token_index;
        u32 statement_expr_index = 0;

        if (state->token.kind == TK_return) {
            statement_kind = AK_Return;
            if (!ast_next_token(state)) {
                return error_0201_missing_value(
                    state->token.source,
                    ast_token_span(state, &state->token),
                    state->token.kind);
            }
        }

        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        if (!ast_parse_expr(state, &statement_expr_index)) {
            state->allow_statement_boundary = previous_boundary;
            return false;
        }
        state->allow_statement_boundary = previous_boundary;

        u32     statement_index         = 0;
        AstNode statement               = {
                          .kind        = statement_kind,
                          .token_index = statement_token,
                          .a           = statement_expr_index,
        };
        if (!ast_emit_node(state, statement, &statement_index)) {
            return false;
        }

        if (!ast_peek_token(state)) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_RBrace,
                TK_EOF);
        }

        if (state->token.kind == TK_RBrace) {
            break;
        }
    }

    UNUSED(fn_start_index);
    return ast_expect_token(state, TK_RBrace);
}

//------------------------------------------------------------------------------
// Parses a declaration of the form `fn () => <expression>`
//
// Emits the node sequence:
//      f -> AK_FnStart(d,g)
//      <expression nodes>
//      g -> AK_FnEnd(d,f)
//      d -> AK_FnDef(f,0)
//
bool ast_parse_declaration(AstParseState* state, u32* out_node)
{
    // Assume current token is `fn`
    ASSERT(state->token.kind == TK_fn, "Expected 'fn' token for declaration");
    AstToken fn_token = state->token;

    if (!ast_expect_token(state, TK_LParen)) {
        return false;
    }
    if (!ast_expect_token(state, TK_RParen)) {
        return false;
    }
    u32 fn_start_index;
    u32 fn_end_index;
    u32 fn_def_index;

    EMIT_NODE(AK_FnStart, 0, 0, 0, fn_start_index);
    u32 fn_kind = AFK_Expr;

    if (!ast_next_token(state)) {
        return error_0201_missing_value(state->token.source,
                                        ast_token_span(state, &state->token),
                                        state->token.kind);
    }

    if (state->token.kind == TK_FatArrow) {
        if (!ast_next_token(state)) {
            return error_0201_missing_value(
                state->token.source,
                ast_token_span(state, &state->token),
                state->token.kind);
        }
        if (!ast_parse_expr(state, NULL)) {
            return false;
        }
    } else if (state->token.kind == TK_LBrace) {
        fn_kind = AFK_Block;
        if (!ast_parse_fn_block(state, fn_start_index)) {
            return false;
        }
    } else {
        return error_0203_expected_token(state->token.source,
                                         ast_token_span(state, &state->token),
                                         TK_FatArrow,
                                         state->token.kind);
    }

    EMIT_NODE(AK_FnEnd, 0, 0, fn_start_index, fn_end_index);
    state->nodes[fn_start_index].b = fn_end_index;

    EMIT_NODE(
        AK_FnDef, fn_token.token_index, fn_start_index, fn_kind, fn_def_index);

    if (out_node) {
        *out_node = fn_def_index;
    }

    return true;
}

//------------------------------------------------------------------------------
// Parses a binding of the form `<symbol> :: <declaration or expression>`
//
// Emits the node sequence:
//      i -> <declaration or expression>
//      AK_Bind(sym_index, i)
//
bool ast_parse_bind(AstParseState* state, u32* out_node)
{
    // Assume current token is a symbol
    ASSERT(state->token.kind == TK_Symbol, "Expected symbol token for binding");
    AstToken bind_token = state->token;
    u32      type_index = U32_MAX;

    if (!ast_expect_token(state, TK_Colon)) {
        return false;
    }

    if (!ast_peek_token(state)) {
        return error_0205_expected_declaration_or_expression(
            state->token.source,
            ast_token_span(state, &state->token),
            TK_EOF,
            "Expected a declaration or expression after ':', but found end of "
            "file");
    }

    if (state->token.kind == TK_Colon) {
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected a declaration or expression after '::', but found "
                "end of file");
        }
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected a declaration or expression after '::', but found "
                "end of file");
        }
    } else {
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected a type after ':', but found end of file");
        }

        if (!ast_parse_type(state, &type_index)) {
            return false;
        }

        if (!ast_expect_token(state, TK_Colon)) {
            return false;
        }

        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected a declaration or expression after the type "
                "annotation, but found end of file");
        }
    }

    ParsingQuery query = ast_parsing_query_for_token(state->token.kind);
    if (query == PQ_Invalid) {
        return error_0205_expected_declaration_or_expression(
            state->token.source,
            ast_token_span(state, &state->token),
            state->token.kind,
            "Expected a declaration or expression after '::', but "
            "found " STRINGP,
            STRINGV(token_kind_to_string(state->token.kind)));
    }

    AstNode node = {0};
    u32     expr_index;

    if (query == PQ_Declaration) {
        if (!ast_parse_declaration(state, &expr_index)) {
            return false;
        }

    } else {
        if (!ast_parse_expr(state, &expr_index)) {
            return false;
        }
    }

    if (type_index != U32_MAX) {
        AstNode annotated = {
            .kind        = AK_AnnotatedValue,
            .token_index = bind_token.token_index,
            .a           = type_index,
            .b           = expr_index,
        };
        if (!ast_emit_node(state, annotated, &expr_index)) {
            return false;
        }
    }

    node = (AstNode){
        .kind        = AK_Bind,
        .token_index = bind_token.token_index,
        .a           = bind_token.value.symbol_handle,
        .b           = expr_index,
    };

    return ast_emit_node(state, node, out_node);
}

//------------------------------------------------------------------------------
// Parse the full source file as a sequence of top-level bindings.

Ast ast_parse(Lexer* lexer)
{
    AstParseState state = {
        .lexer                    = lexer,
        .token_index              = 0,
        .integer_index            = 0,
        .string_index             = 0,
        .symbol_index             = 0,
        .allow_statement_boundary = false,
        .token =
            (AstToken){
                .kind   = TK_EOF,
                .source = lexer->source,
                .offset = 0,
            },
        .nodes             = 0,
        .start_node_index  = 0,
        .start_token_index = 0,
    };

    while (ast_next_token(&state)) {
        AstToken token          = state.token;
        state.start_token_index = token.token_index;
        state.start_node_index  = array_count(state.nodes);

        // <program> :: <binding>*

        switch (token.kind) {
        case TK_Symbol:
            // Parse binding
            if (!ast_parse_bind(&state, NULL)) {
                goto error;
            }
            break;

        default:
            if (!ast_parse_expr(&state, NULL)) {
                goto error;
            }

            if (ast_peek_token(&state)) {
                error_0204_unexpected_token(
                    lexer->source,
                    ast_token_span(&state, &state.token),
                    state.token.kind,
                    "Expected a symbol to start a binding");
            } else {
                AstToken eof = {
                    .kind        = TK_EOF,
                    .source      = lexer->source,
                    .offset      = lexer->source.source.count,
                    .token_index = (u32)array_count(lexer->tokens),
                };
                error_0204_unexpected_token(lexer->source,
                                            ast_token_span(&state, &eof),
                                            eof.kind,
                                            "Expected a symbol to start a "
                                            "binding");
            }
            goto error;
        }
    }

    return (Ast){.nodes = state.nodes};

error:
    ast_done(&(Ast){.nodes = state.nodes});
    return (Ast){0};
}

//------------------------------------------------------------------------------
// Free the parser's AST node table.

void ast_done(Ast* ast) { array_free(ast->nodes); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
