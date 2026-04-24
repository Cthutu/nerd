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
    case TK_Amp:
        return AK_BitwiseAnd;
    case TK_Caret:
        return AK_BitwiseXor;
    case TK_Pipe:
        return AK_BitwiseOr;
    case TK_EqualEqual:
        return AK_Equal;
    case TK_BangEqual:
        return AK_NotEqual;
    case TK_Less:
        return AK_Less;
    case TK_LessEqual:
        return AK_LessEqual;
    case TK_Greater:
        return AK_Greater;
    case TK_GreaterEqual:
        return AK_GreaterEqual;
    case TK_AmpAmp:
        return AK_LogicalAnd;
    case TK_PipePipe:
        return AK_LogicalOr;
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
    case TK_Float:
    case TK_String:
    case TK_InterpolatedStringStart:
    case TK_true:
    case TK_false:
    case TK_Symbol:
    case TK_Bang:
    case TK_Minus:
    case TK_LParen:
    case TK_fn:
    case TK_on:
    case TK_for:
    case TK_Dollar:
        return true;
    default:
        return false;
    }
}

internal TokenKind ast_expr_cursor_kind(const AstParseState* state)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        return TK_EOF;
    }
    return state->lexer->tokens[state->token_index].kind;
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
    case TK_Less:
    case TK_LessEqual:
    case TK_Greater:
    case TK_GreaterEqual:
        *out_left_bp  = AST_BP_COMPARISON;
        *out_right_bp = AST_BP_COMPARISON + 1;
        return true;
    case TK_EqualEqual:
    case TK_BangEqual:
        *out_left_bp  = AST_BP_EQUALITY;
        *out_right_bp = AST_BP_EQUALITY + 1;
        return true;
    case TK_Amp:
        *out_left_bp  = AST_BP_BITWISE_AND;
        *out_right_bp = AST_BP_BITWISE_AND + 1;
        return true;
    case TK_Caret:
        *out_left_bp  = AST_BP_BITWISE_XOR;
        *out_right_bp = AST_BP_BITWISE_XOR + 1;
        return true;
    case TK_Pipe:
        *out_left_bp  = AST_BP_BITWISE_OR;
        *out_right_bp = AST_BP_BITWISE_OR + 1;
        return true;
    case TK_AmpAmp:
        *out_left_bp  = AST_BP_LOGICAL_AND;
        *out_right_bp = AST_BP_LOGICAL_AND + 1;
        return true;
    case TK_PipePipe:
        *out_left_bp  = AST_BP_LOGICAL_OR;
        *out_right_bp = AST_BP_LOGICAL_OR + 1;
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
    case AK_InterpolatedString:
        return true;
    default:
        return false;
    }
}

internal bool ast_parse_on_branch_pattern(AstParseState* state, u32* out_node);

internal bool ast_parse_interpolated_string(AstParseState* state,
                                            AstToken       start_token,
                                            u32*           out_node)
{
    typedef struct {
        bool is_expr;
        u32  token_index;
        u32  payload;
    } InterpPart;

    Array(InterpPart) parts = NULL;

    for (;;) {
        if (!ast_peek_token(state)) {
            array_free(parts);
            return error_0106_unterminated_string_literal(
                state->lexer->source,
                (ErrorSpan){.start = start_token.offset,
                            .end   = state->lexer->source.source.count});
        }

        if (state->token.kind == TK_InterpolatedStringEnd) {
            if (!ast_next_token(state)) {
                array_free(parts);
                return false;
            }

            u32 part_start = (u32)array_count(state->nodes);
            for (u32 i = 0; i < array_count(parts); ++i) {
                AstNode part_node = {
                    .kind =
                        parts[i].is_expr ? AK_InterpPartExpr : AK_StringLiteral,
                    .token_index = parts[i].token_index,
                    .a           = parts[i].payload,
                };
                if (!ast_emit_node(state, part_node, NULL)) {
                    array_free(parts);
                    return false;
                }
            }

            AstNode node = {
                .kind        = AK_InterpolatedString,
                .token_index = start_token.token_index,
                .a           = part_start,
                .b           = (u32)array_count(state->nodes),
            };
            array_free(parts);
            return ast_emit_node(state, node, out_node);
        }

        if (state->token.kind == TK_String) {
            AstToken part_token = state->token;
            if (!ast_next_token(state)) {
                array_free(parts);
                return false;
            }

            array_push(parts,
                       ((InterpPart){
                           .is_expr     = false,
                           .token_index = part_token.token_index,
                           .payload     = part_token.value.string_index,
                       }));
            continue;
        }

        if (state->token.kind == TK_LBrace) {
            u32 expr_root = 0;
            if (!ast_next_token(state)) {
                array_free(parts);
                return false;
            }
            if (!ast_next_token(state)) {
                array_free(parts);
                return error_0201_missing_value(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_RBrace);
            }
            if (!ast_parse_expr_bp(state, 0, &expr_root)) {
                array_free(parts);
                return false;
            }
            if (!ast_expect_token(state, TK_RBrace)) {
                array_free(parts);
                return false;
            }

            array_push(parts,
                       ((InterpPart){
                           .is_expr     = true,
                           .token_index = start_token.token_index,
                           .payload     = expr_root,
                       }));
            continue;
        }

        array_free(parts);
        return error_0204_unexpected_token(
            state->lexer->source,
            ast_token_span(state, &state->token),
            state->token.kind,
            "Continue the interpolated string or close it with `\"`.");
    }
}

