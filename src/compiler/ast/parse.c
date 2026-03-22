//------------------------------------------------------------------------------
// Parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/ast.h>
#include <compiler/compiler.h>
#include <compiler/error/error.h>
#include <limits.h>

//------------------------------------------------------------------------------

#define AST_BP_ADDITIVE 10
#define AST_BP_MULTIPLICATIVE 20
#define AST_BP_PREFIX 30

typedef struct {
    TokenKind  kind;
    NerdSource source;
    u32        offset;
    u32        token_index;

    union {
        u32 integer_index;
    } value;
} AstToken;

typedef struct {
    Lexer* lexer;
    u32    token_index;
    u32    integer_index;

    // Current token (from last peek or next)
    AstToken token;

    // Output
    Array(AstNode) nodes;

    // Current top-level expression start
    u32 expr_start_node_index;
    u32 expr_start_token_index;
} AstParseState;

bool ast_peek_token(AstParseState* state, AstToken* out_token);
bool ast_next_token(AstParseState* state, AstToken* out_token);

internal ErrorSpan ast_token_span(const AstParseState* state, const AstToken* token)
{
    usize end = token->offset + 1;

    if (token->kind == TK_EOF) {
        end = token->source.source.count;
    } else if (token->token_index < array_count(state->lexer->tokens)) {
        const Token* lex_token = &state->lexer->tokens[token->token_index];
        end                    = lex_token_end_offset(state->lexer, lex_token);
    }

    return (ErrorSpan){.start = token->offset, .end = end};
}

internal bool ast_expression_ref(AstParseState* state,
                                 u32            node_index,
                                 AstToken       token,
                                 i16*           out_ref)
{
    isize delta =
        (isize)state->expr_start_node_index - (isize)node_index;
    if (delta < INT16_MIN || delta > INT16_MAX) {
        return error_0200_code_too_complex(
            token.source, ast_token_span(state, &token));
    }

    *out_ref = (i16)delta;
    return true;
}

internal bool
ast_emit_node(AstParseState* state, AstNode node, AstToken token, u32* out_index)
{
    u32 node_index = (u32)array_count(state->nodes);
    if (!ast_expression_ref(state, node_index, token, &node.ref)) {
        return false;
    }

    array_push(state->nodes, node);
    *out_index = node_index;
    return true;
}

internal bool ast_token_starts_expression(TokenKind kind)
{
    switch (kind) {
    case TK_Integer:
    case TK_Minus:
    case TK_LParen:
        return true;
    default:
        return false;
    }
}

internal bool
ast_infix_binding_power(TokenKind kind, u8* out_left_bp, u8* out_right_bp)
{
    switch (kind) {
    case TK_Plus:
    case TK_Minus:
        *out_left_bp  = AST_BP_ADDITIVE;
        *out_right_bp = AST_BP_ADDITIVE + 1;
        return true;
    case TK_Star:
    case TK_Slash:
    case TK_Percent:
        *out_left_bp  = AST_BP_MULTIPLICATIVE;
        *out_right_bp = AST_BP_MULTIPLICATIVE + 1;
        return true;
    default:
        return false;
    }
}

internal bool ast_expect_token(AstParseState* state, TokenKind expected_kind)
{
    AstToken token;
    if (!ast_next_token(state, &token)) {
        AstToken eof = {
            .kind       = TK_EOF,
            .source     = state->lexer->source,
            .offset     = state->lexer->source.source.count,
            .token_index = (u32)array_count(state->lexer->tokens),
        };
        return error_0203_expected_token(
            eof.source, ast_token_span(state, &eof), expected_kind, eof.kind);
    }

    if (token.kind != expected_kind) {
        return error_0203_expected_token(
            token.source, ast_token_span(state, &token), expected_kind, token.kind);
    }

    return true;
}

internal bool ast_parse_expr_bp(AstParseState* state, u8 min_bp, u32* out_node);

internal AstKind ast_binary_kind_from_token(TokenKind kind)
{
    switch (kind) {
    case TK_Plus:
        return AK_IntegerPlus;
    case TK_Minus:
        return AK_IntegerMinus;
    case TK_Star:
        return AK_IntegerMultiply;
    case TK_Slash:
        return AK_IntegerDivide;
    case TK_Percent:
        return AK_IntegerModulo;
    default:
        error_ice("Unhandled binary token kind: %d", kind);
        return AK_IntegerPlus;
    }
}

// Pratt parsing terminology:
//
// - `nud` ("null denotation") parses a token that starts an expression, such
//   as an integer literal, a prefix unary operator, or a parenthesized group.
// - `led` ("left denotation") parses a token that continues an expression
//   after a left-hand side already exists, such as a binary operator.
//
// Example:
// - In `-2`, `-` is handled by `nud`.
// - In `1 - 2`, `-` is handled by `led`.

internal bool ast_parse_nud(AstParseState* state, AstToken token, u32* out_node)
{
    switch (token.kind) {
    case TK_Integer:
        {
            AstNode node = {
                .kind        = AK_IntegerLiteral,
                .token_index = token.token_index,
                .a           = token.value.integer_index,
            };
            node.ref = 0;
            return ast_emit_node(state, node, token, out_node);
        }
    case TK_Minus:
        {
            u32 rhs;
            if (!ast_parse_expr_bp(state, AST_BP_PREFIX, &rhs)) {
                return false;
            }

            AstNode node = {
                .kind        = AK_IntegerNegate,
                .token_index = token.token_index,
                .a           = rhs,
            };
            return ast_emit_node(state, node, token, out_node);
        }
    case TK_LParen:
        if (!ast_parse_expr_bp(state, 0, out_node)) {
            return false;
        }
        return ast_expect_token(state, TK_RParen);
    default:
        return error_0201_missing_value(token.source,
                                        ast_token_span(state, &token),
                                        token.kind);
    }
}

