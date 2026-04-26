//------------------------------------------------------------------------------
// Concrete syntax tree implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cst/cst.h>

//------------------------------------------------------------------------------

#define CST_NO_VALUE UINT32_MAX
#define CST_BP_LOGICAL_OR 10
#define CST_BP_LOGICAL_AND 20
#define CST_BP_BITWISE_OR 30
#define CST_BP_BITWISE_XOR 40
#define CST_BP_BITWISE_AND 50
#define CST_BP_EQUALITY 60
#define CST_BP_COMPARISON 70
#define CST_BP_ADDITIVE 80
#define CST_BP_MULTIPLICATIVE 90
#define CST_BP_PREFIX 100

typedef struct {
    const Lexer* lexer;
    u32          token_index;
    Array(u32) token_integer_indices;
    Array(u32) token_float_indices;
    Array(u32) token_string_indices;
    Array(u32) token_symbol_handles;
    bool allow_statement_boundary;
    bool stop_before_on_branch_head;
    bool stop_before_call;
    bool stop_before_ffi_name;
    Cst  cst;
} CstParseState;

internal bool cst_parse_bind(CstParseState* state, u32* out_node);
internal bool cst_parse_variable(CstParseState* state, u32* out_node);
internal bool cst_parse_destructure(CstParseState* state, u32* out_node);
internal bool cst_parse_expr_bp(CstParseState* state, u8 min_bp, u32* out_node);
internal bool cst_parse_on_branch_expr(CstParseState* state, u32* out_node);
internal bool cst_parse_pattern(CstParseState* state, u32* out_pattern);
internal bool cst_parse_nested_block(CstParseState* state, u32* out_node);
internal bool cst_parse_for(CstParseState* state, u32* out_node);
internal bool cst_parse_top_level_item(CstParseState* state, u32* out_node);
internal bool cst_token_has_newline_before(const CstParseState* state,
                                           u32                  token_index);

//------------------------------------------------------------------------------
// Return the current token, or a synthetic EOF token when the cursor is past
// the end of the lexer token stream.

internal Token cst_current_token(const CstParseState* state)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        return (Token){
            .kind   = TK_EOF,
            .offset = (u32)state->lexer->source.source.count,
        };
    }

    return state->lexer->tokens[state->token_index];
}

//------------------------------------------------------------------------------
// Return the tracked integer index for the current token when applicable.

internal u32 cst_current_integer_index(const CstParseState* state)
{
    if (state->token_index >= array_count(state->token_integer_indices)) {
        return CST_NO_VALUE;
    }
    return state->token_integer_indices[state->token_index];
}

internal u32 cst_current_float_index(const CstParseState* state)
{
    if (state->token_index >= array_count(state->token_float_indices)) {
        return CST_NO_VALUE;
    }
    return state->token_float_indices[state->token_index];
}

//------------------------------------------------------------------------------
// Return the tracked string index for the current token when applicable.

internal u32 cst_current_string_index(const CstParseState* state)
{
    if (state->token_index >= array_count(state->token_string_indices)) {
        return CST_NO_VALUE;
    }
    return state->token_string_indices[state->token_index];
}

//------------------------------------------------------------------------------
// Return the tracked symbol handle for the current token when applicable.

internal u32 cst_current_symbol_handle(const CstParseState* state)
{
    if (state->token_index >= array_count(state->token_symbol_handles)) {
        return CST_NO_VALUE;
    }
    return state->token_symbol_handles[state->token_index];
}

//------------------------------------------------------------------------------
// Advance to the next token in the lexer stream.

internal void cst_advance(CstParseState* state) { state->token_index++; }

//------------------------------------------------------------------------------
// Return whether the current token matches the expected kind and consume it.

internal bool cst_consume(CstParseState* state, TokenKind expected_kind)
{
    if (cst_current_token(state).kind != expected_kind) {
        return false;
    }

    cst_advance(state);
    return true;
}

//------------------------------------------------------------------------------
// Append one CST node and return its table index.

internal bool cst_emit_node(CstParseState* state, CstNode node, u32* out_index)
{
    if (array_count(state->cst.nodes) >= UINT32_MAX) {
        return false;
    }

    u32 node_index = (u32)array_count(state->cst.nodes);
    array_push(state->cst.nodes, node);
    if (out_index) {
        *out_index = node_index;
    }
    return true;
}

//------------------------------------------------------------------------------
// Return whether the current token starts a top-level binding.

internal bool cst_starts_binding(const CstParseState* state)
{
    if (state->token_index + 1 >= array_count(state->lexer->tokens)) {
        return false;
    }

    return state->lexer->tokens[state->token_index].kind == TK_Symbol &&
           state->lexer->tokens[state->token_index + 1].kind == TK_Colon;
}

internal TokenKind cst_peek_kind_at(const CstParseState* state, u32 lookahead)
{
    u32 index = state->token_index + lookahead;
    if (index >= array_count(state->lexer->tokens)) {
        return TK_EOF;
    }
    return state->lexer->tokens[index].kind;
}

internal bool cst_starts_variable(const CstParseState* state)
{
    return cst_current_token(state).kind == TK_Symbol &&
           cst_peek_kind_at(state, 1) == TK_Colon &&
           cst_peek_kind_at(state, 2) != TK_Colon;
}

internal bool cst_token_starts_expression(TokenKind kind)
{
    switch (kind) {
    case TK_Integer:
    case TK_Float:
    case TK_String:
    case TK_InterpolatedStringStart:
    case TK_yes:
    case TK_no:
    case TK_LBracket:
    case TK_Symbol:
    case TK_Bang:
    case TK_Minus:
    case TK_Caret:
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

internal bool cst_token_can_continue_on_branch_head(TokenKind kind)
{
    return kind == TK_FatArrow || kind == TK_as || kind == TK_on;
}

internal bool
cst_current_token_starts_on_branch_head(const CstParseState* state)
{
    Token token = cst_current_token(state);
    if (token.kind == TK_else) {
        return true;
    }

    if (token.kind == TK_EqualEqual || token.kind == TK_BangEqual ||
        token.kind == TK_Less || token.kind == TK_LessEqual ||
        token.kind == TK_Greater || token.kind == TK_GreaterEqual) {
        if (cst_token_has_newline_before(state, state->token_index)) {
            return true;
        }

        u32 depth = 0;
        for (u32 i = state->token_index + 1;
             i < array_count(state->lexer->tokens);
             ++i) {
            TokenKind kind = state->lexer->tokens[i].kind;
            if (depth == 0 && kind == TK_FatArrow) {
                return true;
            }
            if (depth == 0 &&
                (kind == TK_Comma || kind == TK_RBrace || kind == TK_else)) {
                return false;
            }
            if (kind == TK_LParen || kind == TK_LBrace || kind == TK_LBracket) {
                depth++;
            } else if (kind == TK_RParen || kind == TK_RBrace ||
                       kind == TK_RBracket) {
                if (depth == 0) {
                    return false;
                }
                depth--;
            }
        }
        return false;
    }

    if (token.kind == TK_Dot && cst_peek_kind_at(state, 1) == TK_Symbol) {
        return cst_token_can_continue_on_branch_head(
            cst_peek_kind_at(state, 2));
    }

    if (token.kind == TK_Symbol) {
        return cst_token_can_continue_on_branch_head(
            cst_peek_kind_at(state, 1));
    }

    return false;
}

internal TokenKind cst_kind_at_stream_index(const CstParseState* state,
                                            u32                  index)
{
    if (index >= array_count(state->lexer->tokens)) {
        return TK_EOF;
    }
    return state->lexer->tokens[index].kind;
}

internal bool cst_skip_balanced_tokens(const CstParseState* state,
                                       u32*                 io_index,
                                       TokenKind            open_kind,
                                       TokenKind            close_kind)
{
    if (cst_kind_at_stream_index(state, *io_index) != open_kind) {
        return false;
    }
    u32 depth = 0;
    while (*io_index < array_count(state->lexer->tokens)) {
        TokenKind kind = cst_kind_at_stream_index(state, *io_index);
        if (kind == open_kind) {
            depth++;
        } else if (kind == close_kind) {
            depth--;
            (*io_index)++;
            return depth == 0;
        } else if (kind == TK_EOF) {
            return false;
        }
        (*io_index)++;
    }
    return false;
}

internal bool cst_skip_until_matching_rbracket(const CstParseState* state,
                                               u32*                 io_index)
{
    u32 paren_depth   = 0;
    u32 bracket_depth = 0;
    u32 brace_depth   = 0;

    while (*io_index < array_count(state->lexer->tokens)) {
        TokenKind kind = cst_kind_at_stream_index(state, *io_index);
        switch (kind) {
        case TK_LParen:
            paren_depth++;
            break;
        case TK_RParen:
            if (paren_depth == 0) {
                return false;
            }
            paren_depth--;
            break;
        case TK_LBracket:
            bracket_depth++;
            break;
        case TK_RBracket:
            if (paren_depth == 0 && brace_depth == 0 && bracket_depth == 0) {
                return true;
            }
            if (bracket_depth == 0) {
                return false;
            }
            bracket_depth--;
            break;
        case TK_LBrace:
            brace_depth++;
            break;
        case TK_RBrace:
            if (brace_depth == 0) {
                return false;
            }
            brace_depth--;
            break;
        case TK_EOF:
            return false;
        default:
            break;
        }
        (*io_index)++;
    }

    return false;
}

internal bool cst_skip_enum_value_tokens(const CstParseState* state,
                                         u32*                 io_index)
{
    u32 start_index   = *io_index;
    u32 paren_depth   = 0;
    u32 bracket_depth = 0;
    u32 brace_depth   = 0;

    while (*io_index < array_count(state->lexer->tokens)) {
        TokenKind kind = cst_kind_at_stream_index(state, *io_index);
        if (kind == TK_EOF) {
            return false;
        }
        if (paren_depth == 0 && bracket_depth == 0 && brace_depth == 0 &&
            kind == TK_RBrace) {
            return true;
        }
        if (*io_index > start_index && paren_depth == 0 && bracket_depth == 0 &&
            brace_depth == 0 && kind == TK_Symbol &&
            cst_token_has_newline_before(state, *io_index)) {
            return true;
        }

        switch (kind) {
        case TK_LParen:
            paren_depth++;
            break;
        case TK_RParen:
            if (paren_depth == 0) {
                return false;
            }
            paren_depth--;
            break;
        case TK_LBracket:
            bracket_depth++;
            break;
        case TK_RBracket:
            if (bracket_depth == 0) {
                return false;
            }
            bracket_depth--;
            break;
        case TK_LBrace:
            brace_depth++;
            break;
        case TK_RBrace:
            if (brace_depth == 0) {
                return false;
            }
            brace_depth--;
            break;
        default:
            break;
        }
        (*io_index)++;
    }

    return false;
}

internal bool cst_skip_type_tokens(const CstParseState* state, u32* io_index)
{
    TokenKind kind = cst_kind_at_stream_index(state, *io_index);
    if (kind == TK_Symbol) {
        (*io_index)++;
        return true;
    }
    if (kind == TK_Caret) {
        (*io_index)++;
        return cst_skip_type_tokens(state, io_index);
    }
    if (kind == TK_plex || kind == TK_union) {
        (*io_index)++;
        while (kind == TK_plex &&
               cst_kind_at_stream_index(state, *io_index) == TK_Hash) {
            (*io_index)++;
            if (cst_kind_at_stream_index(state, *io_index) != TK_Symbol) {
                return false;
            }
            (*io_index)++;
        }
        if (cst_kind_at_stream_index(state, *io_index) != TK_LBrace) {
            return false;
        }
        (*io_index)++;
        while (cst_kind_at_stream_index(state, *io_index) != TK_RBrace) {
            if (cst_kind_at_stream_index(state, *io_index) == TK_EOF ||
                cst_kind_at_stream_index(state, *io_index) != TK_Symbol) {
                return false;
            }
            (*io_index)++;
            if (!cst_skip_type_tokens(state, io_index)) {
                return false;
            }
            if (cst_kind_at_stream_index(state, *io_index) == TK_Equal) {
                (*io_index)++;
                if (!cst_skip_enum_value_tokens(state, io_index)) {
                    return false;
                }
            }
        }
        (*io_index)++;
        return true;
    }
    if (kind == TK_enum) {
        (*io_index)++;
        if (cst_kind_at_stream_index(state, *io_index) != TK_LBrace) {
            return false;
        }
        (*io_index)++;
        while (cst_kind_at_stream_index(state, *io_index) != TK_RBrace) {
            if (cst_kind_at_stream_index(state, *io_index) != TK_Symbol) {
                return false;
            }
            (*io_index)++;
            if (cst_kind_at_stream_index(state, *io_index) == TK_LParen) {
                (*io_index)++;
                if (cst_kind_at_stream_index(state, *io_index) != TK_RParen) {
                    for (;;) {
                        if (!cst_skip_type_tokens(state, io_index)) {
                            return false;
                        }
                        if (cst_kind_at_stream_index(state, *io_index) !=
                            TK_Comma) {
                            break;
                        }
                        (*io_index)++;
                    }
                }
                if (cst_kind_at_stream_index(state, *io_index) != TK_RParen) {
                    return false;
                }
                (*io_index)++;
            }
        }
        (*io_index)++;
        return true;
    }
    if (kind == TK_LBracket) {
        (*io_index)++;
        if (cst_kind_at_stream_index(state, *io_index) != TK_RBracket &&
            !cst_skip_until_matching_rbracket(state, io_index)) {
            return false;
        }
        if (cst_kind_at_stream_index(state, *io_index) != TK_RBracket) {
            return false;
        }
        (*io_index)++;
        return cst_skip_type_tokens(state, io_index);
    }
    if (kind == TK_LParen) {
        (*io_index)++;
        if (!cst_skip_type_tokens(state, io_index)) {
            return false;
        }
        while (cst_kind_at_stream_index(state, *io_index) == TK_Comma) {
            (*io_index)++;
            if (cst_kind_at_stream_index(state, *io_index) == TK_RParen) {
                break;
            }
            if (!cst_skip_type_tokens(state, io_index)) {
                return false;
            }
        }
        if (cst_kind_at_stream_index(state, *io_index) != TK_RParen) {
            return false;
        }
        (*io_index)++;
        return true;
    }
    if (kind != TK_fn) {
        return false;
    }

    (*io_index)++;
    if (cst_kind_at_stream_index(state, *io_index) != TK_LParen) {
        return false;
    }
    (*io_index)++;
    if (cst_kind_at_stream_index(state, *io_index) != TK_RParen) {
        for (;;) {
            if (!cst_skip_type_tokens(state, io_index)) {
                return false;
            }
            if (cst_kind_at_stream_index(state, *io_index) == TK_Comma) {
                (*io_index)++;
                continue;
            }
            break;
        }
        if (cst_kind_at_stream_index(state, *io_index) != TK_RParen) {
            return false;
        }
    }
    (*io_index)++;
    if (cst_kind_at_stream_index(state, *io_index) != TK_ThinArrow) {
        return false;
    }
    (*io_index)++;
    return cst_skip_type_tokens(state, io_index);
}

internal bool
cst_remaining_bind_value_is_type_syntax(const CstParseState* state)
{
    u32 token_index = state->token_index;
    if (!cst_skip_type_tokens(state, &token_index)) {
        return false;
    }

    TokenKind next_kind = cst_kind_at_stream_index(state, token_index);
    return next_kind == TK_EOF ||
           (next_kind == TK_Symbol &&
            cst_kind_at_stream_index(state, token_index + 1) == TK_Colon);
}

internal bool cst_starts_annotated_bind(const CstParseState* state)
{
    if (cst_current_token(state).kind != TK_Symbol ||
        cst_peek_kind_at(state, 1) != TK_Colon) {
        return false;
    }
    if (cst_peek_kind_at(state, 2) == TK_Colon) {
        return true;
    }

    u32 type_index = state->token_index + 2;
    if (!cst_skip_type_tokens(state, &type_index)) {
        return false;
    }
    return cst_kind_at_stream_index(state, type_index) == TK_Colon;
}

internal bool cst_starts_assignment(const CstParseState* state)
{
    if (cst_current_token(state).kind != TK_Symbol) {
        return false;
    }

    switch (cst_peek_kind_at(state, 1)) {
    case TK_Equal:
    case TK_PlusEqual:
    case TK_MinusEqual:
    case TK_StarEqual:
    case TK_SlashEqual:
    case TK_PercentEqual:
    case TK_AmpEqual:
    case TK_CaretEqual:
    case TK_PipeEqual:
    case TK_AmpAmpEqual:
    case TK_PipePipeEqual:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Return the infix binding powers for the given token kind.

internal bool
cst_infix_binding_power(TokenKind kind, u8* out_left_bp, u8* out_right_bp)
{
    switch (kind) {
    case TK_LParen:
    case TK_LBracket:
    case TK_LBrace:
    case TK_with:
        *out_left_bp  = CST_BP_PREFIX + 10;
        *out_right_bp = CST_BP_PREFIX + 10;
        return true;
    case TK_Dot:
        *out_left_bp  = CST_BP_PREFIX + 10;
        *out_right_bp = CST_BP_PREFIX + 10;
        return true;
    case TK_Plus:
    case TK_Minus:
        *out_left_bp  = CST_BP_ADDITIVE;
        *out_right_bp = CST_BP_ADDITIVE + 1;
        return true;
    case TK_Less:
    case TK_LessEqual:
    case TK_Greater:
    case TK_GreaterEqual:
        *out_left_bp  = CST_BP_COMPARISON;
        *out_right_bp = CST_BP_COMPARISON + 1;
        return true;
    case TK_EqualEqual:
    case TK_BangEqual:
        *out_left_bp  = CST_BP_EQUALITY;
        *out_right_bp = CST_BP_EQUALITY + 1;
        return true;
    case TK_Amp:
        *out_left_bp  = CST_BP_BITWISE_AND;
        *out_right_bp = CST_BP_BITWISE_AND + 1;
        return true;
    case TK_Caret:
        *out_left_bp  = CST_BP_BITWISE_XOR;
        *out_right_bp = CST_BP_BITWISE_XOR + 1;
        return true;
    case TK_Pipe:
        *out_left_bp  = CST_BP_BITWISE_OR;
        *out_right_bp = CST_BP_BITWISE_OR + 1;
        return true;
    case TK_AmpAmp:
        *out_left_bp  = CST_BP_LOGICAL_AND;
        *out_right_bp = CST_BP_LOGICAL_AND + 1;
        return true;
    case TK_PipePipe:
        *out_left_bp  = CST_BP_LOGICAL_OR;
        *out_right_bp = CST_BP_LOGICAL_OR + 1;
        return true;
    case TK_Star:
    case TK_Slash:
    case TK_Percent:
        *out_left_bp  = CST_BP_MULTIPLICATIVE;
        *out_right_bp = CST_BP_MULTIPLICATIVE + 1;
        return true;

    default:
        return false;
    }
}

internal bool cst_token_has_newline_before(const CstParseState* state,
                                           u32                  token_index)
{
    if (token_index == 0 || token_index >= array_count(state->lexer->tokens)) {
        return false;
    }
    usize previous_end = lex_token_end_offset(
        state->lexer, &state->lexer->tokens[token_index - 1]);
    usize  current_start = state->lexer->tokens[token_index].offset;
    string source        = state->lexer->source.source;
    if (previous_end > current_start) {
        return false;
    }
    for (usize i = previous_end; i < current_start && i < source.count; ++i) {
        if (source.data[i] == '\n') {
            return true;
        }
    }
    return false;
}

internal bool cst_postfix_token_can_cross_statement_boundary(TokenKind kind)
{
    return kind == TK_with;
}

internal bool cst_caret_is_postfix_deref(const CstParseState* state)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        return true;
    }
    if (state->token_index + 1 < array_count(state->lexer->tokens) &&
        cst_token_has_newline_before(state, state->token_index + 1)) {
        return true;
    }
    return !cst_token_starts_expression(cst_peek_kind_at(state, 1));
}

internal bool cst_consumed_caret_is_postfix_deref(const CstParseState* state)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        return true;
    }
    if (cst_token_has_newline_before(state, state->token_index)) {
        return true;
    }
    return !cst_token_starts_expression(cst_current_token(state).kind);
}

