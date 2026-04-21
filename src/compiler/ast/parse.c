//------------------------------------------------------------------------------
// Parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>

ParsingQuery ast_parsing_query_for_token(TokenKind kind)
{
    switch (kind) {
    case TK_Symbol:
    case TK_Integer:
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
    if (!ast_expect_token(state, TK_FatArrow)) {
        return false;
    }

    u32 fn_start_index;
    u32 fn_end_index;
    u32 fn_def_index;

    EMIT_NODE(AK_FnStart, 0, 0, 0, fn_start_index);

    if (!ast_parse_expr(state, nullptr)) {
        return false;
    }

    EMIT_NODE(AK_FnEnd, 0, 0, fn_start_index, fn_end_index);
    state->nodes[fn_start_index].b = fn_end_index;

    EMIT_NODE(AK_FnDef, fn_token.token_index, fn_start_index, 0, fn_def_index);

    if (out_node) {
        *out_node = fn_def_index;
    }

    return true;
}

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

    if (!ast_expect_token(state, TK_Colon)) {
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
            "Expected a declaration or expression after '::', but found end of "
            "file");
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

    node = (AstNode){
        .kind        = AK_Bind,
        .token_index = bind_token.token_index,
        .a           = bind_token.value.symbol_handle,
        .b           = expr_index,
    };

    return ast_emit_node(state, node, out_node);
}

Ast ast_parse(Lexer* lexer)
{
    AstParseState state = {
        .lexer         = lexer,
        .token_index   = 0,
        .integer_index = 0,
        .symbol_index  = 0,
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
            error_0204_unexpected_token(lexer->source,
                                        ast_token_span(&state, &token),
                                        token.kind,
                                        "Expected a symbol to start a binding");
            goto error;
        }
    }

    return (Ast){.nodes = state.nodes};

error:
    ast_done(&(Ast){.nodes = state.nodes});
    return (Ast){0};
}

//------------------------------------------------------------------------------

void ast_done(Ast* ast) { array_free(ast->nodes); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