internal bool ast_parse_on_branch_expr(AstParseState* state, u32* out_node)
{
    if (state->token.kind == TK_return) {
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_ReturnExpr,
                                 .token_index = state->token.token_index,
                                 .a           = U32_MAX,
                             },
                             out_node);
    }

    if (state->token.kind == TK_break || state->token.kind == TK_continue) {
        AstKind kind =
            state->token.kind == TK_break ? AK_BreakExpr : AK_ContinueExpr;
        u32 token_index = state->token.token_index;
        u32 label       = U32_MAX;
        if (ast_expr_cursor_kind(state) == TK_Dollar) {
            if (!ast_next_token(state) || !ast_next_token(state) ||
                state->token.kind != TK_Symbol) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_Symbol,
                    state->token.kind);
            }
            label = state->token.value.symbol_handle;
        }
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = kind,
                                 .token_index = token_index,
                                 .a           = U32_MAX,
                                 .b           = label,
                             },
                             out_node);
    }

    return ast_parse_expr_bp(state, 0, out_node);
}

internal bool
ast_parse_on_expr(AstParseState* state, AstToken on_token, u32* out_node)
{
    if (!ast_next_token(state)) {
        return error_0201_missing_value(
            state->token.source, ast_token_span(state, &on_token), TK_FatArrow);
    }

    u32 condition_node = 0;
    if (!ast_parse_expr_bp(state, 0, &condition_node)) {
        return false;
    }

    u32 first_branch = (u32)array_count(state->on_branches);

    if (state->token.kind == TK_LBrace) {
        if (!ast_next_token(state) || !ast_next_token(state)) {
            return error_0201_missing_value(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_else);
        }

        while (state->token.kind != TK_RBrace) {
            if (state->token.kind == TK_EOF) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_RBrace,
                    state->token.kind);
            }

            if (state->token.token_index == state->token_index &&
                !ast_next_token(state)) {
                return error_0201_missing_value(
                    state->token.source,
                    ast_token_span(state, &state->token),
                    TK_RBrace);
            }

            AstOnBranch branch          = {0};
            branch.binder_symbol_handle = U32_MAX;
            branch.binder_token_index   = U32_MAX;
            if (state->token.kind == TK_Symbol &&
                ast_peek_kind_at(state, 0) == TK_At) {
                branch.binder_symbol_handle = state->token.value.symbol_handle;
                branch.binder_token_index   = state->token.token_index;
                if (!ast_next_token(state) || state->token.kind != TK_At ||
                    !ast_next_token(state)) {
                    return error_0201_missing_value(
                        state->token.source,
                        ast_token_span(state, &state->token),
                        TK_RBrace);
                }
            }
            if (state->token.kind == TK_else) {
                branch.flags = AOBF_Else;
                if (!ast_next_token(state) ||
                    state->token.kind != TK_FatArrow ||
                    !ast_next_token(state)) {
                    return error_0201_missing_value(
                        state->token.source,
                        ast_token_span(state, &state->token),
                        TK_RBrace);
                }
            } else {
                branch.pattern_node_index =
                    (u32)array_count(state->on_pattern_nodes);
                branch.pattern_count = 0;
                for (;;) {
                    u32 pattern_root = 0;
                    if (!ast_parse_on_branch_pattern(state, &pattern_root)) {
                        return false;
                    }
                    array_push(state->on_pattern_nodes, pattern_root);
                    ++branch.pattern_count;
                    if (state->token.kind != TK_Comma) {
                        break;
                    }
                    if (!ast_next_token(state) || !ast_next_token(state)) {
                        return error_0201_missing_value(
                            state->token.source,
                            ast_token_span(state, &state->token),
                            TK_FatArrow);
                    }
                }
                if (state->token.kind != TK_FatArrow) {
                    return error_0203_expected_token(
                        state->lexer->source,
                        ast_token_span(state, &state->token),
                        TK_FatArrow,
                        state->token.kind);
                }
                if (!ast_next_token(state) ||
                    state->token.kind != TK_FatArrow ||
                    !ast_next_token(state)) {
                    return error_0201_missing_value(
                        state->token.source,
                        ast_token_span(state, &state->token),
                        TK_RBrace);
                }
            }

            bool saved_statement_boundary   = state->allow_statement_boundary;
            state->allow_statement_boundary = true;
            bool parsed_branch_expr =
                ast_parse_on_branch_expr(state, &branch.expr_node_index);
            state->allow_statement_boundary = saved_statement_boundary;
            if (!parsed_branch_expr) {
                return false;
            }

            array_push(state->on_branches, branch);

            if (branch.flags & AOBF_Else) {
                break;
            }
        }

        if (state->token.kind != TK_RBrace) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_RBrace,
                state->token.kind);
        }
        if (!ast_expect_token(state, TK_RBrace)) {
            return false;
        }

        u32 on_index = (u32)array_count(state->ons);
        array_push(state->ons,
                   (AstOnInfo){
                       .kind         = AOK_Value,
                       .first_branch = first_branch,
                       .branch_count =
                           (u32)array_count(state->on_branches) - first_branch,
                   });

        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_On,
                                 .token_index = on_token.token_index,
                                 .a           = condition_node,
                                 .b           = on_index,
                             },
                             out_node);
    }

    if (state->token.kind != TK_FatArrow) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_FatArrow,
                                         state->token.kind);
    }

    if (!ast_next_token(state) || !ast_next_token(state)) {
        return error_0201_missing_value(
            state->token.source, ast_token_span(state, &state->token), TK_else);
    }

    u32  true_expr_node             = 0;
    bool saved_statement_boundary   = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    bool parsed_true_expr = ast_parse_on_branch_expr(state, &true_expr_node);
    state->allow_statement_boundary = saved_statement_boundary;
    if (!parsed_true_expr) {
        return false;
    }

    if (state->token.kind != TK_else) {
        if (state->allow_statement_boundary) {
            u32 true_pattern = 0;
            if (!ast_emit_node(state,
                               (AstNode){
                                   .kind        = AK_BoolLiteral,
                                   .token_index = on_token.token_index,
                                   .a           = 1,
                               },
                               &true_pattern)) {
                return false;
            }
            array_push(state->on_branches,
                       (AstOnBranch){
                           .pattern_node_index   = true_pattern,
                           .expr_node_index      = true_expr_node,
                           .flags                = AOBF_None,
                           .binder_symbol_handle = U32_MAX,
                           .binder_token_index   = U32_MAX,
                       });

            u32 on_index = (u32)array_count(state->ons);
            array_push(state->ons,
                       (AstOnInfo){
                           .kind         = AOK_Bool,
                           .first_branch = first_branch,
                           .branch_count = 1,
                       });

            return ast_emit_node(state,
                                 (AstNode){
                                     .kind        = AK_On,
                                     .token_index = on_token.token_index,
                                     .a           = condition_node,
                                     .b           = on_index,
                                 },
                                 out_node);
        }
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_else,
                                         state->token.kind);
    }

    if (!ast_next_token(state) || !ast_next_token(state)) {
        return error_0201_missing_value(
            state->token.source, ast_token_span(state, &state->token), TK_EOF);
    }

    u32 false_expr_node = 0;
    if (!ast_parse_expr_bp(state, 0, &false_expr_node)) {
        return false;
    }

    u32 true_pattern = 0;
    if (!ast_emit_node(state,
                       (AstNode){
                           .kind        = AK_BoolLiteral,
                           .token_index = on_token.token_index,
                           .a           = 1,
                       },
                       &true_pattern)) {
        return false;
    }
    array_push(state->on_branches,
               (AstOnBranch){
                   .pattern_node_index   = true_pattern,
                   .expr_node_index      = true_expr_node,
                   .flags                = AOBF_None,
                   .binder_symbol_handle = U32_MAX,
                   .binder_token_index   = U32_MAX,
               });
    array_push(state->on_branches,
               (AstOnBranch){
                   .expr_node_index      = false_expr_node,
                   .flags                = AOBF_Else,
                   .binder_symbol_handle = U32_MAX,
                   .binder_token_index   = U32_MAX,
               });

    u32 on_index = (u32)array_count(state->ons);
    array_push(state->ons,
               (AstOnInfo){
                   .kind         = AOK_Bool,
                   .first_branch = first_branch,
                   .branch_count = 2,
               });

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_On,
                             .token_index = on_token.token_index,
                             .a           = condition_node,
                             .b           = on_index,
                         },
                         out_node);
}