//------------------------------------------------------------------------------
// Parse one expression using Pratt binding powers.

internal bool cst_parse_expr_bp(CstParseState* state, u8 min_bp, u32* out_node);
internal bool cst_parse_variable_payload(CstParseState* state,
                                         u32            token_index,
                                         u32*           out_node);
internal bool cst_parse_fn_expr(CstParseState* state, u32* out_node);
internal bool cst_parse_ffi_def(CstParseState* state, u32* out_node);
internal bool cst_parse_mod_ref(CstParseState* state, u32* out_node);
internal bool cst_parse_use(CstParseState* state, u32* out_node);
internal bool cst_parse_on_expr(CstParseState* state, u32* out_node);
internal bool cst_parse_type(CstParseState* state, u32* out_node);
internal bool cst_parse_fn_signature(CstParseState* state,
                                     bool           allow_named_params,
                                     bool           require_return_type,
                                     u32*           out_signature_index);
internal bool cst_parse_callable_signature(CstParseState* state,
                                           TokenKind      introducer,
                                           bool           allow_named_params,
                                           bool           require_return_type,
                                           u32*           out_signature_index);

//------------------------------------------------------------------------------
// Parse one type annotation.

internal bool cst_parse_type_signature(CstParseState* state,
                                       u32*           out_signature_index)
{
    return cst_parse_fn_signature(state, false, true, out_signature_index);
}

internal bool cst_parse_fn_signature(CstParseState* state,
                                     bool           allow_named_params,
                                     bool           require_return_type,
                                     u32*           out_signature_index)
{
    return cst_parse_callable_signature(state,
                                        TK_fn,
                                        allow_named_params,
                                        require_return_type,
                                        out_signature_index);
}

internal bool cst_parse_callable_signature(CstParseState* state,
                                           TokenKind      introducer,
                                           bool           allow_named_params,
                                           bool           require_return_type,
                                           u32*           out_signature_index)
{
    if (introducer != TK_EOF && !cst_consume(state, introducer)) {
        return false;
    }
    if (!cst_consume(state, TK_LParen)) {
        return false;
    }

    u32  first_param = (u32)array_count(state->cst.params);
    u32  param_count = 0;
    bool is_varargs  = false;

    if (cst_current_token(state).kind != TK_RParen) {
        for (;;) {
            if (!allow_named_params && !require_return_type &&
                cst_current_token(state).kind == TK_Ellipsis) {
                is_varargs = true;
                cst_advance(state);
                break;
            }
            if (allow_named_params) {
                if (cst_current_token(state).kind != TK_Symbol) {
                    return false;
                }
                u32 symbol_handle = cst_current_symbol_handle(state);
                cst_advance(state);
                if (!cst_consume(state, TK_Colon)) {
                    return false;
                }
                u32 type_node = 0;
                if (!cst_parse_type(state, &type_node)) {
                    return false;
                }
                array_push(state->cst.params,
                           (CstParam){
                               .symbol_handle   = symbol_handle,
                               .type_node_index = type_node,
                           });
            } else {
                u32 type_node = 0;
                if (!cst_parse_type(state, &type_node)) {
                    return false;
                }
                array_push(state->cst.params,
                           (CstParam){
                               .symbol_handle   = CST_NO_VALUE,
                               .type_node_index = type_node,
                           });
            }
            ++param_count;
            if (cst_current_token(state).kind == TK_Comma) {
                cst_advance(state);
                if (!allow_named_params && !require_return_type &&
                    cst_current_token(state).kind == TK_Ellipsis) {
                    is_varargs = true;
                    cst_advance(state);
                    break;
                }
                continue;
            }
            break;
        }
    }

    if (!cst_consume(state, TK_RParen)) {
        return false;
    }

    u32 return_type = CST_NO_VALUE;
    if (cst_current_token(state).kind == TK_ThinArrow) {
        cst_advance(state);
        if (!cst_parse_type(state, &return_type)) {
            return false;
        }
    } else if (require_return_type) {
        return false;
    }

    u32 signature_index = (u32)array_count(state->cst.fn_signatures);
    array_push(state->cst.fn_signatures,
               (CstFnSignature){
                   .first_param            = first_param,
                   .param_count            = param_count,
                   .return_type_node_index = return_type,
                   .is_varargs             = is_varargs,
               });
    *out_signature_index = signature_index;
    return true;
}

