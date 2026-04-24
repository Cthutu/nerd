//------------------------------------------------------------------------------
// Parsing implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ast/parse_internal.h>

TokenKind ast_peek_kind_at(const AstParseState* state, u32 lookahead)
{
    u32 index = state->token.token_index + 1 + lookahead;
    if (index >= array_count(state->lexer->tokens)) {
        return TK_EOF;
    }
    return state->lexer->tokens[index].kind;
}

internal bool ast_symbol_starts_assignment(const AstParseState* state)
{
    if (state->token.kind != TK_Symbol) {
        return false;
    }

    switch (ast_peek_kind_at(state, 0)) {
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

internal TokenKind ast_cursor_kind(const AstParseState* state)
{
    if (state->token_index >= array_count(state->lexer->tokens)) {
        return TK_EOF;
    }
    return state->lexer->tokens[state->token_index].kind;
}

internal bool ast_compound_assignment_binary_kind(TokenKind op, AstKind* out)
{
    switch (op) {
    case TK_PlusEqual:
        *out = AK_IntegerPlus;
        return true;
    case TK_MinusEqual:
        *out = AK_IntegerMinus;
        return true;
    case TK_StarEqual:
        *out = AK_IntegerMultiply;
        return true;
    case TK_SlashEqual:
        *out = AK_IntegerDivide;
        return true;
    case TK_PercentEqual:
        *out = AK_IntegerModulo;
        return true;
    case TK_AmpEqual:
        *out = AK_BitwiseAnd;
        return true;
    case TK_CaretEqual:
        *out = AK_BitwiseXor;
        return true;
    case TK_PipeEqual:
        *out = AK_BitwiseOr;
        return true;
    case TK_AmpAmpEqual:
        *out = AK_LogicalAnd;
        return true;
    case TK_PipePipeEqual:
        *out = AK_LogicalOr;
        return true;
    default:
        return false;
    }
}

internal bool ast_symbol_starts_variable(const AstParseState* state)
{
    return state->token.kind == TK_Symbol &&
           ast_peek_kind_at(state, 0) == TK_Colon &&
           ast_peek_kind_at(state, 1) != TK_Colon;
}

internal TokenKind ast_kind_at_stream_index(const AstParseState* state,
                                            u32                  index)
{
    if (index >= array_count(state->lexer->tokens)) {
        return TK_EOF;
    }
    return state->lexer->tokens[index].kind;
}

internal bool ast_skip_type_tokens(const AstParseState* state, u32* io_index)
{
    TokenKind kind = ast_kind_at_stream_index(state, *io_index);
    if (kind == TK_Symbol) {
        (*io_index)++;
        return true;
    }
    if (kind != TK_fn) {
        return false;
    }

    (*io_index)++;
    if (ast_kind_at_stream_index(state, *io_index) != TK_LParen) {
        return false;
    }
    (*io_index)++;
    if (ast_kind_at_stream_index(state, *io_index) != TK_RParen) {
        for (;;) {
            if (!ast_skip_type_tokens(state, io_index)) {
                return false;
            }
            if (ast_kind_at_stream_index(state, *io_index) == TK_Comma) {
                (*io_index)++;
                continue;
            }
            break;
        }
        if (ast_kind_at_stream_index(state, *io_index) != TK_RParen) {
            return false;
        }
    }
    (*io_index)++;
    if (ast_kind_at_stream_index(state, *io_index) != TK_ThinArrow) {
        return false;
    }
    (*io_index)++;
    return ast_skip_type_tokens(state, io_index);
}

bool ast_token_starts_type_syntax(const AstParseState* state, u32 token_index)
{
    return ast_skip_type_tokens(state, &token_index);
}

internal bool
ast_remaining_bind_value_is_type_syntax(const AstParseState* state)
{
    u32 token_index = state->token.token_index;
    if (!ast_skip_type_tokens(state, &token_index)) {
        return false;
    }

    TokenKind next_kind = ast_kind_at_stream_index(state, token_index);
    return next_kind == TK_EOF ||
           (next_kind == TK_Symbol &&
            ast_kind_at_stream_index(state, token_index + 1) == TK_Colon);
}

internal bool ast_symbol_starts_bind(const AstParseState* state)
{
    if (state->token.kind != TK_Symbol ||
        ast_peek_kind_at(state, 0) != TK_Colon) {
        return false;
    }
    if (ast_peek_kind_at(state, 1) == TK_Colon) {
        return true;
    }

    u32 type_index = state->token.token_index + 2;
    if (!ast_skip_type_tokens(state, &type_index)) {
        return false;
    }
    return ast_kind_at_stream_index(state, type_index) == TK_Colon;
}

//------------------------------------------------------------------------------
// Parse one function signature, either in a type position or a function value.

bool ast_parse_fn_signature(AstParseState* state,
                            bool           allow_named_params,
                            bool           require_return_type,
                            u32*           out_signature_index)
{
    ASSERT(state->token.kind == TK_fn, "Expected `fn` token for signature");

    u32 first_param = (u32)array_count(state->params);
    u32 param_count = 0;

    if (!ast_expect_token(state, TK_LParen)) {
        return false;
    }
    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_RParen,
                                         TK_EOF);
    }

    if (state->token.kind != TK_RParen) {
        for (;;) {
            if (allow_named_params) {
                if (state->token.kind != TK_Symbol) {
                    return error_0203_expected_token(
                        state->lexer->source,
                        ast_token_span(state, &state->token),
                        TK_Symbol,
                        state->token.kind);
                }

                AstToken param_token = state->token;
                if (!ast_expect_token(state, TK_Colon) ||
                    !ast_next_token(state)) {
                    return false;
                }

                u32 type_node = 0;
                if (!ast_parse_type(state, &type_node)) {
                    return false;
                }

                array_push(state->params,
                           (AstParam){
                               .token_index   = param_token.token_index,
                               .symbol_handle = param_token.value.symbol_handle,
                               .type_node_index = type_node,
                           });
            } else {
                AstToken type_token = state->token;
                u32      type_node  = 0;
                if (!ast_parse_type(state, &type_node)) {
                    return false;
                }

                array_push(state->params,
                           (AstParam){
                               .token_index     = type_token.token_index,
                               .symbol_handle   = U32_MAX,
                               .type_node_index = type_node,
                           });
            }

            ++param_count;
            if (ast_peek_kind_at(state, 0) == TK_Comma) {
                if (!ast_expect_token(state, TK_Comma) ||
                    !ast_next_token(state)) {
                    return error_0201_missing_value(
                        state->token.source,
                        ast_token_span(state, &state->token),
                        TK_RParen);
                }
                continue;
            }
            break;
        }
        if (!ast_expect_token(state, TK_RParen)) {
            return false;
        }
    }

    u32 return_type = U32_MAX;
    if (ast_peek_kind_at(state, 0) == TK_ThinArrow) {
        if (!ast_expect_token(state, TK_ThinArrow) || !ast_next_token(state)) {
            return false;
        }
        if (!ast_parse_type(state, &return_type)) {
            return false;
        }
    } else if (require_return_type) {
        return error_0203_expected_token(state->token.source,
                                         ast_token_span(state, &state->token),
                                         TK_ThinArrow,
                                         ast_peek_kind_at(state, 0));
    }

    u32 signature_index = (u32)array_count(state->fn_signatures);
    array_push(state->fn_signatures,
               (AstFnSignature){
                   .first_param            = first_param,
                   .param_count            = param_count,
                   .return_type_node_index = return_type,
               });
    *out_signature_index = signature_index;
    return true;
}