internal bool ast_parse_on_branch_pattern(AstParseState* state, u32* out_node)
{
    u32 start_node = 0;
    if (!ast_parse_expr_bp(state, 0, &start_node)) {
        return false;
    }

    AstKind range_kind = AK_IntegerPlus;
    if (state->token.kind == TK_RangeExclusive) {
        range_kind = AK_RangeExclusive;
    } else if (state->token.kind == TK_RangeInclusive) {
        range_kind = AK_RangeInclusive;
    } else {
        *out_node = start_node;
        return true;
    }

    AstToken range_token = state->token;
    if (!ast_next_token(state) || !ast_next_token(state)) {
        return error_0201_missing_value(state->token.source,
                                        ast_token_span(state, &range_token),
                                        TK_Integer);
    }

    u32 end_node = 0;
    if (!ast_parse_expr_bp(state, 0, &end_node)) {
        return false;
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = range_kind,
                             .token_index = range_token.token_index,
                             .a           = start_node,
                             .b           = end_node,
                         },
                         out_node);
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
    case TK_Float:
        {
            AstNode node = {
                .kind        = AK_FloatLiteral,
                .token_index = token.token_index,
                .a           = token.value.float_index,
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
    case TK_true:
    case TK_false:
        {
            AstNode node = {
                .kind        = AK_BoolLiteral,
                .token_index = token.token_index,
                .a           = token.kind == TK_true ? 1u : 0u,
            };
            return ast_emit_node(state, node, out_node);
        }
    case TK_InterpolatedStringStart:
        return ast_parse_interpolated_string(state, token, out_node);
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
    case TK_Bang:
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
                .kind =
                    token.kind == TK_Bang ? AK_LogicalNot : AK_IntegerNegate,
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
    case TK_fn:
        return ast_parse_declaration(state, out_node);
    case TK_on:
        return ast_parse_on_expr(state, token, out_node);
    case TK_for:
        return ast_parse_for(state, U32_MAX, out_node);
    case TK_Dollar:
        {
            u32 label = U32_MAX;
            if (!ast_next_token(state)) {
                return error_0203_expected_token(state->lexer->source,
                                                 ast_token_span(state, &token),
                                                 TK_LBrace,
                                                 state->token.kind);
            }
            if (state->token.kind == TK_Symbol) {
                label = state->token.value.symbol_handle;
                if (!ast_next_token(state)) {
                    return error_0203_expected_token(
                        state->lexer->source,
                        ast_token_span(state, &state->token),
                        TK_LBrace,
                        state->token.kind);
                }
            }
            if (state->token.kind == TK_for) {
                return ast_parse_for(state, label, out_node);
            }
            if (state->token.kind != TK_LBrace) {
                return error_0203_expected_token(state->lexer->source,
                                                 ast_token_span(state, &token),
                                                 TK_LBrace,
                                                 state->token.kind);
            }
            u32 block_node = 0;
            if (!ast_parse_nested_block(state, &block_node)) {
                return false;
            }
            return ast_emit_node(state,
                                 (AstNode){
                                     .kind        = AK_ExprBlock,
                                     .token_index = token.token_index,
                                     .a           = block_node,
                                     .b           = label,
                                 },
                                 out_node);
        }
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

        Array(u32) arg_nodes = NULL;
        if (state->token.kind != TK_RParen) {
            for (;;) {
                right_node = 0;
                if (!ast_parse_expr_bp(state, 0, &right_node)) {
                    array_free(arg_nodes);
                    return false;
                }
                array_push(arg_nodes, right_node);
                if (state->token.kind == TK_Comma) {
                    if (!ast_next_token(state) || !ast_next_token(state)) {
                        array_free(arg_nodes);
                        return error_0201_missing_value(
                            state->token.source,
                            ast_token_span(state, &state->token),
                            TK_RParen);
                    }
                    continue;
                }
                if (ast_peek_kind_at(state, 0) == TK_Comma) {
                    if (!ast_expect_token(state, TK_Comma) ||
                        !ast_next_token(state)) {
                        array_free(arg_nodes);
                        return error_0201_missing_value(
                            state->token.source,
                            ast_token_span(state, &state->token),
                            TK_RParen);
                    }
                    continue;
                }
                break;
            }
        }
        if (state->token.kind == TK_RParen) {
            if (!ast_next_token(state)) {
                array_free(arg_nodes);
                return false;
            }
        } else if (!ast_expect_token(state, TK_RParen)) {
            array_free(arg_nodes);
            return false;
        }

        u32 first_arg = (u32)array_count(state->call_args);
        for (u32 i = 0; i < array_count(arg_nodes); ++i) {
            array_push(state->call_args, arg_nodes[i]);
        }
        u32 arg_count = (u32)array_count(arg_nodes);
        array_free(arg_nodes);

        u32 call_index = (u32)array_count(state->calls);
        array_push(state->calls,
                   (AstCallInfo){
                       .first_arg = first_arg,
                       .arg_count = arg_count,
                   });

        AstNode node = {
            .kind        = AK_Call,
            .token_index = op.token_index,
            .a           = left_node,
            .b           = call_index,
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
            if ((next.kind == TK_String ||
                 next.kind == TK_InterpolatedStringStart) &&
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