internal bool cst_parse_type(CstParseState* state, u32* out_node)
{
    Token token = cst_current_token(state);

    if (token.kind == TK_Symbol) {
        u32 symbol_handle = cst_current_symbol_handle(state);
        if (symbol_handle == CST_NO_VALUE) {
            return false;
        }

        cst_advance(state);
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_SymbolRef,
                                 .token_index = state->token_index - 1,
                                 .a           = symbol_handle,
                             },
                             out_node);
    }

    if (token.kind == TK_LParen) {
        u32 token_index = state->token_index;
        cst_advance(state);
        u32 first_item = (u32)array_count(state->cst.tuple_items);
        u32 item_count = 0;
        u32 item       = 0;
        if (!cst_parse_type(state, &item)) {
            return false;
        }
        array_push(state->cst.tuple_items, item);
        item_count++;

        bool is_tuple = false;
        while (cst_current_token(state).kind == TK_Comma) {
            is_tuple = true;
            cst_advance(state);
            if (cst_current_token(state).kind == TK_RParen) {
                break;
            }
            if (!cst_parse_type(state, &item)) {
                return false;
            }
            array_push(state->cst.tuple_items, item);
            item_count++;
        }
        if (!cst_consume(state, TK_RParen)) {
            return false;
        }
        if (!is_tuple) {
            *out_node = state->cst.tuple_items[first_item];
            return true;
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_TypeTuple,
                                 .token_index = token_index,
                                 .a           = first_item,
                                 .b           = item_count,
                             },
                             out_node);
    }

    if (token.kind == TK_LBracket) {
        u32 token_index = state->token_index;
        cst_advance(state);
        if (cst_current_token(state).kind == TK_RBracket) {
            cst_advance(state);
            u32 element_type = 0;
            if (!cst_parse_type(state, &element_type)) {
                return false;
            }
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_TypeSlice,
                                     .token_index = token_index,
                                     .a           = element_type,
                                 },
                                 out_node);
        }
        u32 length = 0;
        if (!cst_parse_expr_bp(state, 0, &length)) {
            return false;
        }
        if (!cst_consume(state, TK_RBracket)) {
            return false;
        }
        u32 element_type = 0;
        if (!cst_parse_type(state, &element_type)) {
            return false;
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_TypeArray,
                                 .token_index = token_index,
                                 .a           = length,
                                 .b           = element_type,
                             },
                             out_node);
    }

    if (token.kind == TK_Caret) {
        u32 token_index = state->token_index;
        cst_advance(state);
        u32 pointee_type = 0;
        if (!cst_parse_type(state, &pointee_type)) {
            return false;
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_TypePointer,
                                 .token_index = token_index,
                                 .a           = pointee_type,
                             },
                             out_node);
    }

    if (token.kind == TK_plex || token.kind == TK_union) {
        u32  token_index = state->token_index;
        bool is_union    = token.kind == TK_union;
        cst_advance(state);
        u32 flags = is_union ? CPTF_Union : CPTF_None;
        while (!is_union && cst_current_token(state).kind == TK_Hash) {
            cst_advance(state);
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
            string annotation =
                lex_symbol(state->lexer, cst_current_symbol_handle(state));
            if (string_eq(annotation, s("c"))) {
                flags |= CPTF_C;
            } else if (string_eq(annotation, s("packed"))) {
                flags |= CPTF_C | CPTF_Packed;
            } else {
                return false;
            }
            cst_advance(state);
        }
        if (!cst_consume(state, TK_LBrace)) {
            return false;
        }
        u32 first_field = (u32)array_count(state->cst.plex_fields);
        u32 field_count = 0;
        while (cst_current_token(state).kind != TK_RBrace) {
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
            u32 field_token  = state->token_index;
            u32 field_symbol = cst_current_symbol_handle(state);
            cst_advance(state);
            u32 type_node = 0;
            if (!cst_parse_type(state, &type_node)) {
                return false;
            }
            array_push(state->cst.plex_fields,
                       (CstPlexField){
                           .token_index     = field_token,
                           .symbol_handle   = field_symbol,
                           .type_node_index = type_node,
                       });
            field_count++;
        }
        if (!cst_consume(state, TK_RBrace)) {
            return false;
        }
        u32 plex_type_index = (u32)array_count(state->cst.plex_types);
        array_push(state->cst.plex_types,
                   (CstPlexTypeInfo){
                       .first_field = first_field,
                       .field_count = field_count,
                       .flags       = flags,
                   });
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_TypePlex,
                                 .token_index = token_index,
                                 .a           = plex_type_index,
                             },
                             out_node);
    }

    if (token.kind == TK_enum) {
        u32 token_index = state->token_index;
        cst_advance(state);
        if (!cst_consume(state, TK_LBrace)) {
            return false;
        }
        u32 first_variant = (u32)array_count(state->cst.enum_variants);
        u32 variant_count = 0;
        while (cst_current_token(state).kind != TK_RBrace) {
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
            u32 variant_token      = state->token_index;
            u32 variant_symbol     = cst_current_symbol_handle(state);
            u32 variant_type_node  = U32_MAX;
            u32 variant_value_node = U32_MAX;
            cst_advance(state);
            if (cst_current_token(state).kind == TK_LParen) {
                u32 lparen = state->token_index;
                cst_advance(state);
                u32 first_item = (u32)array_count(state->cst.tuple_items);
                u32 item_count = 0;
                if (cst_current_token(state).kind != TK_RParen) {
                    for (;;) {
                        u32 item = 0;
                        if (!cst_parse_type(state, &item)) {
                            return false;
                        }
                        array_push(state->cst.tuple_items, item);
                        item_count++;
                        if (cst_current_token(state).kind != TK_Comma) {
                            break;
                        }
                        cst_advance(state);
                    }
                }
                if (!cst_consume(state, TK_RParen)) {
                    return false;
                }
                if (item_count == 1) {
                    variant_type_node = state->cst.tuple_items[first_item];
                } else {
                    if (!cst_emit_node(state,
                                       (CstNode){
                                           .kind        = CK_TypeTuple,
                                           .token_index = lparen,
                                           .a           = first_item,
                                           .b           = item_count,
                                       },
                                       &variant_type_node)) {
                        return false;
                    }
                }
            }
            if (cst_current_token(state).kind == TK_Equal) {
                cst_advance(state);
                bool previous_boundary = state->allow_statement_boundary;
                state->allow_statement_boundary = true;
                bool parsed = cst_parse_expr_bp(state, 0, &variant_value_node);
                state->allow_statement_boundary = previous_boundary;
                if (!parsed) {
                    return false;
                }
            }
            array_push(state->cst.enum_variants,
                       (CstEnumVariant){
                           .token_index      = variant_token,
                           .symbol_handle    = variant_symbol,
                           .type_node_index  = variant_type_node,
                           .value_node_index = variant_value_node,
                       });
            variant_count++;
        }
        if (!cst_consume(state, TK_RBrace)) {
            return false;
        }
        u32 enum_type_index = (u32)array_count(state->cst.enum_types);
        array_push(state->cst.enum_types,
                   (CstEnumTypeInfo){
                       .first_variant = first_variant,
                       .variant_count = variant_count,
                   });
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_TypeEnum,
                                 .token_index = token_index,
                                 .a           = enum_type_index,
                             },
                             out_node);
    }

    if (token.kind != TK_fn) {
        return false;
    }

    u32 token_index     = state->token_index;
    u32 signature_index = 0;
    if (!cst_parse_type_signature(state, &signature_index)) {
        return false;
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_TypeFn,
                             .token_index = token_index,
                             .a           = signature_index,
                         },
                         out_node);
}

//------------------------------------------------------------------------------
// Return whether one CST node already represents a string-literal chain.

internal bool cst_node_is_stringish(const Cst* cst, u32 node_index)
{
    switch (cst->nodes[node_index].kind) {
    case CK_StringLiteral:
    case CK_StringConcat:
    case CK_InterpolatedString:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Parse one interpolated string expression into a contiguous part range.

internal bool cst_parse_interpolated_string(CstParseState* state, u32* out_node)
{
    typedef struct {
        bool is_expr;
        u32  token_index;
        u32  payload;
    } InterpPart;

    u32 token_index         = state->token_index;
    Array(InterpPart) parts = NULL;
    cst_advance(state);

    for (;;) {
        Token token = cst_current_token(state);

        if (token.kind == TK_InterpolatedStringEnd) {
            cst_advance(state);
            u32 part_start = (u32)array_count(state->cst.nodes);
            for (u32 i = 0; i < array_count(parts); ++i) {
                if (!cst_emit_node(state,
                                   (CstNode){
                                       .kind        = parts[i].is_expr
                                                          ? CK_InterpPartExpr
                                                          : CK_StringLiteral,
                                       .token_index = parts[i].token_index,
                                       .a           = parts[i].payload,
                                   },
                                   NULL)) {
                    array_free(parts);
                    return false;
                }
            }

            array_free(parts);
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_InterpolatedString,
                                     .token_index = token_index,
                                     .a           = part_start,
                                     .b = (u32)array_count(state->cst.nodes),
                                 },
                                 out_node);
        }

        if (token.kind == TK_String) {
            u32 string_index = cst_current_string_index(state);
            if (string_index == CST_NO_VALUE) {
                array_free(parts);
                return false;
            }

            u32 string_token_index = state->token_index;
            cst_advance(state);
            array_push(parts,
                       ((InterpPart){
                           .is_expr     = false,
                           .token_index = string_token_index,
                           .payload     = string_index,
                       }));
            continue;
        }

        if (token.kind == TK_LBrace) {
            u32 expr = 0;
            cst_advance(state);
            if (!cst_parse_expr_bp(state, 0, &expr) ||
                !cst_consume(state, TK_RBrace)) {
                array_free(parts);
                return false;
            }

            array_push(parts,
                       ((InterpPart){
                           .is_expr     = true,
                           .token_index = token_index,
                           .payload     = expr,
                       }));
            continue;
        }

        array_free(parts);
        return false;
    }
}

//------------------------------------------------------------------------------
// Parse one prefix expression.

internal bool cst_parse_prefix(CstParseState* state, u32* out_node)
{
    Token token = cst_current_token(state);

    switch (token.kind) {
    case TK_Integer:
        {
            u32 integer_index = cst_current_integer_index(state);
            if (integer_index == CST_NO_VALUE) {
                return false;
            }

            cst_advance(state);
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_IntegerLiteral,
                                     .token_index = state->token_index - 1,
                                     .a           = integer_index,
                                 },
                                 out_node);
        }

    case TK_Float:
        {
            u32 float_index = cst_current_float_index(state);
            if (float_index == CST_NO_VALUE) {
                return false;
            }

            cst_advance(state);
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_FloatLiteral,
                                     .token_index = state->token_index - 1,
                                     .a           = float_index,
                                 },
                                 out_node);
        }

    case TK_String:
        {
            u32 string_index = cst_current_string_index(state);
            if (string_index == CST_NO_VALUE) {
                return false;
            }

            cst_advance(state);
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_StringLiteral,
                                     .token_index = state->token_index - 1,
                                     .a           = string_index,
                                 },
                                 out_node);
        }

    case TK_yes:
    case TK_no:
        cst_advance(state);
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_BoolLiteral,
                                 .token_index = state->token_index - 1,
                                 .a           = token.kind == TK_yes ? 1u : 0u,
                             },
                             out_node);

    case TK_InterpolatedStringStart:
        return cst_parse_interpolated_string(state, out_node);

    case TK_Symbol:
        {
            u32 symbol_handle = cst_current_symbol_handle(state);
            if (symbol_handle == CST_NO_VALUE) {
                return false;
            }

            cst_advance(state);
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_SymbolRef,
                                     .token_index = state->token_index - 1,
                                     .a           = symbol_handle,
                                 },
                                 out_node);
        }

    case TK_Minus:
    case TK_Bang:
    case TK_Caret:
        {
            u32 token_index = state->token_index;
            u32 operand     = 0;
            cst_advance(state);
            if (!cst_parse_expr_bp(state, CST_BP_PREFIX, &operand)) {
                return false;
            }

            return cst_emit_node(
                state,
                (CstNode){
                    .kind        = token.kind == TK_Bang    ? CK_LogicalNot
                                   : token.kind == TK_Caret ? CK_AddressOf
                                                            : CK_IntegerNegate,
                    .token_index = token_index,
                    .a           = operand,
                },
                out_node);
        }

    case TK_LParen:
        {
            u32 token_index = state->token_index;
            u32 inner       = 0;
            cst_advance(state);
            if (!cst_parse_expr_bp(state, 0, &inner)) {
                return false;
            }

            u32 first_item = (u32)array_count(state->cst.tuple_items);
            u32 item_count = 1;
            array_push(state->cst.tuple_items, inner);

            bool is_tuple = false;
            while (cst_current_token(state).kind == TK_Comma) {
                is_tuple = true;
                cst_advance(state);
                if (cst_current_token(state).kind == TK_RParen) {
                    break;
                }
                if (!cst_parse_expr_bp(state, 0, &inner)) {
                    return false;
                }
                array_push(state->cst.tuple_items, inner);
                item_count++;
            }

            if (!cst_consume(state, TK_RParen)) {
                return false;
            }
            if (is_tuple) {
                return cst_emit_node(state,
                                     (CstNode){
                                         .kind        = CK_Tuple,
                                         .token_index = token_index,
                                         .a           = first_item,
                                         .b           = item_count,
                                     },
                                     out_node);
            }

            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_Group,
                                     .token_index = token_index,
                                     .a           = inner,
                                 },
                                 out_node);
        }

    case TK_LBracket:
        {
            u32 token_index = state->token_index;
            cst_advance(state);
            u32 first_item = (u32)array_count(state->cst.tuple_items);
            u32 item_count = 0;
            if (cst_current_token(state).kind != TK_RBracket) {
                for (;;) {
                    u32 item = 0;
                    if (!cst_parse_expr_bp(state, 0, &item)) {
                        return false;
                    }
                    array_push(state->cst.tuple_items, item);
                    ++item_count;
                    if (cst_current_token(state).kind == TK_Comma) {
                        cst_advance(state);
                        if (cst_current_token(state).kind == TK_RBracket) {
                            break;
                        }
                        continue;
                    }
                    break;
                }
            }
            if (!cst_consume(state, TK_RBracket)) {
                return false;
            }
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_Array,
                                     .token_index = token_index,
                                     .a           = first_item,
                                     .b           = item_count,
                                 },
                                 out_node);
        }

    case TK_fn:
        return cst_parse_fn_expr(state, out_node);
    case TK_on:
        return cst_parse_on_expr(state, out_node);
    case TK_for:
        return cst_parse_for(state, out_node);
    case TK_Dollar:
        {
            u32 token_index = state->token_index;
            u32 block       = 0;
            u32 label       = U32_MAX;
            cst_advance(state);
            if (cst_current_token(state).kind == TK_Symbol) {
                label = cst_current_symbol_handle(state);
                cst_advance(state);
            }
            if (cst_current_token(state).kind != TK_LBrace ||
                !cst_parse_nested_block(state, &block)) {
                return false;
            }

            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_ExprBlock,
                                     .token_index = token_index,
                                     .a           = block,
                                     .b           = label,
                                 },
                                 out_node);
        }

    default:
        return false;
    }
}