bool ast_parse_type_signature(AstParseState* state, u32* out_signature_index)
{
    return ast_parse_fn_signature(state, false, true, out_signature_index);
}

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

    AstToken fn_token        = state->token;
    u32      signature_index = 0;
    if (!ast_parse_type_signature(state, &signature_index)) {
        return false;
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_TypeFn,
                             .token_index = fn_token.token_index,
                             .a           = signature_index,
                         },
                         out_node);
}

//------------------------------------------------------------------------------
// Classify which top-level parser should handle a token.

ParsingQuery ast_parsing_query_for_token(TokenKind kind)
{
    if (kind == TK_fn) {
        return PQ_Declaration;
    }

    if (ast_token_starts_expression(kind)) {
        return PQ_Expression;
    }

    return PQ_Invalid;
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

internal bool ast_parse_block_statement(AstParseState* state);

//------------------------------------------------------------------------------
// Parse one standalone block statement.

bool ast_parse_nested_block(AstParseState* state, u32* out_node)
{
    ASSERT(state->token.kind == TK_LBrace, "Expected '{' token for block");

    u32 block_token_index = state->token.token_index;
    u32 block_index       = 0;
    if (!ast_emit_node(state,
                       (AstNode){
                           .kind        = AK_Block,
                           .token_index = block_token_index,
                       },
                       &block_index)) {
        return false;
    }
    u32 first_statement = (u32)array_count(state->nodes);

    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_RBrace,
                                         TK_EOF);
    }

    while (state->token.kind != TK_RBrace) {
        if (!ast_parse_block_statement(state)) {
            return false;
        }

        if (!ast_next_token(state)) {
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

    state->nodes[block_index].a = first_statement;
    state->nodes[block_index].b = (u32)array_count(state->nodes);
    if (out_node) {
        *out_node = block_index;
    }
    return true;
}

//------------------------------------------------------------------------------
// Parse one comma-separated item in a C-style for clause.

internal bool ast_parse_for_clause_item(AstParseState* state,
                                        bool           allow_declaration,
                                        u32*           out_node,
                                        bool*          out_raw_expr)
{
    if (allow_declaration && ast_symbol_starts_bind(state)) {
        if (ast_symbol_starts_variable(state)) {
            *out_raw_expr = false;
            return ast_parse_variable(state, out_node);
        }

        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        bool ok                         = ast_parse_bind(state, out_node);
        state->allow_statement_boundary = previous_boundary;
        *out_raw_expr                   = false;
        return ok;
    }

    if (allow_declaration && ast_symbol_starts_variable(state)) {
        *out_raw_expr = false;
        return ast_parse_variable(state, out_node);
    }

    if (ast_symbol_starts_assignment(state)) {
        *out_raw_expr = false;
        return ast_parse_assignment(state, out_node);
    }

    u32  expr_node                  = 0;
    bool previous_boundary          = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    bool ok                         = ast_parse_expr(state, &expr_node);
    state->allow_statement_boundary = previous_boundary;
    if (!ok) {
        return false;
    }

    *out_node     = expr_node;
    *out_raw_expr = true;
    return true;
}

internal bool
ast_wrap_for_expr_item(AstParseState* state, u32 token_index, u32* in_out_node)
{
    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_Statement,
                             .token_index = token_index,
                             .a           = *in_out_node,
                         },
                         in_out_node);
}

