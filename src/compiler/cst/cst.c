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
    Cst cst;
} CstParseState;

internal bool cst_parse_bind(CstParseState* state, u32* out_node);
internal bool cst_parse_on_branch_pattern(CstParseState* state, u32* out_node);

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

internal TokenKind cst_kind_at_stream_index(const CstParseState* state,
                                            u32                  index)
{
    if (index >= array_count(state->lexer->tokens)) {
        return TK_EOF;
    }
    return state->lexer->tokens[index].kind;
}

internal bool cst_skip_type_tokens(const CstParseState* state, u32* io_index)
{
    TokenKind kind = cst_kind_at_stream_index(state, *io_index);
    if (kind == TK_Symbol) {
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
    return cst_current_token(state).kind == TK_Symbol &&
           cst_peek_kind_at(state, 1) == TK_Equal;
}

//------------------------------------------------------------------------------
// Return the infix binding powers for the given token kind.

internal bool
cst_infix_binding_power(TokenKind kind, u8* out_left_bp, u8* out_right_bp)
{
    switch (kind) {
    case TK_LParen:
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

//------------------------------------------------------------------------------
// Parse one expression using Pratt binding powers.

internal bool cst_parse_expr_bp(CstParseState* state, u8 min_bp, u32* out_node);
internal bool cst_parse_variable_payload(CstParseState* state,
                                         u32            token_index,
                                         u32*           out_node);
internal bool cst_parse_fn_expr(CstParseState* state, u32* out_node);
internal bool cst_parse_on_expr(CstParseState* state, u32* out_node);
internal bool cst_parse_type(CstParseState* state, u32* out_node);
internal bool cst_parse_fn_signature(CstParseState* state,
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
    if (!cst_consume(state, TK_fn) || !cst_consume(state, TK_LParen)) {
        return false;
    }

    u32 first_param = (u32)array_count(state->cst.params);
    u32 param_count = 0;

    if (cst_current_token(state).kind != TK_RParen) {
        for (;;) {
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

    case TK_true:
    case TK_false:
        cst_advance(state);
        return cst_emit_node(state,
                             (CstNode){
                                 .kind        = CK_BoolLiteral,
                                 .token_index = state->token_index - 1,
                                 .a           = token.kind == TK_true ? 1u : 0u,
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
        {
            u32 token_index = state->token_index;
            u32 operand     = 0;
            cst_advance(state);
            if (!cst_parse_expr_bp(state, CST_BP_PREFIX, &operand)) {
                return false;
            }

            return cst_emit_node(state,
                                 (CstNode){
                                     .kind = token.kind == TK_Bang
                                                 ? CK_LogicalNot
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
            if (!cst_parse_expr_bp(state, 0, &inner) ||
                !cst_consume(state, TK_RParen)) {
                return false;
            }

            return cst_emit_node(state,
                                 (CstNode){
                                     .kind        = CK_Group,
                                     .token_index = token_index,
                                     .a           = inner,
                                 },
                                 out_node);
        }

    case TK_fn:
        return cst_parse_fn_expr(state, out_node);
    case TK_on:
        return cst_parse_on_expr(state, out_node);

    default:
        return false;
    }
}

internal bool cst_parse_on_expr(CstParseState* state, u32* out_node)
{
    u32 token_index = state->token_index;
    cst_advance(state);

    u32 scrutinee = 0;
    if (!cst_parse_expr_bp(state, 0, &scrutinee)) {
        return false;
    }

    u32 first_branch = (u32)array_count(state->cst.on_branches);

    if (cst_current_token(state).kind == TK_LBrace) {
        cst_advance(state);
        bool saw_else = false;

        while (cst_current_token(state).kind != TK_RBrace) {
            if (cst_current_token(state).kind == TK_EOF) {
                return false;
            }

            CstOnBranch branch = {0};
            if (cst_current_token(state).kind == TK_else) {
                branch.flags = COBF_Else;
                saw_else     = true;
                cst_advance(state);
                if (!cst_consume(state, TK_FatArrow)) {
                    return false;
                }
            } else {
                branch.pattern_node_index =
                    (u32)array_count(state->cst.on_pattern_nodes);
                branch.pattern_count = 0;
                for (;;) {
                    u32 pattern_root = 0;
                    if (!cst_parse_on_branch_pattern(state, &pattern_root)) {
                        return false;
                    }
                    array_push(state->cst.on_pattern_nodes, pattern_root);
                    ++branch.pattern_count;
                    if (cst_current_token(state).kind != TK_Comma) {
                        break;
                    }
                    cst_advance(state);
                }
                if (!cst_consume(state, TK_FatArrow)) {
                    return false;
                }
            }

            if (!cst_parse_expr_bp(state, 0, &branch.expr_node_index)) {
                return false;
            }
            array_push(state->cst.on_branches, branch);

            if (branch.flags & COBF_Else) {
                break;
            }
        }

        if (!saw_else || !cst_consume(state, TK_RBrace)) {
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
    if (!cst_parse_expr_bp(state, 0, &true_expr) ||
        !cst_consume(state, TK_else)) {
        return false;
    }

    u32 false_expr = 0;
    if (!cst_parse_expr_bp(state, 0, &false_expr)) {
        return false;
    }

    u32 true_pattern = 0;
    if (!cst_emit_node(state,
                       (CstNode){
                           .kind        = CK_BoolLiteral,
                           .token_index = token_index,
                           .a           = 1,
                       },
                       &true_pattern)) {
        return false;
    }
    array_push(state->cst.on_branches,
               (CstOnBranch){
                   .pattern_node_index = true_pattern,
                   .expr_node_index    = true_expr,
               });
    array_push(state->cst.on_branches,
               (CstOnBranch){
                   .expr_node_index = false_expr,
                   .flags           = COBF_Else,
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

internal bool cst_parse_on_branch_pattern(CstParseState* state, u32* out_node)
{
    u32 start_node = 0;
    if (!cst_parse_expr_bp(state, 0, &start_node)) {
        return false;
    }

    CstKind range_kind = CK_IntegerPlus;
    if (cst_current_token(state).kind == TK_RangeExclusive) {
        range_kind = CK_RangeExclusive;
    } else if (cst_current_token(state).kind == TK_RangeInclusive) {
        range_kind = CK_RangeInclusive;
    } else {
        *out_node = start_node;
        return true;
    }

    u32 token_index = state->token_index;
    cst_advance(state);
    u32 end_node = 0;
    if (!cst_parse_expr_bp(state, 0, &end_node)) {
        return false;
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind        = range_kind,
                             .token_index = token_index,
                             .a           = start_node,
                             .b           = end_node,
                         },
                         out_node);
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
        Token token = cst_current_token(state);
        u8    left_bp;
        u8    right_bp;

        if (!cst_infix_binding_power(token.kind, &left_bp, &right_bp) ||
            left_bp < min_bp) {
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
            break;
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
        if (token.kind == TK_Dot) {
            if (cst_current_token(state).kind != TK_Symbol ||
                !string_eq(
                    lex_symbol(state->lexer, cst_current_symbol_handle(state)),
                    s("cast"))) {
                return false;
            }
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

internal bool cst_parse_block_statement(CstParseState* state)
{
    u32 token_index = state->token_index;

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

    if (cst_current_token(state).kind == TK_LBrace) {
        return cst_parse_nested_block(state, NULL);
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
        if (!cst_consume(state, TK_Equal) ||
            !cst_parse_expr_bp(state, 0, &value)) {
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

    return cst_parse_expr_bp(state, 0, out_node);
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
        if (!cst_starts_binding(&state)) {
            cst_done(&state.cst);
            array_free(state.token_integer_indices);
            array_free(state.token_float_indices);
            array_free(state.token_string_indices);
            array_free(state.token_symbol_handles);
            return false;
        }

        u32 bind_index = 0;
        if (cst_starts_variable(&state) && !cst_starts_annotated_bind(&state)) {
            if (!cst_parse_variable(&state, &bind_index)) {
                cst_done(&state.cst);
                array_free(state.token_integer_indices);
                array_free(state.token_float_indices);
                array_free(state.token_string_indices);
                array_free(state.token_symbol_handles);
                return false;
            }
        } else if (!cst_parse_bind(&state, &bind_index)) {
            cst_done(&state.cst);
            array_free(state.token_integer_indices);
            array_free(state.token_float_indices);
            array_free(state.token_string_indices);
            array_free(state.token_symbol_handles);
            return false;
        }

        array_push(state.cst.bindings, bind_index);
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
    array_free(cst->call_args);
    array_free(cst->calls);
    array_free(cst->on_pattern_nodes);
    array_free(cst->on_branches);
    array_free(cst->ons);
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

//------------------------------------------------------------------------------