internal bool cst_parse_on_expr(CstParseState* state, u32* out_node)
{
    u32 token_index = state->token_index;
    cst_advance(state);

    u32 first_branch = (u32)array_count(state->cst.on_branches);
    if (cst_current_token(state).kind == TK_LBrace) {
        cst_advance(state);
        while (cst_current_token(state).kind != TK_RBrace) {
            if (cst_current_token(state).kind == TK_EOF) {
                return false;
            }

            CstOnBranch branch          = {0};
            branch.pattern_index        = U32_MAX;
            branch.pattern_count        = 0;
            branch.binder_symbol_handle = U32_MAX;
            branch.binder_token_index   = U32_MAX;
            branch.guard_node_index     = U32_MAX;
            if (cst_current_token(state).kind == TK_else) {
                branch.flags = COBF_Else;
                cst_advance(state);
            } else if (!cst_parse_expr_bp(state, 0, &branch.guard_node_index)) {
                return false;
            }

            if (!cst_consume(state, TK_FatArrow)) {
                return false;
            }

            if (!cst_parse_on_branch_expr(state, &branch.expr_node_index)) {
                return false;
            }
            array_push(state->cst.on_branches, branch);

            if (branch.flags & COBF_Else) {
                break;
            }
        }

        if (!cst_consume(state, TK_RBrace)) {
            return false;
        }

        u32 on_index = (u32)array_count(state->cst.ons);
        array_push(
            state->cst.ons,
            (CstOnInfo){
                .kind         = COK_Condition,
                .first_branch = first_branch,
                .branch_count =
                    (u32)array_count(state->cst.on_branches) - first_branch,
            });
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_On,
                                 .token_index = token_index,
                                 .a           = U32_MAX,
                                 .b           = on_index,
                             },
                             out_node);
    }

    u32 scrutinee = 0;
    if (!cst_parse_expr_bp(state, 0, &scrutinee)) {
        return false;
    }

    if (cst_current_token(state).kind == TK_LBrace) {
        cst_advance(state);
        while (cst_current_token(state).kind != TK_RBrace) {
            if (cst_current_token(state).kind == TK_EOF) {
                return false;
            }

            CstOnBranch branch          = {0};
            branch.binder_symbol_handle = U32_MAX;
            branch.binder_token_index   = U32_MAX;
            branch.guard_node_index     = U32_MAX;
            if (cst_current_token(state).kind == TK_else) {
                branch.flags = COBF_Else;
                cst_advance(state);
                if (cst_current_token(state).kind == TK_as) {
                    cst_advance(state);
                    if (cst_current_token(state).kind != TK_Symbol) {
                        return false;
                    }
                    branch.binder_symbol_handle =
                        cst_current_symbol_handle(state);
                    branch.binder_token_index = state->token_index;
                    cst_advance(state);
                }
                if (!cst_consume(state, TK_FatArrow)) {
                    return false;
                }
            } else {
                Array(u32) branch_patterns = NULL;
                for (;;) {
                    u32 pattern_root = 0;
                    if (!cst_parse_pattern(state, &pattern_root)) {
                        array_free(branch_patterns);
                        return false;
                    }
                    array_push(branch_patterns, pattern_root);
                    if (cst_current_token(state).kind != TK_Comma) {
                        break;
                    }
                    cst_advance(state);
                }
                branch.pattern_index =
                    (u32)array_count(state->cst.pattern_items);
                branch.pattern_count = (u32)array_count(branch_patterns);
                for (u32 pattern = 0; pattern < branch.pattern_count;
                     ++pattern) {
                    array_push(state->cst.pattern_items,
                               branch_patterns[pattern]);
                }
                array_free(branch_patterns);
                if (cst_current_token(state).kind == TK_as) {
                    cst_advance(state);
                    if (cst_current_token(state).kind != TK_Symbol) {
                        return false;
                    }
                    branch.binder_symbol_handle =
                        cst_current_symbol_handle(state);
                    branch.binder_token_index = state->token_index;
                    cst_advance(state);
                }
                if (cst_current_token(state).kind == TK_on) {
                    cst_advance(state);
                    if (!cst_parse_expr_bp(
                            state, 0, &branch.guard_node_index)) {
                        return false;
                    }
                }
                if (!cst_consume(state, TK_FatArrow)) {
                    return false;
                }
            }

            if (!cst_parse_on_branch_expr(state, &branch.expr_node_index)) {
                return false;
            }
            array_push(state->cst.on_branches, branch);

            if (branch.flags & COBF_Else) {
                break;
            }
        }

        if (!cst_consume(state, TK_RBrace)) {
            return false;
        }

        u32 on_index = (u32)array_count(state->cst.ons);
        array_push(
            state->cst.ons,
            (CstOnInfo){
                .kind         = COK_Value,
                .first_branch = first_branch,
                .branch_count =
                    (u32)array_count(state->cst.on_branches) - first_branch,
            });
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_On,
                                 .token_index = token_index,
                                 .a           = scrutinee,
                                 .b           = on_index,
                             },
                             out_node);
    }

    if (!cst_consume(state, TK_FatArrow)) {
        return false;
    }

    u32 true_expr = 0;
    if (!cst_parse_on_branch_expr(state, &true_expr)) {
        return false;
    }
    if (cst_current_token(state).kind != TK_else) {
        u32 true_node    = 0;
        u32 true_pattern = 0;
        if (!cst_emit_node(state,
                           (CstNode){
                               .kind        = CK_BoolLiteral,
                               .token_index = token_index,
                               .a           = 1,
                           },
                           &true_node)) {
            return false;
        }
        true_pattern = (u32)array_count(state->cst.patterns);
        array_push(state->cst.patterns,
                   (CstPattern){
                       .kind        = CPK_Value,
                       .token_index = token_index,
                       .a           = true_node,
                   });
        u32 first_pattern = (u32)array_count(state->cst.pattern_items);
        array_push(state->cst.pattern_items, true_pattern);
        array_push(state->cst.on_branches,
                   (CstOnBranch){
                       .pattern_index        = first_pattern,
                       .expr_node_index      = true_expr,
                       .pattern_count        = 1,
                       .guard_node_index     = U32_MAX,
                       .binder_symbol_handle = U32_MAX,
                       .binder_token_index   = U32_MAX,
                   });

        u32 on_index = (u32)array_count(state->cst.ons);
        array_push(state->cst.ons,
                   (CstOnInfo){
                       .kind         = COK_Bool,
                       .first_branch = first_branch,
                       .branch_count = 1,
                   });
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_On,
                                 .token_index = token_index,
                                 .a           = scrutinee,
                                 .b           = on_index,
                             },
                             out_node);
    }
    if (!cst_consume(state, TK_else)) {
        return false;
    }

    u32 false_expr = 0;
    if (!cst_parse_expr_bp(state, 0, &false_expr)) {
        return false;
    }

    u32 true_node    = 0;
    u32 true_pattern = 0;
    if (!cst_emit_node(state,
                       (CstNode){
                           .kind        = CK_BoolLiteral,
                           .token_index = token_index,
                           .a           = 1,
                       },
                       &true_node)) {
        return false;
    }
    true_pattern = (u32)array_count(state->cst.patterns);
    array_push(state->cst.patterns,
               (CstPattern){
                   .kind        = CPK_Value,
                   .token_index = token_index,
                   .a           = true_node,
               });
    u32 first_pattern = (u32)array_count(state->cst.pattern_items);
    array_push(state->cst.pattern_items, true_pattern);
    array_push(state->cst.on_branches,
               (CstOnBranch){
                   .pattern_index        = first_pattern,
                   .expr_node_index      = true_expr,
                   .pattern_count        = 1,
                   .guard_node_index     = U32_MAX,
                   .binder_symbol_handle = U32_MAX,
                   .binder_token_index   = U32_MAX,
               });
    array_push(state->cst.on_branches,
               (CstOnBranch){
                   .expr_node_index      = false_expr,
                   .flags                = COBF_Else,
                   .guard_node_index     = U32_MAX,
                   .binder_symbol_handle = U32_MAX,
                   .binder_token_index   = U32_MAX,
               });

    u32 on_index = (u32)array_count(state->cst.ons);
    array_push(state->cst.ons,
               (CstOnInfo){
                   .kind         = COK_Bool,
                   .first_branch = first_branch,
                   .branch_count = 2,
               });
    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_On,
                             .token_index = token_index,
                             .a           = scrutinee,
                             .b           = on_index,
                         },
                         out_node);
}

internal bool cst_parse_on_branch_expr(CstParseState* state, u32* out_node)
{
    if (cst_current_token(state).kind == TK_return) {
        u32 token_index = state->token_index;
        cst_advance(state);
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_ReturnExpr,
                                 .token_index = token_index,
                                 .a           = U32_MAX,
                             },
                             out_node);
    }

    if (cst_current_token(state).kind == TK_break ||
        cst_current_token(state).kind == TK_continue) {
        u32     token_index = state->token_index;
        CstKind kind        = cst_current_token(state).kind == TK_break
                                  ? CK_BreakExpr
                                  : CK_ContinueExpr;
        cst_advance(state);
        u32 label = U32_MAX;
        if (cst_current_token(state).kind == TK_Dollar) {
            cst_advance(state);
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
            label = cst_current_symbol_handle(state);
            cst_advance(state);
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = kind,
                                 .token_index = token_index,
                                 .a           = U32_MAX,
                                 .b           = label,
                             },
                             out_node);
    }

    bool previous_stop_before_on_branch_head =
        state->stop_before_on_branch_head;
    state->stop_before_on_branch_head = true;
    bool parsed                       = cst_parse_expr_bp(state, 0, out_node);
    state->stop_before_on_branch_head = previous_stop_before_on_branch_head;
    return parsed;
}

internal bool cst_symbol_is_underscore(const Lexer* lexer, u32 symbol_handle)
{
    return string_eq_cstr(lex_symbol(lexer, symbol_handle), "_");
}

internal bool
cst_emit_pattern(CstParseState* state, CstPattern pattern, u32* out_pattern)
{
    *out_pattern = (u32)array_count(state->cst.patterns);
    array_push(state->cst.patterns, pattern);
    return true;
}

internal bool cst_parse_pattern(CstParseState* state, u32* out_pattern);
internal bool cst_parse_destructure_pattern(CstParseState* state,
                                            u32*           out_pattern);

internal bool cst_parse_destructure_tuple_pattern(CstParseState* state,
                                                  u32*           out_pattern)
{
    u32 token_index = state->token_index;
    cst_advance(state);
    u32 first_item = (u32)array_count(state->cst.pattern_items);
    u32 item_count = 0;
    if (cst_current_token(state).kind != TK_RParen) {
        for (;;) {
            u32 item = U32_MAX;
            if (!cst_parse_destructure_pattern(state, &item)) {
                return false;
            }
            array_push(state->cst.pattern_items, item);
            ++item_count;
            if (cst_current_token(state).kind != TK_Comma) {
                break;
            }
            cst_advance(state);
            if (cst_current_token(state).kind == TK_RParen) {
                break;
            }
        }
    }
    if (!cst_consume(state, TK_RParen)) {
        return false;
    }
    return cst_emit_pattern(state,
                            (CstPattern){
                                .kind        = CPK_Tuple,
                                .token_index = token_index,
                                .a           = first_item,
                                .b           = item_count,
                            },
                            out_pattern);
}

internal bool cst_parse_destructure_plex_pattern(CstParseState* state,
                                                 u32*           out_pattern)
{
    u32 token_index = state->token_index;
    cst_advance(state);
    u32 first_field = (u32)array_count(state->cst.pattern_fields);
    u32 field_count = 0;
    while (cst_current_token(state).kind != TK_RBrace) {
        if (cst_current_token(state).kind != TK_Symbol) {
            return false;
        }
        u32 field_token   = state->token_index;
        u32 field_symbol  = cst_current_symbol_handle(state);
        u32 field_pattern = U32_MAX;
        if (cst_symbol_is_underscore(state->lexer, field_symbol)) {
            if (!cst_emit_pattern(state,
                                  (CstPattern){
                                      .kind        = CPK_Ignore,
                                      .token_index = field_token,
                                  },
                                  &field_pattern)) {
                return false;
            }
            cst_advance(state);
        } else if (cst_peek_kind_at(state, 1) == TK_Colon) {
            cst_advance(state);
            if (!cst_consume(state, TK_Colon) ||
                !cst_parse_destructure_pattern(state, &field_pattern)) {
                return false;
            }
        } else {
            if (!cst_emit_pattern(state,
                                  (CstPattern){
                                      .kind        = CPK_Bind,
                                      .token_index = field_token,
                                      .a           = field_symbol,
                                      .b           = U32_MAX,
                                  },
                                  &field_pattern)) {
                return false;
            }
            cst_advance(state);
        }
        array_push(state->cst.pattern_fields,
                   (CstPlexPatternField){
                       .token_index   = field_token,
                       .symbol_handle = field_symbol,
                       .pattern_index = field_pattern,
                   });
        ++field_count;
        if (cst_current_token(state).kind != TK_Comma) {
            break;
        }
        cst_advance(state);
        if (cst_current_token(state).kind == TK_RBrace) {
            break;
        }
    }
    if (!cst_consume(state, TK_RBrace)) {
        return false;
    }
    return cst_emit_pattern(state,
                            (CstPattern){
                                .kind        = CPK_Plex,
                                .token_index = token_index,
                                .a           = first_field,
                                .b           = field_count,
                            },
                            out_pattern);
}

internal bool cst_parse_destructure_pattern(CstParseState* state,
                                            u32*           out_pattern)
{
    if (cst_current_token(state).kind == TK_Symbol) {
        u32 token_index = state->token_index;
        u32 symbol      = cst_current_symbol_handle(state);
        cst_advance(state);
        return cst_emit_pattern(
            state,
            (CstPattern){
                .kind        = cst_symbol_is_underscore(state->lexer, symbol)
                                   ? CPK_Ignore
                                   : CPK_Bind,
                .token_index = token_index,
                .a           = symbol,
                .b           = U32_MAX,
            },
            out_pattern);
    }
    if (cst_current_token(state).kind == TK_LParen) {
        return cst_parse_destructure_tuple_pattern(state, out_pattern);
    }
    if (cst_current_token(state).kind == TK_LBrace) {
        return cst_parse_destructure_plex_pattern(state, out_pattern);
    }
    return cst_parse_pattern(state, out_pattern);
}

internal bool cst_parse_tuple_pattern(CstParseState* state, u32* out_pattern)
{
    u32 token_index = state->token_index;
    cst_advance(state);
    u32 first_item = (u32)array_count(state->cst.pattern_items);
    u32 item_count = 0;
    if (cst_current_token(state).kind != TK_RParen) {
        for (;;) {
            u32 item = U32_MAX;
            if (!cst_parse_pattern(state, &item)) {
                return false;
            }
            array_push(state->cst.pattern_items, item);
            ++item_count;
            if (cst_current_token(state).kind != TK_Comma) {
                break;
            }
            cst_advance(state);
            if (cst_current_token(state).kind == TK_RParen) {
                break;
            }
        }
    }
    if (!cst_consume(state, TK_RParen)) {
        return false;
    }
    return cst_emit_pattern(state,
                            (CstPattern){
                                .kind        = CPK_Tuple,
                                .token_index = token_index,
                                .a           = first_item,
                                .b           = item_count,
                            },
                            out_pattern);
}

internal bool cst_pattern_starts_enum_variant(const CstParseState* state)
{
    if (cst_current_token(state).kind != TK_Symbol) {
        return false;
    }

    u32 index = state->token_index + 1;
    if (index >= array_count(state->lexer->tokens)) {
        return false;
    }

    if (state->lexer->tokens[index].kind == TK_LParen) {
        return true;
    }

    bool saw_dot = false;
    while (index < array_count(state->lexer->tokens) &&
           state->lexer->tokens[index].kind == TK_Dot) {
        saw_dot = true;
        index++;
        if (index >= array_count(state->lexer->tokens) ||
            state->lexer->tokens[index].kind != TK_Symbol) {
            return false;
        }
        index++;
    }

    return saw_dot && index < array_count(state->lexer->tokens) &&
           state->lexer->tokens[index].kind == TK_LParen;
}

