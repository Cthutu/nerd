//------------------------------------------------------------------------------
// Concrete syntax tree implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cst/cst.h>

//------------------------------------------------------------------------------

#define CST_NO_VALUE UINT32_MAX
#define CST_BP_ADDITIVE 10
#define CST_BP_MULTIPLICATIVE 20
#define CST_BP_PREFIX 30

typedef struct {
    const Lexer* lexer;
    u32          token_index;
    Array(u32)   token_integer_indices;
    Array(u32)   token_symbol_handles;
    Cst          cst;
} CstParseState;

//------------------------------------------------------------------------------
// Return the current token, or a synthetic EOF token when the cursor is past
// the end of the lexer token stream.

internal Token cst_current_token(const CstParseState* state)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        return (Token){
            .kind = TK_EOF,
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
    *out_index = node_index;
    return true;
}

//------------------------------------------------------------------------------
// Return whether the current token starts a top-level binding.

internal bool cst_starts_binding(const CstParseState* state)
{
    if (state->token_index + 2 >= array_count(state->lexer->tokens)) {
        return false;
    }

    return state->lexer->tokens[state->token_index].kind == TK_Symbol &&
           state->lexer->tokens[state->token_index + 1].kind == TK_Colon &&
           state->lexer->tokens[state->token_index + 2].kind == TK_Colon;
}

//------------------------------------------------------------------------------
// Return the infix binding powers for the given token kind.

internal bool cst_infix_binding_power(TokenKind kind,
                                      u8*       out_left_bp,
                                      u8*       out_right_bp)
{
    switch (kind) {
    case TK_Plus:
    case TK_Minus:
        *out_left_bp  = CST_BP_ADDITIVE;
        *out_right_bp = CST_BP_ADDITIVE + 1;
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

internal bool cst_parse_expr_bp(CstParseState* state,
                                u8             min_bp,
                                u32*           out_node);

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
                                     .kind = CK_IntegerLiteral,
                                     .token_index = state->token_index - 1,
                                     .a = integer_index,
                                 },
                                 out_node);
        }

    case TK_Symbol:
        {
            u32 symbol_handle = cst_current_symbol_handle(state);
            if (symbol_handle == CST_NO_VALUE) {
                return false;
            }

            cst_advance(state);
            return cst_emit_node(state,
                                 (CstNode){
                                     .kind = CK_SymbolRef,
                                     .token_index = state->token_index - 1,
                                     .a = symbol_handle,
                                 },
                                 out_node);
        }

    case TK_Minus:
        {
            u32 token_index = state->token_index;
            u32 operand     = 0;
            cst_advance(state);
            if (!cst_parse_expr_bp(state, CST_BP_PREFIX, &operand)) {
                return false;
            }

            return cst_emit_node(state,
                                 (CstNode){
                                     .kind = CK_IntegerNegate,
                                     .token_index = token_index,
                                     .a = operand,
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
                                     .kind = CK_Group,
                                     .token_index = token_index,
                                     .a = inner,
                                 },
                                 out_node);
        }

    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Parse one expression using Pratt binding powers.

internal bool cst_parse_expr_bp(CstParseState* state,
                                u8             min_bp,
                                u32*           out_node)
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
            break;
        }

        u32 token_index = state->token_index;
        u32 right       = 0;
        cst_advance(state);
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
        default:
            return false;
        }

        if (!cst_emit_node(state,
                           (CstNode){
                               .kind = kind,
                               .token_index = token_index,
                               .a = left,
                               .b = right,
                           },
                           &left)) {
            return false;
        }
    }

    *out_node = left;
    return true;
}

//------------------------------------------------------------------------------
// Parse one function expression.

internal bool cst_parse_fn_expr(CstParseState* state, u32* out_node)
{
    u32 token_index = state->token_index;
    u32 body        = 0;

    if (!cst_consume(state, TK_fn) || !cst_consume(state, TK_LParen) ||
        !cst_consume(state, TK_RParen) || !cst_consume(state, TK_FatArrow) ||
        !cst_parse_expr_bp(state, 0, &body)) {
        return false;
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind = CK_FnExpr,
                             .token_index = token_index,
                             .a = body,
                         },
                         out_node);
}

//------------------------------------------------------------------------------
// Parse one binding value, which may currently be either a function expression
// or a plain expression.

internal bool cst_parse_value(CstParseState* state, u32* out_node)
{
    if (cst_current_token(state).kind == TK_fn) {
        return cst_parse_fn_expr(state, out_node);
    }

    return cst_parse_expr_bp(state, 0, out_node);
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
    u32 value         = 0;
    if (symbol_handle == CST_NO_VALUE) {
        return false;
    }

    cst_advance(state);
    if (!cst_consume(state, TK_Colon) || !cst_consume(state, TK_Colon) ||
        !cst_parse_value(state, &value)) {
        return false;
    }

    return cst_emit_node(state,
                         (CstNode){
                             .kind = CK_Bind,
                             .token_index = token_index,
                             .a = symbol_handle,
                             .b = value,
                         },
                         out_node);
}

//------------------------------------------------------------------------------
// Build parallel token-value tables so the parser can use direct token-index
// access without re-running lexer-side counters during recursive parsing.

internal void cst_build_token_value_tables(CstParseState* state)
{
    u32 integer_index = 0;
    u32 symbol_index  = 0;

    for (u32 i = 0; i < array_count(state->lexer->tokens); ++i) {
        Token token = state->lexer->tokens[i];

        switch (token.kind) {
        case TK_Integer:
            array_push(state->token_integer_indices, integer_index++);
            array_push(state->token_symbol_handles, CST_NO_VALUE);
            break;

        case TK_Symbol:
            array_push(state->token_integer_indices, CST_NO_VALUE);
            array_push(state->token_symbol_handles,
                       state->lexer->symbol_handles[symbol_index++]);
            break;

        default:
            array_push(state->token_integer_indices, CST_NO_VALUE);
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
            array_free(state.token_symbol_handles);
            return false;
        }

        u32 bind_index = 0;
        if (!cst_parse_bind(&state, &bind_index)) {
            cst_done(&state.cst);
            array_free(state.token_integer_indices);
            array_free(state.token_symbol_handles);
            return false;
        }

        array_push(state.cst.bindings, bind_index);
    }

    for (u32 i = 0; i < array_count(lexer->integers); ++i) {
        array_push(state.cst.integers, lexer->integers[i]);
    }

    array_free(state.token_integer_indices);
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
    array_free(cst->bindings);
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

//------------------------------------------------------------------------------
// Return the symbol handle payload for one CST node.

u32 cst_get_symbol(const CstNode* node)
{
    ASSERT(node->kind == CK_SymbolRef || node->kind == CK_Bind,
           "Expected symbol-bearing CST node");
    return node->a;
}

//------------------------------------------------------------------------------