internal bool ast_parse_for_item_list(AstParseState* state,
                                      bool           allow_declaration,
                                      TokenKind      terminator,
                                      u32            first_node,
                                      bool           first_raw_expr,
                                      u32            first_token_index,
                                      u32*           out_first_item,
                                      u32*           out_item_count)
{
    u32 first_item = (u32)array_count(state->for_items);
    u32 item_count = 0;
    u32 item       = first_node;
    if (first_raw_expr &&
        !ast_wrap_for_expr_item(state, first_token_index, &item)) {
        return false;
    }
    array_push(state->for_items, item);
    item_count++;

    while (state->token.kind == TK_Comma) {
        if (!ast_next_token(state) || !ast_next_token(state)) {
            return error_0201_missing_value(
                state->token.source,
                ast_token_span(state, &state->token),
                state->token.kind);
        }

        u32  next_item        = 0;
        bool next_is_raw_expr = false;
        u32  next_token_index = state->token.token_index;
        if (!ast_parse_for_clause_item(
                state, allow_declaration, &next_item, &next_is_raw_expr)) {
            return false;
        }
        if (next_is_raw_expr &&
            !ast_wrap_for_expr_item(state, next_token_index, &next_item)) {
            return false;
        }
        array_push(state->for_items, next_item);
        item_count++;
    }

    if (state->token.kind != terminator) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         terminator,
                                         state->token.kind);
    }

    *out_first_item = first_item;
    *out_item_count = item_count;
    return true;
}