internal bool cst_parse_plex_pattern(CstParseState* state, u32* out_pattern)
{
    u32 token_index = state->token_index;
    cst_advance(state);
    u32 first_field = (u32)array_count(state->cst.pattern_fields);
    u32 field_count = 0;
    while (cst_current_token(state).kind != TK_RBrace) {
        if (cst_current_token(state).kind != TK_Symbol) {
            return false;
        }
        u32 field_token   = state->token_index;
        u32 field_symbol  = cst_current_symbol_handle(state);
        u32 field_pattern = U32_MAX;
        if (cst_symbol_is_underscore(state->lexer, field_symbol)) {
            if (!cst_emit_pattern(state,
                                  (CstPattern){
                                      .kind        = CPK_Ignore,
                                      .token_index = field_token,
                                  },
                                  &field_pattern)) {
                return false;
            }
            cst_advance(state);
        } else if (cst_peek_kind_at(state, 1) == TK_Colon) {
            cst_advance(state);
            if (!cst_consume(state, TK_Colon) ||
                !cst_parse_pattern(state, &field_pattern)) {
                return false;
            }
        } else {
            if (!cst_parse_pattern(state, &field_pattern)) {
                return false;
            }
        }
        array_push(state->cst.pattern_fields,
                   (CstPlexPatternField){
                       .token_index   = field_token,
                       .symbol_handle = field_symbol,
                       .pattern_index = field_pattern,
                   });
        ++field_count;
        if (cst_current_token(state).kind == TK_Comma) {
            cst_advance(state);
        }
    }
    if (!cst_consume(state, TK_RBrace)) {
        return false;
    }
    return cst_emit_pattern(state,
                            (CstPattern){
                                .kind        = CPK_Plex,
                                .token_index = token_index,
                                .a           = first_field,
                                .b           = field_count,
                            },
                            out_pattern);
}

internal bool cst_parse_enum_variant_pattern(CstParseState* state,
                                             u32*           out_pattern)
{
    u32 token_index = state->token_index;
    u32 symbol      = cst_current_symbol_handle(state);
    u32 qualifier   = U32_MAX;
    cst_advance(state);

    if (cst_current_token(state).kind == TK_Dot) {
        u32 left = U32_MAX;
        if (!cst_emit_node(state,
                           (CstNode){
                               .kind        = CK_SymbolRef,
                               .token_index = token_index,
                               .a           = symbol,
                           },
                           &left)) {
            return false;
        }

        while (cst_current_token(state).kind == TK_Dot) {
            u32 dot_token = state->token_index;
            cst_advance(state);
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
            symbol = cst_current_symbol_handle(state);
            if (cst_peek_kind_at(state, 1) == TK_Dot) {
                u32 field = U32_MAX;
                if (!cst_emit_node(state,
                                   (CstNode){
                                       .kind        = CK_Field,
                                       .token_index = dot_token + 1,
                                       .a           = left,
                                       .b           = symbol,
                                   },
                                   &field)) {
                    return false;
                }
                left = field;
                cst_advance(state);
                continue;
            }
            qualifier = left;
            cst_advance(state);
            break;
        }
    }

    if (!cst_consume(state, TK_LParen)) {
        return false;
    }

    u32 first_pattern = (u32)array_count(state->cst.pattern_items);
    u32 pattern_count = 0;
    if (cst_current_token(state).kind != TK_RParen) {
        for (;;) {
            u32 item = U32_MAX;
            if (!cst_parse_pattern(state, &item)) {
                return false;
            }
            array_push(state->cst.pattern_items, item);
            pattern_count++;
            if (cst_current_token(state).kind != TK_Comma) {
                break;
            }
            cst_advance(state);
            if (cst_current_token(state).kind == TK_RParen) {
                break;
            }
        }
    }
    if (!cst_consume(state, TK_RParen)) {
        return false;
    }

    u32 enum_pattern_index = (u32)array_count(state->cst.enum_patterns);
    array_push(state->cst.enum_patterns,
               (CstEnumPattern){
                   .token_index          = token_index,
                   .qualifier_node_index = qualifier,
                   .symbol_handle        = symbol,
                   .first_pattern        = first_pattern,
                   .pattern_count        = pattern_count,
               });
    return cst_emit_pattern(state,
                            (CstPattern){
                                .kind        = CPK_EnumVariant,
                                .token_index = token_index,
                                .a           = enum_pattern_index,
                            },
                            out_pattern);
}

internal CstPatternKind cst_comparison_pattern_kind(TokenKind kind)
{
    switch (kind) {
    case TK_EqualEqual:
        return CPK_Equal;
    case TK_BangEqual:
        return CPK_NotEqual;
    case TK_Less:
        return CPK_Less;
    case TK_LessEqual:
        return CPK_LessEqual;
    case TK_Greater:
        return CPK_Greater;
    case TK_GreaterEqual:
        return CPK_GreaterEqual;
    default:
        return CPK_Value;
    }
}

internal bool cst_parse_pattern(CstParseState* state, u32* out_pattern)
{
    if (cst_current_token(state).kind == TK_as) {
        cst_advance(state);
        if (cst_current_token(state).kind != TK_Symbol ||
            cst_symbol_is_underscore(state->lexer,
                                     cst_current_symbol_handle(state))) {
            return false;
        }
        u32 token_index = state->token_index;
        u32 symbol      = cst_current_symbol_handle(state);
        cst_advance(state);
        return cst_emit_pattern(state,
                                (CstPattern){
                                    .kind        = CPK_Bind,
                                    .token_index = token_index,
                                    .a           = symbol,
                                    .b           = U32_MAX,
                                },
                                out_pattern);
    }

    CstPatternKind comparison_kind =
        cst_comparison_pattern_kind(cst_current_token(state).kind);
    if (comparison_kind != CPK_Value) {
        u32 token_index = state->token_index;
        cst_advance(state);
        u32 value_node = 0;
        if (!cst_parse_expr_bp(state, 0, &value_node)) {
            return false;
        }
        return cst_emit_pattern(state,
                                (CstPattern){
                                    .kind        = comparison_kind,
                                    .token_index = token_index,
                                    .a           = value_node,
                                },
                                out_pattern);
    }

    if (cst_current_token(state).kind == TK_Symbol &&
        cst_symbol_is_underscore(state->lexer,
                                 cst_current_symbol_handle(state))) {
        u32 token_index = state->token_index;
        cst_advance(state);
        return cst_emit_pattern(state,
                                (CstPattern){
                                    .kind        = CPK_Ignore,
                                    .token_index = token_index,
                                },
                                out_pattern);
    }

    if (cst_current_token(state).kind == TK_LParen) {
        return cst_parse_tuple_pattern(state, out_pattern);
    }
    if (cst_current_token(state).kind == TK_LBrace) {
        return cst_parse_plex_pattern(state, out_pattern);
    }
    if (cst_pattern_starts_enum_variant(state)) {
        return cst_parse_enum_variant_pattern(state, out_pattern);
    }

    u32 start_node = 0;
    if (!cst_parse_expr_bp(state, 0, &start_node)) {
        return false;
    }

    CstPatternKind range_kind = CPK_Value;
    if (cst_current_token(state).kind == TK_Range) {
        range_kind = CPK_RangeExclusive;
    } else if (cst_current_token(state).kind == TK_RangeInclusive) {
        range_kind = CPK_RangeInclusive;
    } else {
        return cst_emit_pattern(
            state,
            (CstPattern){
                .kind        = CPK_Value,
                .token_index = state->cst.nodes[start_node].token_index,
                .a           = start_node,
            },
            out_pattern);
    }

    u32 token_index = state->token_index;
    cst_advance(state);
    u32 end_node = 0;
    if (!cst_parse_expr_bp(state, 0, &end_node)) {
        return false;
    }

    return cst_emit_pattern(state,
                            (CstPattern){
                                .kind        = range_kind,
                                .token_index = token_index,
                                .a           = start_node,
                                .b           = end_node,
                            },
                            out_pattern);
}

//------------------------------------------------------------------------------
// Parse one expression using Pratt binding powers.