internal bool ast_parse_led(AstParseState* state,
                            AstToken       op,
                            u32            left_node,
                            u32*           out_node)
{
    u8 left_bp  = 0;
    u8 right_bp = 0;
    u32 right_node;

    if (!ast_infix_binding_power(op.kind, &left_bp, &right_bp)) {
        error_ice("Unhandled led token kind: %d", op.kind);
    }

    if (!ast_parse_expr_bp(state, right_bp, &right_node)) {
        return false;
    }

    AstNode node = {
        .kind        = ast_binary_kind_from_token(op.kind),
        .token_index = op.token_index,
        .a           = left_node,
        .b           = right_node,
    };
    return ast_emit_node(state, node, op, out_node);
}

bool ast_peek_token(AstParseState* state, AstToken* out_token)
{
    // Check of EOF
    if (state->token_index >= array_count(state->lexer->tokens)) {
        *out_token = (AstToken){
            .kind        = TK_EOF,
            .source      = state->lexer->source,
            .offset      = state->lexer->source.source.count,
            .token_index = (u32)array_count(state->lexer->tokens),
        };
        return false;
    }

    Token      token  = state->lexer->tokens[state->token_index];
    NerdSource source = state->lexer->source;

    switch (token.kind) {
    case TK_Integer:
        *out_token = (AstToken){
            .kind                = TK_Integer,
            .source              = source,
            .offset              = token.offset,
            .token_index         = state->token_index,
            .value.integer_index = state->integer_index,
        };
        return true;

    default:
        *out_token = (AstToken){
            .kind        = token.kind,
            .source      = source,
            .offset      = token.offset,
            .token_index = state->token_index,
        };
        return true;
    }
}

bool ast_next_token(AstParseState* state, AstToken* out_token)
{
    if (!ast_peek_token(state, out_token)) {
        return false;
    }

    // Advance the token index and integer index if necessary
    state->token_index++;
    if (out_token->kind == TK_Integer) {
        state->integer_index++;
    }

    return true;
}

bool ast_parse_expr(AstParseState* state, u32* out_expr_node)
{
    u32 root_node;
    if (!ast_parse_expr_bp(state, 0, &root_node)) {
        return false;
    }

    AstToken expr_token = {
        .kind        = state->lexer->tokens[state->expr_start_token_index].kind,
        .source      = state->lexer->source,
        .offset      = state->lexer->tokens[state->expr_start_token_index].offset,
        .token_index = state->expr_start_token_index,
    };

    AstNode expr_node = {
        .kind        = AK_Expression,
        .token_index = state->expr_start_token_index,
        .a           = root_node,
    };
    return ast_emit_node(state, expr_node, expr_token, out_expr_node);
}

internal bool ast_parse_expr_bp(AstParseState* state, u8 min_bp, u32* out_node)
{
    AstToken token;
    u32      left_node;

    if (!ast_next_token(state, &token)) {
        AstToken eof = {
            .kind        = TK_EOF,
            .source      = state->lexer->source,
            .offset      = state->lexer->source.source.count,
            .token_index = (u32)array_count(state->lexer->tokens),
        };
        return error_0201_missing_value(
            eof.source, ast_token_span(state, &eof), eof.kind);
    }

    if (!ast_parse_nud(state, token, &left_node)) {
        return false;
    }

    for (;;) {
        AstToken next;
        u8       left_bp;
        u8       right_bp;

        if (!ast_peek_token(state, &next)) {
            break;
        }

        if (!ast_infix_binding_power(next.kind, &left_bp, &right_bp)) {
            if (ast_token_starts_expression(next.kind)) {
                return error_0202_missing_operator(
                    next.source, ast_token_span(state, &next), next.kind);
            }
            break;
        }

        if (left_bp < min_bp) {
            break;
        }

        if (!ast_next_token(state, &next)) {
            break;
        }

        if (!ast_parse_led(state, next, left_node, &left_node)) {
            return false;
        }
    }

    *out_node = left_node;
    return true;
}

Ast ast_parse(Lexer* lexer)
{
    AstParseState state = {
         .lexer         = lexer,
         .token_index   = 0,
         .integer_index = 0,
         .token =
            (AstToken){
                 .kind   = TK_EOF,
                 .source = lexer->source,
                 .offset = 0,
            },
         .nodes                 = 0,
         .expr_start_node_index = 0,
         .expr_start_token_index = 0,
    };

    if (array_count(lexer->tokens) == 0) {
        return (Ast){0};
    }

    state.expr_start_node_index  = (u32)array_count(state.nodes);
    state.expr_start_token_index = state.token_index;

    u32 expr_index;
    if (!ast_parse_expr(&state, &expr_index)) {
        ast_done(&(Ast){.nodes = state.nodes});
        return (Ast){0};
    }

    if (state.token_index < array_count(lexer->tokens)) {
        AstToken token;
        ast_peek_token(&state, &token);
        ast_done(&(Ast){.nodes = state.nodes});
        error_0204_unexpected_token(
            token.source, ast_token_span(&state, &token), token.kind);
        return (Ast){0};
    }

    return (Ast){.nodes = state.nodes};
}

//------------------------------------------------------------------------------

void ast_done(Ast* ast) { array_free(ast->nodes); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