//------------------------------------------------------------------------------
// Parse a `for` loop statement or expression. The current token must be `for`.

bool ast_parse_for(AstParseState* state, u32 label_symbol, u32* out_node)
{
    u32        for_token_index = state->token.token_index;
    u32        for_node        = 0;
    AstForInfo for_info        = {
               .first_init           = U32_MAX,
               .init_count           = 0,
               .condition_node_index = U32_MAX,
               .first_update         = U32_MAX,
               .update_count         = 0,
               .label_symbol         = label_symbol,
    };
    u32 body_node = 0;
    if (!ast_emit_node(state,
                       (AstNode){
                           .kind        = AK_For,
                           .token_index = for_token_index,
                       },
                       &for_node)) {
        return false;
    }
    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_LBrace,
                                         TK_EOF);
    }
    if (state->token.kind != TK_LBrace) {
        if (state->token.kind == TK_Semicolon) {
            if (ast_cursor_kind(state) != TK_Semicolon) {
                if (!ast_next_token(state)) {
                    return false;
                }
                bool previous_boundary = state->allow_statement_boundary;
                state->allow_statement_boundary = true;
                if (!ast_parse_expr(state, &for_info.condition_node_index)) {
                    state->allow_statement_boundary = previous_boundary;
                    return false;
                }
                state->allow_statement_boundary = previous_boundary;
            } else if (!ast_next_token(state)) {
                return false;
            }

            if (state->token.kind != TK_Semicolon) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_Semicolon,
                    state->token.kind);
            }
            if (!ast_expect_token(state, TK_Semicolon)) {
                return false;
            }

            if (ast_cursor_kind(state) != TK_LBrace) {
                if (!ast_next_token(state)) {
                    return false;
                }
                u32  update_node        = 0;
                bool update_raw_expr    = false;
                u32  update_token_index = state->token.token_index;
                if (!ast_parse_for_clause_item(
                        state, false, &update_node, &update_raw_expr) ||
                    !ast_parse_for_item_list(state,
                                             false,
                                             TK_LBrace,
                                             update_node,
                                             update_raw_expr,
                                             update_token_index,
                                             &for_info.first_update,
                                             &for_info.update_count)) {
                    return false;
                }
            } else if (!ast_next_token(state)) {
                return false;
            }
        } else {
            u32  first_node        = 0;
            bool first_raw_expr    = false;
            u32  first_token_index = state->token.token_index;
            if (!ast_parse_for_clause_item(
                    state, true, &first_node, &first_raw_expr)) {
                return false;
            }

            if (state->token.kind == TK_LBrace) {
                if (!first_raw_expr) {
                    return error_0203_expected_token(
                        state->lexer->source,
                        ast_token_span(state, &state->token),
                        TK_Semicolon,
                        state->token.kind);
                }
                for_info.condition_node_index = first_node;
            } else if (state->token.kind == TK_Comma ||
                       state->token.kind == TK_Semicolon) {
                if (state->token.kind == TK_Comma) {
                    if (!ast_parse_for_item_list(state,
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
                        !ast_wrap_for_expr_item(
                            state, first_token_index, &init_item)) {
                        return false;
                    }
                    for_info.first_init = (u32)array_count(state->for_items);
                    for_info.init_count = 1;
                    array_push(state->for_items, init_item);
                }

                if (!ast_expect_token(state, TK_Semicolon)) {
                    return false;
                }

                if (ast_cursor_kind(state) != TK_Semicolon) {
                    if (!ast_next_token(state)) {
                        return false;
                    }
                    bool previous_boundary = state->allow_statement_boundary;
                    state->allow_statement_boundary = true;
                    if (!ast_parse_expr(state,
                                        &for_info.condition_node_index)) {
                        state->allow_statement_boundary = previous_boundary;
                        return false;
                    }
                    state->allow_statement_boundary = previous_boundary;
                } else if (!ast_next_token(state)) {
                    return false;
                }

                if (state->token.kind != TK_Semicolon) {
                    return error_0203_expected_token(
                        state->lexer->source,
                        ast_token_span(state, &state->token),
                        TK_Semicolon,
                        state->token.kind);
                }
                if (!ast_expect_token(state, TK_Semicolon)) {
                    return false;
                }

                if (ast_cursor_kind(state) != TK_LBrace) {
                    if (!ast_next_token(state)) {
                        return false;
                    }
                    u32  update_node        = 0;
                    bool update_raw_expr    = false;
                    u32  update_token_index = state->token.token_index;
                    if (!ast_parse_for_clause_item(
                            state, false, &update_node, &update_raw_expr)) {
                        return false;
                    }
                    if (!ast_parse_for_item_list(state,
                                                 false,
                                                 TK_LBrace,
                                                 update_node,
                                                 update_raw_expr,
                                                 update_token_index,
                                                 &for_info.first_update,
                                                 &for_info.update_count)) {
                        return false;
                    }
                } else if (!ast_next_token(state)) {
                    return false;
                }
            } else {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_LBrace,
                    state->token.kind);
            }
        }
        if (state->token_index == state->token.token_index &&
            !ast_next_token(state)) {
            return false;
        }
    }
    if (!ast_parse_nested_block(state, &body_node)) {
        return false;
    }
    u32 for_info_index = (u32)array_count(state->fors);
    array_push(state->fors, for_info);
    state->nodes[for_node].a = for_info_index;
    state->nodes[for_node].b = body_node;
    if (out_node) {
        *out_node = for_node;
    }
    return true;
}