internal bool cst_parse_expr_bp(CstParseState* state, u8 min_bp, u32* out_node)
{
    u32 left = 0;
    if (!cst_parse_prefix(state, &left)) {
        return false;
    }

    while (true) {
        Token token    = cst_current_token(state);
        u8    left_bp  = 0;
        u8    right_bp = 0;

        if (state->stop_before_on_branch_head &&
            cst_current_token_starts_on_branch_head(state)) {
            break;
        }

        if (state->stop_before_call && token.kind == TK_LParen) {
            break;
        }

        if (state->stop_before_ffi_name && token.kind == TK_Symbol &&
            cst_peek_kind_at(state, 1) == TK_LParen) {
            break;
        }

        if (token.kind == TK_LBrace) {
            bool starts_plex = state->cst.nodes[left].kind == CK_SymbolRef &&
                               (cst_peek_kind_at(state, 1) == TK_RBrace ||
                                (cst_peek_kind_at(state, 1) == TK_Symbol &&
                                 cst_peek_kind_at(state, 2) == TK_Colon));
            if (!starts_plex) {
                break;
            }
        }

        bool caret_is_postfix =
            token.kind == TK_Caret && cst_caret_is_postfix_deref(state);
        if ((!caret_is_postfix &&
             !cst_infix_binding_power(token.kind, &left_bp, &right_bp)) ||
            left_bp < min_bp ||
            (!cst_postfix_token_can_cross_statement_boundary(token.kind) &&
             left_bp >= CST_BP_PREFIX + 10 &&
             cst_token_has_newline_before(state, state->token_index))) {
            if ((token.kind == TK_String ||
                 token.kind == TK_InterpolatedStringStart) &&
                cst_node_is_stringish(&state->cst, left)) {
                u32 right = 0;
                if (!cst_parse_prefix(state, &right)) {
                    return false;
                }
                if (!cst_emit_node(
                        state,
                        (CstNode){
                            .kind        = CK_StringConcat,
                            .token_index = state->cst.nodes[right].token_index,
                            .a           = left,
                            .b           = right,
                        },
                        &left)) {
                    return false;
                }
                continue;
            }
            if (cst_starts_binding(state)) {
                break;
            }
            if (state->allow_statement_boundary &&
                cst_token_starts_expression(token.kind)) {
                break;
            }
            break;
        }

        if (caret_is_postfix) {
            left_bp  = CST_BP_PREFIX + 10;
            right_bp = CST_BP_PREFIX + 10;
        }

        u32 token_index = state->token_index;
        u32 right       = 0;
        cst_advance(state);
        if (token.kind == TK_LParen) {
            u32 first_arg = (u32)array_count(state->cst.call_args);
            u32 arg_count = 0;
            if (cst_current_token(state).kind != TK_RParen) {
                for (;;) {
                    if (!cst_parse_expr_bp(state, 0, &right)) {
                        return false;
                    }
                    array_push(state->cst.call_args, right);
                    ++arg_count;
                    if (cst_current_token(state).kind == TK_Comma) {
                        cst_advance(state);
                        continue;
                    }
                    break;
                }
            }
            if (!cst_consume(state, TK_RParen)) {
                return false;
            }

            u32 call_index = (u32)array_count(state->cst.calls);
            array_push(state->cst.calls,
                       (CstCallInfo){
                           .first_arg = first_arg,
                           .arg_count = arg_count,
                       });

            if (!cst_emit_node(state,
                               (CstNode){
                                   .kind        = CK_Call,
                                   .token_index = token_index,
                                   .a           = left,
                                   .b           = call_index,
                               },
                               &left)) {
                return false;
            }
            continue;
        }
        if (token.kind == TK_LBracket) {
            u32  start    = U32_MAX;
            u32  end      = U32_MAX;
            bool is_slice = false;
            if (cst_current_token(state).kind == TK_Range) {
                is_slice = true;
            } else {
                if (!cst_parse_expr_bp(state, 0, &start)) {
                    return false;
                }
                if (cst_current_token(state).kind == TK_Range) {
                    is_slice = true;
                }
            }
            if (is_slice) {
                if (!cst_consume(state, TK_Range)) {
                    return false;
                }
                if (cst_token_starts_expression(
                        cst_current_token(state).kind)) {
                    if (!cst_parse_expr_bp(state, 0, &end)) {
                        return false;
                    }
                }
                if (!cst_consume(state, TK_RBracket)) {
                    return false;
                }
                u32 slice_index = (u32)array_count(state->cst.slices);
                array_push(state->cst.slices,
                           (CstSliceInfo){
                               .target_node_index = left,
                               .start_node_index  = start,
                               .end_node_index    = end,
                           });
                if (!cst_emit_node(state,
                                   (CstNode){
                                       .kind        = CK_Slice,
                                       .token_index = token_index,
                                       .a           = slice_index,
                                   },
                                   &left)) {
                    return false;
                }
                continue;
            }
            if (!cst_consume(state, TK_RBracket)) {
                return false;
            }
            if (!cst_emit_node(state,
                               (CstNode){
                                   .kind        = CK_Index,
                                   .token_index = token_index,
                                   .a           = left,
                                   .b           = start,
                               },
                               &left)) {
                return false;
            }
            continue;
        }
        if (token.kind == TK_LBrace) {
            u32 first_field = (u32)array_count(state->cst.plex_literal_fields);
            u32 field_count = 0;
            while (cst_current_token(state).kind != TK_RBrace) {
                if (cst_current_token(state).kind != TK_Symbol) {
                    return false;
                }
                u32 field_token  = state->token_index;
                u32 field_symbol = cst_current_symbol_handle(state);
                cst_advance(state);
                if (!cst_consume(state, TK_Colon)) {
                    return false;
                }
                if (!cst_parse_expr_bp(state, 0, &right)) {
                    return false;
                }
                array_push(state->cst.plex_literal_fields,
                           (CstPlexLiteralField){
                               .token_index      = field_token,
                               .symbol_handle    = field_symbol,
                               .value_node_index = right,
                           });
                field_count++;
                if (cst_current_token(state).kind == TK_Comma) {
                    cst_advance(state);
                    continue;
                }
                if (cst_current_token(state).kind == TK_Symbol &&
                    cst_peek_kind_at(state, 1) == TK_Colon) {
                    continue;
                }
                break;
            }
            if (!cst_consume(state, TK_RBrace)) {
                return false;
            }
            u32 literal_index = (u32)array_count(state->cst.plex_literals);
            array_push(state->cst.plex_literals,
                       (CstPlexLiteralInfo){
                           .target_node_index = left,
                           .first_field       = first_field,
                           .field_count       = field_count,
                       });
            if (!cst_emit_node(state,
                               (CstNode){
                                   .kind        = CK_Plex,
                                   .token_index = token_index,
                                   .a           = literal_index,
                               },
                               &left)) {
                return false;
            }
            continue;
        }
        if (token.kind == TK_with) {
            if (!cst_consume(state, TK_LBrace)) {
                return false;
            }
            u32 first_field = (u32)array_count(state->cst.plex_literal_fields);
            u32 field_count = 0;
            while (cst_current_token(state).kind != TK_RBrace) {
                if (cst_current_token(state).kind != TK_Symbol) {
                    return false;
                }
                u32 field_token  = state->token_index;
                u32 field_symbol = cst_current_symbol_handle(state);
                cst_advance(state);
                if (!cst_consume(state, TK_Colon)) {
                    return false;
                }
                if (!cst_parse_expr_bp(state, 0, &right)) {
                    return false;
                }
                array_push(state->cst.plex_literal_fields,
                           (CstPlexLiteralField){
                               .token_index      = field_token,
                               .symbol_handle    = field_symbol,
                               .value_node_index = right,
                           });
                field_count++;
                if (cst_current_token(state).kind == TK_Comma) {
                    cst_advance(state);
                    continue;
                }
                if (cst_current_token(state).kind == TK_Symbol &&
                    cst_peek_kind_at(state, 1) == TK_Colon) {
                    continue;
                }
                break;
            }
            if (!cst_consume(state, TK_RBrace)) {
                return false;
            }
            u32 literal_index = (u32)array_count(state->cst.plex_literals);
            array_push(state->cst.plex_literals,
                       (CstPlexLiteralInfo){
                           .target_node_index = left,
                           .first_field       = first_field,
                           .field_count       = field_count,
                       });
            if (!cst_emit_node(state,
                               (CstNode){
                                   .kind        = CK_PlexUpdate,
                                   .token_index = token_index,
                                   .a           = literal_index,
                               },
                               &left)) {
                return false;
            }
            continue;
        }
        if (token.kind == TK_Dot) {
            if (cst_current_token(state).kind == TK_Integer) {
                u32 integer_index = cst_current_integer_index(state);
                if (integer_index == CST_NO_VALUE) {
                    return false;
                }
                u64 field_index = state->lexer->integers[integer_index];
                if (field_index > UINT32_MAX) {
                    return false;
                }
                cst_advance(state);
                if (!cst_emit_node(state,
                                   (CstNode){
                                       .kind        = CK_TupleField,
                                       .token_index = token_index + 1,
                                       .a           = left,
                                       .b           = (u32)field_index,
                                   },
                                   &left)) {
                    return false;
                }
                continue;
            }
            if (cst_current_token(state).kind == TK_as) {
                cst_advance(state);
                if (!cst_consume(state, TK_LParen) ||
                    !cst_parse_type(state, &right) ||
                    !cst_consume(state, TK_RParen)) {
                    return false;
                }
                if (!cst_emit_node(state,
                                   (CstNode){
                                       .kind        = CK_Cast,
                                       .token_index = token_index,
                                       .a           = left,
                                       .b           = right,
                                   },
                                   &left)) {
                    return false;
                }
                continue;
            }
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
            u32 symbol_handle = cst_current_symbol_handle(state);
            cst_advance(state);
            if (!cst_emit_node(state,
                               (CstNode){
                                   .kind        = CK_Field,
                                   .token_index = token_index + 1,
                                   .a           = left,
                                   .b           = symbol_handle,
                               },
                               &left)) {
                return false;
            }
            continue;
        }

        if (token.kind == TK_Caret &&
            cst_consumed_caret_is_postfix_deref(state)) {
            if (!cst_emit_node(state,
                               (CstNode){
                                   .kind        = CK_Deref,
                                   .token_index = token_index,
                                   .a           = left,
                               },
                               &left)) {
                return false;
            }
            continue;
        }

        if (!cst_parse_expr_bp(state, right_bp, &right)) {
            return false;
        }

        CstKind kind = CK_IntegerPlus;
        switch (token.kind) {
        case TK_Plus:
            kind = CK_IntegerPlus;
            break;
        case TK_Minus:
            kind = CK_IntegerMinus;
            break;
        case TK_Star:
            kind = CK_IntegerMultiply;
            break;
        case TK_Slash:
            kind = CK_IntegerDivide;
            break;
        case TK_Percent:
            kind = CK_IntegerModulo;
            break;
        case TK_Amp:
            kind = CK_BitwiseAnd;
            break;
        case TK_Caret:
            kind = CK_BitwiseXor;
            break;
        case TK_Pipe:
            kind = CK_BitwiseOr;
            break;
        case TK_EqualEqual:
            kind = CK_Equal;
            break;
        case TK_BangEqual:
            kind = CK_NotEqual;
            break;
        case TK_Less:
            kind = CK_Less;
            break;
        case TK_LessEqual:
            kind = CK_LessEqual;
            break;
        case TK_Greater:
            kind = CK_Greater;
            break;
        case TK_GreaterEqual:
            kind = CK_GreaterEqual;
            break;
        case TK_AmpAmp:
            kind = CK_LogicalAnd;
            break;
        case TK_PipePipe:
            kind = CK_LogicalOr;
            break;
        default:
            return false;
        }

        if (!cst_emit_node(state,
                           (CstNode){
                               .kind        = kind,
                               .token_index = token_index,
                               .a           = left,
                               .b           = right,
                           },
                           &left)) {
            return false;
        }
    }

    *out_node = left;
    return true;
}

internal bool cst_parse_block_statement(CstParseState* state);

internal bool cst_parse_nested_block(CstParseState* state, u32* out_node)
{
    u32 block_token_index = state->token_index;
    if (!cst_consume(state, TK_LBrace)) {
        return false;
    }

    u32 block_index = 0;
    if (!cst_emit_node(state,
                       (CstNode){
                           .kind        = CK_Block,
                           .token_index = block_token_index,
                       },
                       &block_index)) {
        return false;
    }
    u32 first_statement = (u32)array_count(state->cst.nodes);

    while (cst_current_token(state).kind != TK_RBrace) {
        if (!cst_parse_block_statement(state)) {
            return false;
        }

        if (cst_current_token(state).kind == TK_EOF) {
            return false;
        }
        if (cst_current_token(state).kind == TK_RBrace) {
            break;
        }
    }

    if (!cst_consume(state, TK_RBrace)) {
        return false;
    }

    state->cst.nodes[block_index].a = first_statement;
    state->cst.nodes[block_index].b = (u32)array_count(state->cst.nodes);
    if (out_node) {
        *out_node = block_index;
    }
    return true;
}

internal bool cst_parse_for_clause_item(CstParseState* state,
                                        bool           allow_declaration,
                                        u32*           out_node,
                                        bool*          out_raw_expr)
{
    u32 token_index = state->token_index;

    if (allow_declaration && cst_starts_binding(state)) {
        *out_raw_expr = false;
        if (cst_starts_variable(state) && !cst_starts_annotated_bind(state)) {
            return cst_parse_variable(state, out_node);
        }
        return cst_parse_bind(state, out_node);
    }

    if (allow_declaration && cst_starts_variable(state)) {
        *out_raw_expr = false;
        return cst_parse_variable(state, out_node);
    }

    if (cst_starts_assignment(state)) {
        u32 symbol_handle = cst_current_symbol_handle(state);
        u32 value         = 0;
        if (symbol_handle == CST_NO_VALUE) {
            return false;
        }
        cst_advance(state);
        cst_advance(state);
        if (!cst_parse_expr_bp(state, 0, &value)) {
            return false;
        }
        *out_raw_expr = false;
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_Assign,
                                 .token_index = token_index,
                                 .a           = symbol_handle,
                                 .b           = value,
                             },
                             out_node);
    }

    *out_raw_expr = true;
    return cst_parse_expr_bp(state, 0, out_node);
}

internal bool
cst_wrap_for_expr_item(CstParseState* state, u32 token_index, u32* in_out_node)
{
    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_Statement,
                             .token_index = token_index,
                             .a           = *in_out_node,
                         },
                         in_out_node);
}

internal bool cst_parse_for_item_list(CstParseState* state,
                                      bool           allow_declaration,
                                      TokenKind      terminator,
                                      u32            first_node,
                                      bool           first_raw_expr,
                                      u32            first_token_index,
                                      u32*           out_first_item,
                                      u32*           out_item_count)
{
    u32 first_item = (u32)array_count(state->cst.for_items);
    u32 item_count = 0;
    u32 item       = first_node;
    if (first_raw_expr &&
        !cst_wrap_for_expr_item(state, first_token_index, &item)) {
        return false;
    }
    array_push(state->cst.for_items, item);
    item_count++;

    while (cst_current_token(state).kind == TK_Comma) {
        cst_advance(state);
        u32  next_item        = 0;
        bool next_is_raw_expr = false;
        u32  next_token_index = state->token_index;
        if (!cst_parse_for_clause_item(
                state, allow_declaration, &next_item, &next_is_raw_expr)) {
            return false;
        }
        if (next_is_raw_expr &&
            !cst_wrap_for_expr_item(state, next_token_index, &next_item)) {
            return false;
        }
        array_push(state->cst.for_items, next_item);
        item_count++;
    }

    if (cst_current_token(state).kind != terminator &&
        !(terminator == TK_LBrace &&
          cst_current_token(state).kind == TK_Dollar)) {
        return false;
    }

    *out_first_item = first_item;
    *out_item_count = item_count;
    return true;
}

internal bool cst_token_is_for_body_start(TokenKind kind)
{
    return kind == TK_LBrace || kind == TK_Dollar;
}

internal bool
cst_parse_for_body(CstParseState* state, CstForInfo* for_info, u32* out_body)
{
    if (cst_current_token(state).kind == TK_Dollar) {
        cst_advance(state);
        if (cst_current_token(state).kind != TK_Symbol) {
            return false;
        }
        for_info->label_symbol = cst_current_symbol_handle(state);
        cst_advance(state);
    }

    return cst_current_token(state).kind == TK_LBrace &&
           cst_parse_nested_block(state, out_body);
}

internal bool cst_parse_for(CstParseState* state, u32* out_node)
{
    u32        token_index = state->token_index;
    u32        for_node    = 0;
    u32        body        = 0;
    CstForInfo for_info    = {
           .mode                 = CFM_Condition,
           .first_init           = U32_MAX,
           .init_count           = 0,
           .condition_node_index = U32_MAX,
           .first_update         = U32_MAX,
           .update_count         = 0,
           .iterable_node_index  = U32_MAX,
           .item_symbol          = U32_MAX,
           .item_token_index     = U32_MAX,
           .label_symbol         = U32_MAX,
           .else_block_index     = U32_MAX,
           .item_is_pointer      = false,
    };
    if (!cst_emit_node(state,
                       (CstNode){
                           .kind        = CK_For,
                           .token_index = token_index,
                       },
                       &for_node)) {
        return false;
    }
    cst_advance(state);
    bool starts_for_in = cst_current_token(state).kind == TK_Symbol &&
                         cst_peek_kind_at(state, 1) == TK_in;
    if (cst_current_token(state).kind == TK_Caret &&
        cst_peek_kind_at(state, 1) == TK_Symbol &&
        cst_peek_kind_at(state, 2) == TK_in) {
        starts_for_in = true;
    }
    if (!cst_token_is_for_body_start(cst_current_token(state).kind) &&
        starts_for_in) {
        for_info.mode = CFM_In;
        if (cst_current_token(state).kind == TK_Caret) {
            for_info.item_is_pointer = true;
            cst_advance(state);
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
        }
        for_info.item_symbol      = cst_current_symbol_handle(state);
        for_info.item_token_index = state->token_index;
        cst_advance(state);
        if (!cst_consume(state, TK_in)) {
            return false;
        }
        if (!cst_parse_expr_bp(state, 0, &for_info.iterable_node_index)) {
            return false;
        }
    } else if (!cst_token_is_for_body_start(cst_current_token(state).kind)) {
        if (cst_current_token(state).kind == TK_Semicolon) {
            for_info.mode = CFM_CStyle;
            if (!cst_consume(state, TK_Semicolon)) {
                return false;
            }
            if (cst_current_token(state).kind != TK_Semicolon) {
                if (!cst_parse_expr_bp(
                        state, 0, &for_info.condition_node_index)) {
                    return false;
                }
            }
            if (!cst_consume(state, TK_Semicolon)) {
                return false;
            }
            if (!cst_token_is_for_body_start(cst_current_token(state).kind)) {
                u32  update_node        = 0;
                bool update_raw_expr    = false;
                u32  update_token_index = state->token_index;
                if (!cst_parse_for_clause_item(
                        state, false, &update_node, &update_raw_expr) ||
                    !cst_parse_for_item_list(state,
                                             false,
                                             TK_LBrace,
                                             update_node,
                                             update_raw_expr,
                                             update_token_index,
                                             &for_info.first_update,
                                             &for_info.update_count)) {
                    return false;
                }
            }
        } else {
            u32  first_node        = 0;
            bool first_raw_expr    = false;
            u32  first_token_index = state->token_index;
            if (!cst_parse_for_clause_item(
                    state, true, &first_node, &first_raw_expr)) {
                return false;
            }

            if (cst_token_is_for_body_start(cst_current_token(state).kind)) {
                if (!first_raw_expr) {
                    return false;
                }
                for_info.condition_node_index = first_node;
            } else if (cst_current_token(state).kind == TK_Comma ||
                       cst_current_token(state).kind == TK_Semicolon) {
                for_info.mode = CFM_CStyle;
                if (cst_current_token(state).kind == TK_Comma) {
                    if (!cst_parse_for_item_list(state,
                                                 true,
                                                 TK_Semicolon,
                                                 first_node,
                                                 first_raw_expr,
                                                 first_token_index,
                                                 &for_info.first_init,
                                                 &for_info.init_count)) {
                        return false;
                    }
                } else {
                    u32 init_item = first_node;
                    if (first_raw_expr &&
                        !cst_wrap_for_expr_item(
                            state, first_token_index, &init_item)) {
                        return false;
                    }
                    for_info.first_init =
                        (u32)array_count(state->cst.for_items);
                    for_info.init_count = 1;
                    array_push(state->cst.for_items, init_item);
                }

                if (!cst_consume(state, TK_Semicolon)) {
                    return false;
                }

                if (cst_current_token(state).kind != TK_Semicolon) {
                    if (!cst_parse_expr_bp(
                            state, 0, &for_info.condition_node_index)) {
                        return false;
                    }
                }
                if (!cst_consume(state, TK_Semicolon)) {
                    return false;
                }

                if (!cst_token_is_for_body_start(
                        cst_current_token(state).kind)) {
                    u32  update_node        = 0;
                    bool update_raw_expr    = false;
                    u32  update_token_index = state->token_index;
                    if (!cst_parse_for_clause_item(
                            state, false, &update_node, &update_raw_expr) ||
                        !cst_parse_for_item_list(state,
                                                 false,
                                                 TK_LBrace,
                                                 update_node,
                                                 update_raw_expr,
                                                 update_token_index,
                                                 &for_info.first_update,
                                                 &for_info.update_count)) {
                        return false;
                    }
                }
            } else {
                return false;
            }
        }
    }
    if (!cst_parse_for_body(state, &for_info, &body)) {
        return false;
    }
    if (cst_current_token(state).kind == TK_else) {
        cst_advance(state);
        if (!cst_parse_nested_block(state, &for_info.else_block_index)) {
            return false;
        }
    }
    u32 for_info_index = (u32)array_count(state->cst.fors);
    array_push(state->cst.fors, for_info);
    state->cst.nodes[for_node].a = for_info_index;
    state->cst.nodes[for_node].b = body;
    if (out_node) {
        *out_node = for_node;
    }
    return true;
}

