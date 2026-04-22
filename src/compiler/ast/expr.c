//------------------------------------------------------------------------------
// AST expression parsing
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>

//------------------------------------------------------------------------------
// Map a token kind to the corresponding binary AST node kind.

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

//------------------------------------------------------------------------------
// Report whether a token can begin an expression.

bool ast_token_starts_expression(TokenKind kind)
{
    switch (kind) {
    case TK_Integer:
    case TK_String:
    case TK_Symbol:
    case TK_Minus:
    case TK_LParen:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Return Pratt binding powers for supported infix operators.

bool ast_infix_binding_power(TokenKind kind, u8* out_left_bp, u8* out_right_bp)
{
    switch (kind) {
    case TK_LParen:
        *out_left_bp  = AST_BP_POSTFIX;
        *out_right_bp = AST_BP_POSTFIX;
        return true;
    case TK_Dot:
        *out_left_bp  = AST_BP_POSTFIX;
        *out_right_bp = AST_BP_POSTFIX;
        return true;
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

//------------------------------------------------------------------------------
// Report whether the upcoming token sequence starts a new top-level binding.

internal bool ast_next_tokens_start_binding(const AstParseState* state)
{
    if (state->token_index + 1 >= array_count(state->lexer->tokens)) {
        return false;
    }

    const Token* tokens = state->lexer->tokens;
    return tokens[state->token_index].kind == TK_Symbol &&
           tokens[state->token_index + 1].kind == TK_Colon;
}

//------------------------------------------------------------------------------
// Return whether one AST node already represents a string-literal chain.

internal bool ast_node_is_stringish(const AstParseState* state, u32 node_index)
{
    switch (state->nodes[node_index].kind) {
    case AK_StringLiteral:
    case AK_StringConcat:
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

//------------------------------------------------------------------------------
// Parse the current token as the start of an expression fragment.

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
            return ast_emit_node(state, node, out_node);
        }
    case TK_String:
        {
            AstNode node = {
                .kind        = AK_StringLiteral,
                .token_index = token.token_index,
                .a           = token.value.string_index,
            };
            return ast_emit_node(state, node, out_node);
        }
    case TK_Symbol:
        {
            AstNode node = {
                .kind        = AK_SymbolRef,
                .token_index = token.token_index,
                .a           = token.value.symbol_handle,
            };
            return ast_emit_node(state, node, out_node);
        }
    case TK_Minus:
        {
            u32 rhs;
            if (!ast_next_token(state)) {
                return error_0201_missing_value(
                    state->token.source,
                    ast_token_span(state, &state->token),
                    state->token.kind);
            }
            if (!ast_parse_expr_bp(state, AST_BP_PREFIX, &rhs)) {
                return false;
            }

            AstNode node = {
                .kind        = AK_IntegerNegate,
                .token_index = token.token_index,
                .a           = rhs,
            };
            return ast_emit_node(state, node, out_node);
        }
    case TK_LParen:
        if (!ast_next_token(state)) {
            return error_0201_missing_value(
                state->token.source,
                ast_token_span(state, &state->token),
                state->token.kind);
        }
        if (!ast_parse_expr_bp(state, 0, out_node)) {
            return false;
        }
        return ast_expect_token(state, TK_RParen);
    default:
        return error_0201_missing_value(
            token.source, ast_token_span(state, &token), token.kind);
    }
}

//------------------------------------------------------------------------------
// Parse the current token as an infix operator that extends an existing lhs.

internal bool
ast_parse_led(AstParseState* state, AstToken op, u32 left_node, u32* out_node)
{
    u8  left_bp    = 0;
    u8  right_bp   = 0;
    u32 right_node = 0;

    if (!ast_infix_binding_power(op.kind, &left_bp, &right_bp)) {
        error_ice("Unhandled led token kind: %d", op.kind);
    }

    if (op.kind == TK_LParen) {
        if (!ast_next_token(state)) {
            return error_0201_missing_value(
                state->token.source,
                ast_token_span(state, &state->token),
                state->token.kind);
        }
        if (!ast_parse_expr_bp(state, 0, &right_node)) {
            return false;
        }
        if (!ast_expect_token(state, TK_RParen)) {
            return false;
        }

        AstNode node = {
            .kind        = AK_Call,
            .token_index = op.token_index,
            .a           = left_node,
            .b           = right_node,
        };
        return ast_emit_node(state, node, out_node);
    }

    if (op.kind == TK_Dot) {
        if (!ast_next_token(state)) {
            return error_0203_expected_token(state->lexer->source,
                                             ast_token_span(state, &op),
                                             TK_Symbol,
                                             TK_EOF);
        }
        if (state->token.kind != TK_Symbol ||
            !string_eq(
                lex_symbol(state->lexer, state->token.value.symbol_handle),
                s("cast"))) {
            return error_0203_expected_token(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_Symbol,
                state->token.kind);
        }
        if (!ast_expect_token(state, TK_LParen) || !ast_next_token(state)) {
            return false;
        }

        u32 type_node = 0;
        if (!ast_parse_type(state, &type_node)) {
            return false;
        }
        if (!ast_expect_token(state, TK_RParen)) {
            return false;
        }

        AstNode node = {
            .kind        = AK_Cast,
            .token_index = op.token_index,
            .a           = left_node,
            .b           = type_node,
        };
        return ast_emit_node(state, node, out_node);
    }

    if (!ast_next_token(state)) {
        return error_0201_missing_value(state->token.source,
                                        ast_token_span(state, &state->token),
                                        state->token.kind);
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
    return ast_emit_node(state, node, out_node);
}

//------------------------------------------------------------------------------
// Parse an expression starting at the current token and wrap it in
// AK_Expression.

bool ast_parse_expr(AstParseState* state, u32* out_expr_node)
{
    u32 root_node;
    if (!ast_parse_expr_bp(state, 0, &root_node)) {
        return false;
    }

    AstNode expr_node = {
        .kind        = AK_Expression,
        .token_index = state->start_token_index,
        .a           = root_node,
    };
    return ast_emit_node(state, expr_node, out_expr_node);
}

//------------------------------------------------------------------------------
// Parse an expression with the current token as the first token of the lhs.

bool ast_parse_expr_bp(AstParseState* state, u8 min_bp, u32* out_node)
{
    u32 left_node;

    if (!ast_parse_nud(state, state->token, &left_node)) {
        return false;
    }

    for (;;) {
        AstToken next;
        u8       left_bp;
        u8       right_bp;

        if (!ast_peek_token(state)) {
            break;
        }
        next = state->token;

        if (!ast_infix_binding_power(next.kind, &left_bp, &right_bp)) {
            if (ast_next_tokens_start_binding(state)) {
                break;
            }
            if (next.kind == TK_String &&
                ast_node_is_stringish(state, left_node)) {
                u32 right_node = 0;
                if (!ast_next_token(state)) {
                    return error_0201_missing_value(
                        next.source, ast_token_span(state, &next), next.kind);
                }
                if (!ast_parse_nud(state, state->token, &right_node)) {
                    return false;
                }

                AstNode node = {
                    .kind        = AK_StringConcat,
                    .token_index = next.token_index,
                    .a           = left_node,
                    .b           = right_node,
                };
                if (!ast_emit_node(state, node, &left_node)) {
                    return false;
                }
                continue;
            }
            if (state->allow_statement_boundary &&
                ast_token_starts_expression(next.kind)) {
                break;
            }
            if (ast_token_starts_expression(next.kind)) {
                return error_0202_missing_operator(
                    next.source, ast_token_span(state, &next), next.kind);
            }
            break;
        }

        if (left_bp < min_bp) {
            break;
        }

        if (!ast_next_token(state)) {
            return error_0201_missing_value(
                state->token.source,
                ast_token_span(state, &state->token),
                state->token.kind);
        }
        if (!ast_parse_led(state, state->token, left_node, &left_node)) {
            return false;
        }
    }

    *out_node = left_node;
    return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