//------------------------------------------------------------------------------
// Parse one statement inside a function or nested block.

internal bool ast_parse_block_statement(AstParseState* state)
{
    if (state->token.kind == TK_break || state->token.kind == TK_continue) {
        AstKind kind = state->token.kind == TK_break ? AK_Break : AK_Continue;
        u32     token_index = state->token.token_index;
        u32     payload     = U32_MAX;
        u32     label       = U32_MAX;
        if (ast_cursor_kind(state) == TK_Dollar) {
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
        if (kind == AK_Break &&
            ast_token_starts_expression(ast_cursor_kind(state))) {
            if (!ast_next_token(state)) {
                return false;
            }
            bool previous_boundary          = state->allow_statement_boundary;
            state->allow_statement_boundary = true;
            if (!ast_parse_expr(state, &payload)) {
                state->allow_statement_boundary = previous_boundary;
                return false;
            }
            state->allow_statement_boundary = previous_boundary;
        }
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = kind,
                                 .token_index = token_index,
                                 .a           = payload,
                                 .b           = label,
                             },
                             NULL);
    }

    if (state->token.kind == TK_return) {
        u32 return_token_index   = state->token.token_index;
        u32 statement_expr_index = 0;
        if (!ast_next_token(state)) {
            return error_0201_missing_value(
                state->token.source,
                ast_token_span(state, &state->token),
                state->token.kind);
        }
        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        if (!ast_parse_expr(state, &statement_expr_index)) {
            state->allow_statement_boundary = previous_boundary;
            return false;
        }
        state->allow_statement_boundary = previous_boundary;

        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_Return,
                                 .token_index = return_token_index,
                                 .a           = statement_expr_index,
                             },
                             NULL);
    }

    if (state->token.kind == TK_LBrace) {
        return ast_parse_nested_block(state, NULL);
    }

    if (state->token.kind == TK_Dollar && ast_cursor_kind(state) == TK_Symbol &&
        ast_peek_kind_at(state, 1) == TK_for) {
        if (!ast_next_token(state)) {
            return false;
        }
        u32 label = state->token.value.symbol_handle;
        if (!ast_next_token(state)) {
            return false;
        }
        return ast_parse_for(state, label, NULL);
    }

    if (state->token.kind == TK_for) {
        return ast_parse_for(state, U32_MAX, NULL);
    }

    if (ast_symbol_starts_bind(state)) {
        if (ast_symbol_starts_variable(state)) {
            return ast_parse_variable(state, NULL);
        }
        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        bool ok                         = ast_parse_bind(state, NULL);
        state->allow_statement_boundary = previous_boundary;
        return ok;
    }

    if (ast_symbol_starts_variable(state)) {
        return ast_parse_variable(state, NULL);
    }

    if (ast_symbol_starts_assignment(state)) {
        return ast_parse_assignment(state, NULL);
    }

    u32 statement_expr_index        = 0;
    u32 statement_token             = state->token.token_index;

    bool previous_boundary          = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    if (!ast_parse_expr(state, &statement_expr_index)) {
        state->allow_statement_boundary = previous_boundary;
        return false;
    }
    state->allow_statement_boundary = previous_boundary;

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_Statement,
                             .token_index = statement_token,
                             .a           = statement_expr_index,
                         },
                         NULL);
}