internal bool cst_parse_block_statement(CstParseState* state)
{
    u32 token_index = state->token_index;

    if (cst_current_token(state).kind == TK_ffi) {
        return cst_parse_ffi_def(state, NULL);
    }

    if (cst_current_token(state).kind == TK_use) {
        return cst_parse_use(state, NULL);
    }

    if (cst_current_token(state).kind == TK_break ||
        cst_current_token(state).kind == TK_continue) {
        CstKind kind =
            cst_current_token(state).kind == TK_break ? CK_Break : CK_Continue;
        cst_advance(state);
        u32 payload = U32_MAX;
        u32 label   = U32_MAX;
        if (cst_current_token(state).kind == TK_Dollar) {
            cst_advance(state);
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
            label = cst_current_symbol_handle(state);
            cst_advance(state);
        }
        if (kind == CK_Break &&
            cst_token_starts_expression(cst_current_token(state).kind)) {
            if (!cst_parse_expr_bp(state, 0, &payload)) {
                return false;
            }
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = kind,
                                 .token_index = token_index,
                                 .a           = payload,
                                 .b           = label,
                             },
                             NULL);
    }

    if (cst_current_token(state).kind == TK_return) {
        u32 expr = 0;
        cst_advance(state);
        if (!cst_parse_expr_bp(state, 0, &expr)) {
            return false;
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_Return,
                                 .token_index = token_index,
                                 .a           = expr,
                             },
                             NULL);
    }

    if (cst_current_token(state).kind == TK_LParen ||
        cst_current_token(state).kind == TK_LBrace) {
        u32 cursor = state->token_index;
        if (cst_skip_balanced_tokens(state,
                                     &cursor,
                                     cst_current_token(state).kind,
                                     cst_current_token(state).kind == TK_LParen
                                         ? TK_RParen
                                         : TK_RBrace) &&
            (cst_kind_at_stream_index(state, cursor) == TK_Colon ||
             cst_kind_at_stream_index(state, cursor) == TK_Equal)) {
            return cst_parse_destructure(state, NULL);
        }
    }

    if (cst_current_token(state).kind == TK_LBrace) {
        return cst_parse_nested_block(state, NULL);
    }

    if (cst_current_token(state).kind == TK_for) {
        return cst_parse_for(state, NULL);
    }

    if (cst_starts_binding(state)) {
        if (cst_starts_variable(state) && !cst_starts_annotated_bind(state)) {
            u32 symbol_handle = cst_current_symbol_handle(state);
            u32 payload       = 0;
            if (symbol_handle == CST_NO_VALUE) {
                return false;
            }
            cst_advance(state);
            if (!cst_parse_variable_payload(state, token_index, &payload)) {
                return false;
            }
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_Variable,
                                     .token_index = token_index,
                                     .a           = symbol_handle,
                                     .b           = payload,
                                 },
                                 NULL);
        }

        return cst_parse_bind(state, NULL);
    }

    if (cst_starts_variable(state)) {
        u32 symbol_handle = cst_current_symbol_handle(state);
        u32 payload       = 0;
        if (symbol_handle == CST_NO_VALUE) {
            return false;
        }
        cst_advance(state);
        if (!cst_parse_variable_payload(state, token_index, &payload)) {
            return false;
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_Variable,
                                 .token_index = token_index,
                                 .a           = symbol_handle,
                                 .b           = payload,
                             },
                             NULL);
    }

    if (cst_starts_assignment(state)) {
        u32 symbol_handle = cst_current_symbol_handle(state);
        u32 value         = 0;
        if (symbol_handle == CST_NO_VALUE) {
            return false;
        }
        cst_advance(state);
        cst_advance(state);
        if (!cst_parse_expr_bp(state, 0, &value)) {
            return false;
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_Assign,
                                 .token_index = token_index,
                                 .a           = symbol_handle,
                                 .b           = value,
                             },
                             NULL);
    }

    u32 expr = 0;
    if (!cst_parse_expr_bp(state, 0, &expr)) {
        return false;
    }
    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_Statement,
                             .token_index = token_index,
                             .a           = expr,
                         },
                         NULL);
}

//------------------------------------------------------------------------------
// Parse one function block body as a sequence of statements.

internal bool cst_parse_fn_block(CstParseState* state,
                                 u32            fn_token_index,
                                 u32            signature_index,
                                 u32*           out_node)
{
    u32 block_node = 0;
    if (!cst_parse_nested_block(state, &block_node)) {
        return false;
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_FnBlock,
                             .token_index = fn_token_index,
                             .a           = signature_index,
                             .b           = block_node,
                         },
                         out_node);
}

//------------------------------------------------------------------------------
// Parse one function expression.

internal bool cst_parse_fn_expr(CstParseState* state, u32* out_node)
{
    u32 token_index     = state->token_index;
    u32 body            = 0;
    u32 signature_index = 0;

    if (!cst_parse_fn_signature(state, true, false, &signature_index)) {
        return false;
    }

    if (cst_current_token(state).kind == TK_FatArrow) {
        cst_advance(state);
        if (!cst_parse_expr_bp(state, 0, &body)) {
            return false;
        }

        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_FnExpr,
                                 .token_index = token_index,
                                 .a           = signature_index,
                                 .b           = body,
                             },
                             out_node);
    }

    if (cst_current_token(state).kind == TK_LBrace) {
        return cst_parse_fn_block(
            state, token_index, signature_index, out_node);
    }

    return false;
}

//------------------------------------------------------------------------------
// Parse one binding value, which may currently be either a function expression
// or a plain expression.

internal bool cst_parse_value(CstParseState* state, u32* out_node)
{
    if (cst_remaining_bind_value_is_type_syntax(state)) {
        return cst_parse_type(state, out_node);
    }

    if (cst_current_token(state).kind == TK_fn) {
        return cst_parse_fn_expr(state, out_node);
    }

    if (cst_current_token(state).kind == TK_ffi) {
        return cst_parse_ffi_def(state, out_node);
    }

    if (cst_current_token(state).kind == TK_mod) {
        return cst_parse_mod_ref(state, out_node);
    }

    return cst_parse_expr_bp(state, 0, out_node);
}

internal bool cst_parse_mod_ref(CstParseState* state, u32* out_node)
{
    u32 token_index  = state->token_index;
    u32 first_symbol = (u32)array_count(state->cst.module_path_symbols);
    u32 symbol_count = 0;
    cst_advance(state);

    if (cst_current_token(state).kind != TK_Symbol) {
        return false;
    }

    for (;;) {
        u32 symbol_handle = cst_current_symbol_handle(state);
        if (symbol_handle == CST_NO_VALUE) {
            return false;
        }
        array_push(state->cst.module_path_symbols, symbol_handle);
        ++symbol_count;
        cst_advance(state);
        if (cst_current_token(state).kind != TK_Dot) {
            break;
        }
        cst_advance(state);
        if (cst_current_token(state).kind != TK_Symbol) {
            return false;
        }
    }

    u32 module_path_index = (u32)array_count(state->cst.module_paths);
    array_push(state->cst.module_paths,
               (CstModulePath){
                   .first_symbol = first_symbol,
                   .symbol_count = symbol_count,
               });
    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_ModRef,
                             .token_index = token_index,
                             .a           = module_path_index,
                         },
                         out_node);
}

internal bool cst_parse_ffi_def(CstParseState* state, u32* out_node)
{
    u32 token_index = state->token_index;
    cst_advance(state);

    bool old_stop_before_ffi_name = state->stop_before_ffi_name;
    state->stop_before_ffi_name   = true;
    u32  library_node_index       = 0;
    bool parsed_library = cst_parse_expr_bp(state, 0, &library_node_index);
    state->stop_before_ffi_name = old_stop_before_ffi_name;
    if (!parsed_library) {
        return false;
    }

    if (cst_current_token(state).kind != TK_Symbol) {
        return false;
    }
    u32 symbol_handle = cst_current_symbol_handle(state);
    if (symbol_handle == CST_NO_VALUE) {
        return false;
    }
    cst_advance(state);

    u32 signature_index = 0;
    if (!cst_parse_callable_signature(
            state, TK_EOF, false, false, &signature_index)) {
        return false;
    }

    u32 ffi_info_index = (u32)array_count(state->cst.ffi_infos);
    array_push(state->cst.ffi_infos,
               (CstFfiInfo){
                   .library_node_index = library_node_index,
                   .symbol_handle      = symbol_handle,
                   .signature_index    = signature_index,
               });

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_FfiDef,
                             .token_index = token_index,
                             .a           = ffi_info_index,
                         },
                         out_node);
}

internal bool
cst_parse_variable_payload(CstParseState* state, u32 token_index, u32* out_node)
{
    u32 type_node = CST_NO_VALUE;
    u32 value     = 0;

    if (!cst_consume(state, TK_Colon)) {
        return false;
    }

    if (cst_current_token(state).kind == TK_Equal) {
        cst_advance(state);
        return cst_parse_expr_bp(state, 0, out_node);
    }

    if (!cst_parse_type(state, &type_node)) {
        return false;
    }

    if (cst_current_token(state).kind == TK_Equal) {
        cst_advance(state);
        if (!cst_parse_expr_bp(state, 0, &value)) {
            return false;
        }
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_AnnotatedValue,
                                 .token_index = token_index,
                                 .a           = type_node,
                                 .b           = value,
                             },
                             out_node);
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_ZeroInit,
                             .token_index = token_index,
                             .a           = type_node,
                         },
                         out_node);
}

//------------------------------------------------------------------------------
// Parse one top-level binding.

internal bool cst_parse_bind(CstParseState* state, u32* out_node)
{
    Token token = cst_current_token(state);
    if (token.kind != TK_Symbol) {
        return false;
    }

    u32 token_index   = state->token_index;
    u32 symbol_handle = cst_current_symbol_handle(state);
    u32 type_node     = CST_NO_VALUE;
    u32 value         = 0;
    if (symbol_handle == CST_NO_VALUE) {
        return false;
    }

    cst_advance(state);
    if (!cst_consume(state, TK_Colon)) {
        return false;
    }

    if (cst_current_token(state).kind == TK_Colon) {
        cst_advance(state);
    } else {
        if (!cst_parse_type(state, &type_node) ||
            !cst_consume(state, TK_Colon)) {
            return false;
        }
    }

    if (!cst_parse_value(state, &value)) {
        return false;
    }

    if (type_node != CST_NO_VALUE) {
        if (!cst_emit_node(state,
                           (CstNode){
                               .kind        = CK_AnnotatedValue,
                               .token_index = token_index,
                               .a           = type_node,
                               .b           = value,
                           },
                           &value)) {
            return false;
        }
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_Bind,
                             .token_index = token_index,
                             .a           = symbol_handle,
                             .b           = value,
                         },
                         out_node);
}

internal bool cst_parse_variable(CstParseState* state, u32* out_node)
{
    Token token = cst_current_token(state);
    if (token.kind != TK_Symbol) {
        return false;
    }

    u32 token_index   = state->token_index;
    u32 symbol_handle = cst_current_symbol_handle(state);
    u32 value         = 0;
    if (symbol_handle == CST_NO_VALUE) {
        return false;
    }

    cst_advance(state);
    if (!cst_parse_variable_payload(state, token_index, &value)) {
        return false;
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_Variable,
                             .token_index = token_index,
                             .a           = symbol_handle,
                             .b           = value,
                         },
                         out_node);
}

