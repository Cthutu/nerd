//------------------------------------------------------------------------------
// AST expression parsing
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>

//------------------------------------------------------------------------------

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

bool ast_token_starts_expression(TokenKind kind)
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

bool ast_infix_binding_power(TokenKind kind, u8* out_left_bp, u8* out_right_bp)
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
        return error_0201_missing_value(
            token.source, ast_token_span(state, &token), token.kind);
    }
}

internal bool
ast_parse_led(AstParseState* state, AstToken op, u32 left_node, u32* out_node)
{
    u8  left_bp  = 0;
    u8  right_bp = 0;
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

bool ast_parse_expr(AstParseState* state, u32* out_expr_node)
{
    u32 root_node;
    if (!ast_parse_expr_bp(state, 0, &root_node)) {
        return false;
    }

    AstToken expr_token = {
        .kind   = state->lexer->tokens[state->expr_start_token_index].kind,
        .source = state->lexer->source,
        .offset = state->lexer->tokens[state->expr_start_token_index].offset,
        .token_index = state->expr_start_token_index,
    };

    AstNode expr_node = {
        .kind        = AK_Expression,
        .token_index = state->expr_start_token_index,
        .a           = root_node,
    };
    return ast_emit_node(state, expr_node, expr_token, out_expr_node);
}

bool ast_parse_expr_bp(AstParseState* state, u8 min_bp, u32* out_node)
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