//------------------------------------------------------------------------------
// Parse one function block body as a sequence of statements.

internal bool ast_parse_fn_block(AstParseState* state, u32 fn_start_index)
{
    ASSERT(state->token.kind == TK_LBrace, "Expected '{' token for fn block");

    if (!ast_next_token(state)) {
        return error_0201_missing_value(state->token.source,
                                        ast_token_span(state, &state->token),
                                        state->token.kind);
    }

    while (state->token.kind != TK_RBrace) {
        if (!ast_parse_block_statement(state)) {
            return false;
        }

        if (!ast_next_token(state)) {
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
    return true;
}

//------------------------------------------------------------------------------
// Parses a declaration of the form `fn (...) => <expression>` or
// `fn (...) -> <type> { ... }`.
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
    AstToken fn_token        = state->token;
    u32      signature_index = 0;

    if (!ast_parse_fn_signature(state, true, false, &signature_index)) {
        return false;
    }
    if (!ast_next_token(state)) {
        return error_0201_missing_value(state->token.source,
                                        ast_token_span(state, &state->token),
                                        state->token.kind);
    }
    u32 fn_start_index;
    u32 fn_end_index;
    u32 fn_def_index;

    EMIT_NODE(
        AK_FnStart, fn_token.token_index, signature_index, 0, fn_start_index);
    u32 fn_kind = AFK_Expr;

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

    EMIT_NODE(AK_FnEnd, fn_token.token_index, 0, fn_start_index, fn_end_index);
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

    bool         starts_type = ast_remaining_bind_value_is_type_syntax(state);
    ParsingQuery query       = starts_type
                                   ? PQ_Invalid
                                   : ast_parsing_query_for_token(state->token.kind);
    if (query == PQ_Invalid && !starts_type) {
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

    if (starts_type) {
        if (!ast_parse_type(state, &expr_index)) {
            return false;
        }
    } else if (query == PQ_Declaration) {
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

internal bool ast_parse_variable_payload(AstParseState* state,
                                         AstToken       symbol_token,
                                         u32*           out_value_node)
{
    u32 type_index = U32_MAX;

    if (!ast_expect_token(state, TK_Colon)) {
        return false;
    }

    if (!ast_next_token(state)) {
        return error_0205_expected_declaration_or_expression(
            state->token.source,
            ast_token_span(state, &state->token),
            TK_EOF,
            "Expected a type or initializer after ':', but found end of file");
    }

    if (state->token.kind == TK_Equal) {
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected an initializer after ':=', but found end of file");
        }
        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        bool ok                         = ast_parse_expr(state, out_value_node);
        state->allow_statement_boundary = previous_boundary;
        return ok;
    }

    if (!ast_parse_type(state, &type_index)) {
        return false;
    }

    if (ast_peek_token(state) && state->token.kind == TK_Equal) {
        u32 value_index = 0;
        if (!ast_expect_token(state, TK_Equal) || !ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected an initializer after '=', but found end of file");
        }
        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        bool ok                         = ast_parse_expr(state, &value_index);
        state->allow_statement_boundary = previous_boundary;
        if (!ok) {
            return false;
        }
        AstNode annotated = {
            .kind        = AK_AnnotatedValue,
            .token_index = symbol_token.token_index,
            .a           = type_index,
            .b           = value_index,
        };
        return ast_emit_node(state, annotated, out_value_node);
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_ZeroInit,
                             .token_index = symbol_token.token_index,
                             .a           = type_index,
                         },
                         out_value_node);
}

bool ast_parse_variable(AstParseState* state, u32* out_node)
{
    ASSERT(state->token.kind == TK_Symbol,
           "Expected symbol token for variable");
    AstToken symbol_token = state->token;
    u32      payload_node = 0;
    if (!ast_parse_variable_payload(state, symbol_token, &payload_node)) {
        return false;
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_Variable,
                             .token_index = symbol_token.token_index,
                             .a           = symbol_token.value.symbol_handle,
                             .b           = payload_node,
                         },
                         out_node);
}

bool ast_parse_assignment(AstParseState* state, u32* out_node)
{
    ASSERT(state->token.kind == TK_Symbol,
           "Expected symbol token for assignment");
    AstToken symbol_token = state->token;

    if (!ast_next_token(state)) {
        return false;
    }
    AstToken assign_token = state->token;
    if (assign_token.kind != TK_Equal) {
        AstKind ignored = AK_IntegerPlus;
        if (!ast_compound_assignment_binary_kind(assign_token.kind, &ignored)) {
            return error_0203_expected_token(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_Equal,
                state->token.kind);
        }
    }
    if (!ast_next_token(state)) {
        return error_0205_expected_declaration_or_expression(
            state->token.source,
            ast_token_span(state, &state->token),
            TK_EOF,
            "Expected an expression after '=', but found end of file");
    }

    u32  value_node                 = 0;
    bool previous_boundary          = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    bool ok                         = ast_parse_expr(state, &value_node);
    state->allow_statement_boundary = previous_boundary;
    if (!ok) {
        return false;
    }

    AstKind binary_kind = AK_IntegerPlus;
    if (ast_compound_assignment_binary_kind(assign_token.kind, &binary_kind)) {
        u32 symbol_ref = 0;
        if (!ast_emit_node(state,
                           (AstNode){
                               .kind        = AK_SymbolRef,
                               .token_index = symbol_token.token_index,
                               .a           = symbol_token.value.symbol_handle,
                           },
                           &symbol_ref)) {
            return false;
        }
        if (!ast_emit_node(state,
                           (AstNode){
                               .kind        = binary_kind,
                               .token_index = assign_token.token_index,
                               .a           = symbol_ref,
                               .b           = value_node,
                           },
                           &value_node)) {
            return false;
        }
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_Assign,
                             .token_index = symbol_token.token_index,
                             .a           = symbol_token.value.symbol_handle,
                             .b           = value_node,
                         },
                         out_node);
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
            if (ast_peek_kind_at(&state, 0) != TK_Colon) {
                goto invalid_binding_start;
            }
            if (ast_symbol_starts_bind(&state)) {
                if (!ast_parse_bind(&state, NULL)) {
                    goto error;
                }
            } else if (!ast_parse_variable(&state, NULL)) {
                goto error;
            }
            break;

        default:
        invalid_binding_start:
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

    return (Ast){
        .nodes            = state.nodes,
        .params           = state.params,
        .fn_signatures    = state.fn_signatures,
        .call_args        = state.call_args,
        .calls            = state.calls,
        .on_pattern_nodes = state.on_pattern_nodes,
        .on_branches      = state.on_branches,
        .ons              = state.ons,
        .for_items        = state.for_items,
        .fors             = state.fors,
    };

error:
    ast_done(&(Ast){.nodes            = state.nodes,
                    .params           = state.params,
                    .fn_signatures    = state.fn_signatures,
                    .call_args        = state.call_args,
                    .calls            = state.calls,
                    .on_pattern_nodes = state.on_pattern_nodes,
                    .on_branches      = state.on_branches,
                    .ons              = state.ons,
                    .for_items        = state.for_items,
                    .fors             = state.fors});
    return (Ast){0};
}

//------------------------------------------------------------------------------
// Free the parser's AST node table.

void ast_done(Ast* ast)
{
    array_free(ast->nodes);
    array_free(ast->params);
    array_free(ast->fn_signatures);
    array_free(ast->call_args);
    array_free(ast->calls);
    array_free(ast->on_pattern_nodes);
    array_free(ast->on_branches);
    array_free(ast->ons);
    array_free(ast->for_items);
    array_free(ast->fors);
    *ast = (Ast){0};
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