internal bool cst_parse_destructure(CstParseState* state, u32* out_node)
{
    u32     token_index = state->token_index;
    u32     pattern     = U32_MAX;
    u32     type_node   = CST_NO_VALUE;
    u32     value       = 0;
    CstKind kind        = CK_DestructureBind;

    if (!cst_parse_destructure_pattern(state, &pattern)) {
        return false;
    }

    if (cst_current_token(state).kind == TK_Equal) {
        kind = CK_DestructureAssign;
        cst_advance(state);
    } else if (!cst_consume(state, TK_Colon)) {
        return false;
    } else if (cst_current_token(state).kind == TK_Colon) {
        cst_advance(state);
    } else if (cst_current_token(state).kind == TK_Equal) {
        kind = CK_DestructureVariable;
        cst_advance(state);
    } else {
        kind = CK_DestructureVariable;
        if (!cst_parse_type(state, &type_node)) {
            return false;
        }
        if (cst_current_token(state).kind == TK_Colon) {
            kind = CK_DestructureBind;
            cst_advance(state);
        } else if (cst_current_token(state).kind == TK_Equal) {
            cst_advance(state);
        } else {
            return false;
        }
    }

    if (!cst_parse_expr_bp(state, 0, &value)) {
        return false;
    }

    if (type_node != CST_NO_VALUE) {
        if (!cst_emit_node(state,
                           (CstNode){
                               .kind        = CK_AnnotatedValue,
                               .token_index = token_index,
                               .a           = type_node,
                               .b           = value,
                           },
                           &value)) {
            return false;
        }
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = kind,
                             .token_index = token_index,
                             .a           = pattern,
                             .b           = value,
                         },
                         out_node);
}

internal bool cst_parse_use(CstParseState* state, u32* out_node)
{
    u32 token_index = state->token_index;
    cst_advance(state);

    u32 module_node = 0;
    if (cst_current_token(state).kind == TK_mod) {
        if (!cst_parse_mod_ref(state, &module_node)) {
            return false;
        }
    } else if (cst_current_token(state).kind == TK_Symbol &&
               cst_peek_kind_at(state, 1) == TK_Dot) {
        u32 path_token_index = state->token_index;
        u32 first_symbol     = (u32)array_count(state->cst.module_path_symbols);
        u32 symbol_count     = 0;

        for (;;) {
            u32 symbol_handle = cst_current_symbol_handle(state);
            if (symbol_handle == CST_NO_VALUE) {
                return false;
            }
            array_push(state->cst.module_path_symbols, symbol_handle);
            ++symbol_count;
            cst_advance(state);
            if (cst_current_token(state).kind != TK_Dot) {
                break;
            }
            cst_advance(state);
            if (cst_current_token(state).kind != TK_Symbol) {
                return false;
            }
        }

        u32 module_path_index = (u32)array_count(state->cst.module_paths);
        array_push(state->cst.module_paths,
                   (CstModulePath){
                       .first_symbol = first_symbol,
                       .symbol_count = symbol_count,
                   });
        if (!cst_emit_node(state,
                           (CstNode){
                               .kind        = CK_ModRef,
                               .token_index = path_token_index,
                               .a           = module_path_index,
                           },
                           &module_node)) {
            return false;
        }
    } else if (!cst_parse_expr_bp(state, 0, &module_node)) {
        return false;
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = CK_Use,
                             .token_index = token_index,
                             .a           = module_node,
                         },
                         out_node);
}

internal bool cst_parse_top_level_on(CstParseState* state, u32* out_node)
{
    u32 token_index = state->token_index;
    cst_advance(state);

    bool is_negated = false;
    if (cst_current_token(state).kind == TK_Bang) {
        is_negated = true;
        cst_advance(state);
    }

    if (cst_current_token(state).kind != TK_Symbol) {
        return false;
    }
    u32 symbol_handle = cst_current_symbol_handle(state);
    if (symbol_handle == CST_NO_VALUE) {
        return false;
    }
    cst_advance(state);

    if (cst_current_token(state).kind != TK_LBrace) {
        return false;
    }

    u32 top_on_node = 0;
    if (!cst_emit_node(state,
                       (CstNode){
                           .kind        = CK_TopOn,
                           .token_index = token_index,
                       },
                       &top_on_node)) {
        return false;
    }

    u32 block_node = 0;
    if (!cst_emit_node(state,
                       (CstNode){
                           .kind        = CK_Block,
                           .token_index = state->token_index,
                       },
                       &block_node)) {
        return false;
    }
    u32 first_item = (u32)array_count(state->cst.nodes);

    cst_advance(state);
    while (cst_current_token(state).kind != TK_RBrace) {
        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        bool ok                         = cst_parse_top_level_item(state, NULL);
        state->allow_statement_boundary = previous_boundary;
        if (!ok) {
            return false;
        }
    }
    cst_advance(state);

    u32 top_on_info_index = (u32)array_count(state->cst.top_ons);
    array_push(state->cst.top_ons,
               (CstTopOnInfo){
                   .symbol_handle   = symbol_handle,
                   .body_node_index = block_node,
                   .is_negated      = is_negated,
               });
    state->cst.nodes[top_on_node].a = top_on_info_index;
    state->cst.nodes[block_node].a  = first_item;
    state->cst.nodes[block_node].b  = (u32)array_count(state->cst.nodes);
    if (out_node) {
        *out_node = top_on_node;
    }
    return true;
}

internal bool cst_parse_top_level_item(CstParseState* state, u32* out_node)
{
    bool is_public = false;
    if (cst_current_token(state).kind == TK_pub) {
        is_public = true;
        cst_advance(state);
    }

    if (cst_current_token(state).kind == TK_ffi) {
        if (is_public) {
            return false;
        }
        return cst_parse_ffi_def(state, out_node);
    }

    if (cst_current_token(state).kind == TK_use) {
        if (is_public) {
            return false;
        }
        return cst_parse_use(state, out_node);
    }

    if (cst_current_token(state).kind == TK_on) {
        if (is_public) {
            return false;
        }
        return cst_parse_top_level_on(state, out_node);
    }

    if (!cst_starts_binding(state)) {
        return false;
    }

    if (cst_starts_variable(state) && !cst_starts_annotated_bind(state)) {
        if (is_public) {
            return false;
        }
        return cst_parse_variable(state, out_node);
    }

    u32 bind_node = 0;
    if (!cst_parse_bind(state, &bind_node)) {
        return false;
    }
    if (is_public) {
        state->cst.nodes[bind_node].flags |= CNF_Public;
    }
    if (out_node != NULL) {
        *out_node = bind_node;
    }
    return true;
}

//------------------------------------------------------------------------------
// Build parallel token-value tables so the parser can use direct token-index
// access without re-running lexer-side counters during recursive parsing.

internal void cst_build_token_value_tables(CstParseState* state)
{
    u32 integer_index = 0;
    u32 float_index   = 0;
    u32 string_index  = 0;
    u32 symbol_index  = 0;

    for (u32 i = 0; i < array_count(state->lexer->tokens); ++i) {
        Token token = state->lexer->tokens[i];

        switch (token.kind) {
        case TK_Integer:
            array_push(state->token_integer_indices, integer_index++);
            array_push(state->token_float_indices, CST_NO_VALUE);
            array_push(state->token_string_indices, CST_NO_VALUE);
            array_push(state->token_symbol_handles, CST_NO_VALUE);
            break;
        case TK_Float:
            array_push(state->token_integer_indices, CST_NO_VALUE);
            array_push(state->token_float_indices, float_index++);
            array_push(state->token_string_indices, CST_NO_VALUE);
            array_push(state->token_symbol_handles, CST_NO_VALUE);
            break;

        case TK_String:
            array_push(state->token_integer_indices, CST_NO_VALUE);
            array_push(state->token_float_indices, CST_NO_VALUE);
            array_push(state->token_string_indices, string_index++);
            array_push(state->token_symbol_handles, CST_NO_VALUE);
            break;

        case TK_Symbol:
            array_push(state->token_integer_indices, CST_NO_VALUE);
            array_push(state->token_float_indices, CST_NO_VALUE);
            array_push(state->token_string_indices, CST_NO_VALUE);
            array_push(state->token_symbol_handles,
                       state->lexer->symbol_handles[symbol_index++]);
            break;

        default:
            array_push(state->token_integer_indices, CST_NO_VALUE);
            array_push(state->token_float_indices, CST_NO_VALUE);
            array_push(state->token_string_indices, CST_NO_VALUE);
            array_push(state->token_symbol_handles, CST_NO_VALUE);
            break;
        }
    }
}

//------------------------------------------------------------------------------
// Parse a compact CST for the current Nerd surface grammar.

bool cst_parse(const Lexer* lexer, Cst* out_cst)
{
    CstParseState state = {.lexer = lexer};
    cst_build_token_value_tables(&state);

    while (state.token_index < array_count(lexer->tokens)) {
        u32 item_index = 0;
        if (!cst_parse_top_level_item(&state, &item_index)) {
            cst_done(&state.cst);
            array_free(state.token_integer_indices);
            array_free(state.token_float_indices);
            array_free(state.token_string_indices);
            array_free(state.token_symbol_handles);
            return false;
        }
        array_push(state.cst.bindings, item_index);
    }

    for (u32 i = 0; i < array_count(lexer->integers); ++i) {
        array_push(state.cst.integers, lexer->integers[i]);
    }
    for (u32 i = 0; i < array_count(lexer->floats); ++i) {
        array_push(state.cst.floats, lexer->floats[i]);
    }

    array_free(state.token_integer_indices);
    array_free(state.token_float_indices);
    array_free(state.token_string_indices);
    array_free(state.token_symbol_handles);
    *out_cst = state.cst;
    return true;
}

//------------------------------------------------------------------------------
// Release CST storage.

void cst_done(Cst* cst)
{
    array_free(cst->nodes);
    array_free(cst->integers);
    array_free(cst->floats);
    array_free(cst->bindings);
    array_free(cst->params);
    array_free(cst->fn_signatures);
    array_free(cst->ffi_infos);
    array_free(cst->module_paths);
    array_free(cst->module_path_symbols);
    array_free(cst->call_args);
    array_free(cst->tuple_items);
    array_free(cst->calls);
    array_free(cst->slices);
    array_free(cst->plex_fields);
    array_free(cst->plex_types);
    array_free(cst->enum_variants);
    array_free(cst->enum_types);
    array_free(cst->plex_literal_fields);
    array_free(cst->plex_literals);
    array_free(cst->patterns);
    array_free(cst->pattern_items);
    array_free(cst->pattern_fields);
    array_free(cst->enum_patterns);
    array_free(cst->on_branches);
    array_free(cst->ons);
    array_free(cst->top_ons);
    array_free(cst->for_items);
    array_free(cst->fors);
    *cst = (Cst){0};
}

//------------------------------------------------------------------------------
// Return the literal integer payload for one CST node.

u64 cst_get_integer(const Cst* cst, const CstNode* node)
{
    ASSERT(node->kind == CK_IntegerLiteral,
           "Expected integer literal CST node");
    ASSERT(node->a < array_count(cst->integers), "Integer index out of bounds");
    return cst->integers[node->a];
}

f64 cst_get_float(const Cst* cst, const CstNode* node)
{
    ASSERT(node->kind == CK_FloatLiteral, "Expected float literal CST node");
    ASSERT(node->a < array_count(cst->floats), "Float index out of bounds");
    return cst->floats[node->a];
}

//------------------------------------------------------------------------------
// Return the symbol handle payload for one CST node.

u32 cst_get_symbol(const CstNode* node)
{
    ASSERT(node->kind == CK_SymbolRef || node->kind == CK_Bind ||
               node->kind == CK_Variable || node->kind == CK_Assign,
           "Expected symbol-bearing CST node");
    return node->a;
}

bool cst_node_is_block_statement(const CstNode* node)
{
    return node->kind == CK_Block || node->kind == CK_Statement ||
           node->kind == CK_Return || node->kind == CK_Bind ||
           node->kind == CK_For || node->kind == CK_Break ||
           node->kind == CK_Continue || node->kind == CK_Variable ||
           node->kind == CK_DestructureBind ||
           node->kind == CK_DestructureVariable ||
           node->kind == CK_DestructureAssign || node->kind == CK_Assign ||
           node->kind == CK_Use || node->kind == CK_FfiDef ||
           node->kind == CK_TopOn;
}

u32 cst_block_statement_end_exclusive(const Cst* cst, u32 node_index)
{
    const CstNode* node = &cst->nodes[node_index];
    if (node->kind == CK_Block) {
        return node->b;
    }
    if (node->kind == CK_For) {
        const CstForInfo* for_info = &cst->fors[node->a];
        return for_info->else_block_index == U32_MAX
                   ? cst->nodes[node->b].b
                   : cst->nodes[for_info->else_block_index].b;
    }
    if (node->kind == CK_TopOn) {
        return cst->nodes[cst->top_ons[node->a].body_node_index].b;
    }
    if (node->kind == CK_Bind || node->kind == CK_Variable ||
        node->kind == CK_DestructureBind ||
        node->kind == CK_DestructureVariable ||
        node->kind == CK_DestructureAssign || node->kind == CK_Statement ||
        node->kind == CK_Use) {
        u32 child_index = node->kind == CK_Statement ? node->a : node->b;
        if (node->kind == CK_Use) {
            child_index = node->a;
        }
        if (child_index >= array_count(cst->nodes)) {
            return node_index + 1;
        }
        const CstNode* child = &cst->nodes[child_index];
        if (child->kind == CK_For || child->kind == CK_Block) {
            u32 end = cst_block_statement_end_exclusive(cst, child_index);
            return end > node_index + 1 ? end : node_index + 1;
        }
    }
    return node_index + 1;
}

//------------------------------------------------------------------------------
