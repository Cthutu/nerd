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

internal bool ast_skip_balanced_tokens(const AstParseState* state,
                                       u32*                 io_index,
                                       TokenKind            open_kind,
                                       TokenKind            close_kind)
{
    if (ast_kind_at_stream_index(state, *io_index) != open_kind) {
        return false;
    }
    u32 depth = 0;
    while (*io_index < array_count(state->lexer->tokens)) {
        TokenKind kind = ast_kind_at_stream_index(state, *io_index);
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

internal u32 ast_symbol_handle_at(const AstParseState* state, u32 token_index)
{
    u32 symbol_index = 0;
    for (u32 i = 0; i < token_index; ++i) {
        if (state->lexer->tokens[i].kind == TK_Symbol) {
            symbol_index++;
        }
    }
    ASSERT(symbol_index < array_count(state->lexer->symbol_handles),
           "Symbol token missing symbol handle");
    return state->lexer->symbol_handles[symbol_index];
}

internal ErrorSpan ast_span_for_token_index(const AstParseState* state,
                                            u32                  token_index)
{
    Token token = state->lexer->tokens[token_index];
    return (ErrorSpan){
        .start = token.offset,
        .end   = lex_token_end_offset(state->lexer, &token),
    };
}

internal void ast_sync_value_indices(AstParseState* state, u32 token_index)
{
    state->integer_index = 0;
    state->float_index   = 0;
    state->string_index  = 0;
    state->symbol_index  = 0;
    for (u32 i = 0; i < token_index; ++i) {
        switch (state->lexer->tokens[i].kind) {
        case TK_Integer:
            state->integer_index++;
            break;
        case TK_Float:
            state->float_index++;
            break;
        case TK_String:
            state->string_index++;
            break;
        case TK_Symbol:
            state->symbol_index++;
            break;
        default:
            break;
        }
    }
}

internal bool ast_skip_type_tokens(const AstParseState* state, u32* io_index)
{
    TokenKind kind = ast_kind_at_stream_index(state, *io_index);
    if (kind == TK_Symbol) {
        (*io_index)++;
        return true;
    }
    if (kind == TK_Caret) {
        (*io_index)++;
        return ast_skip_type_tokens(state, io_index);
    }
    if (kind == TK_plex || kind == TK_union) {
        (*io_index)++;
        while (kind == TK_plex &&
               ast_kind_at_stream_index(state, *io_index) == TK_Hash) {
            (*io_index)++;
            if (ast_kind_at_stream_index(state, *io_index) != TK_Symbol) {
                return false;
            }
            (*io_index)++;
        }
        if (ast_kind_at_stream_index(state, *io_index) != TK_LBrace) {
            return false;
        }
        (*io_index)++;
        while (ast_kind_at_stream_index(state, *io_index) != TK_RBrace) {
            if (ast_kind_at_stream_index(state, *io_index) == TK_EOF ||
                ast_kind_at_stream_index(state, *io_index) != TK_Symbol) {
                return false;
            }
            (*io_index)++;
            if (!ast_skip_type_tokens(state, io_index)) {
                return false;
            }
        }
        (*io_index)++;
        return true;
    }
    if (kind == TK_enum) {
        (*io_index)++;
        if (ast_kind_at_stream_index(state, *io_index) != TK_LBrace) {
            return false;
        }
        (*io_index)++;
        while (ast_kind_at_stream_index(state, *io_index) != TK_RBrace) {
            if (ast_kind_at_stream_index(state, *io_index) != TK_Symbol) {
                return false;
            }
            (*io_index)++;
            if (ast_kind_at_stream_index(state, *io_index) == TK_LParen) {
                (*io_index)++;
                if (ast_kind_at_stream_index(state, *io_index) != TK_RParen) {
                    for (;;) {
                        if (!ast_skip_type_tokens(state, io_index)) {
                            return false;
                        }
                        if (ast_kind_at_stream_index(state, *io_index) !=
                            TK_Comma) {
                            break;
                        }
                        (*io_index)++;
                    }
                }
                if (ast_kind_at_stream_index(state, *io_index) != TK_RParen) {
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
        if (ast_kind_at_stream_index(state, *io_index) == TK_Integer) {
            (*io_index)++;
        }
        if (ast_kind_at_stream_index(state, *io_index) != TK_RBracket) {
            return false;
        }
        (*io_index)++;
        return ast_skip_type_tokens(state, io_index);
    }
    if (kind == TK_LParen) {
        (*io_index)++;
        if (!ast_skip_type_tokens(state, io_index)) {
            return false;
        }
        if (ast_kind_at_stream_index(state, *io_index) == TK_Comma) {
            while (ast_kind_at_stream_index(state, *io_index) == TK_Comma) {
                (*io_index)++;
                if (ast_kind_at_stream_index(state, *io_index) == TK_RParen) {
                    break;
                }
                if (!ast_skip_type_tokens(state, io_index)) {
                    return false;
                }
            }
        }
        if (ast_kind_at_stream_index(state, *io_index) != TK_RParen) {
            return false;
        }
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

internal bool ast_tokens_cross_line_break(const AstParseState* state,
                                          u32                  previous_token,
                                          u32                  next_token)
{
    if (previous_token >= array_count(state->lexer->tokens) ||
        next_token >= array_count(state->lexer->tokens)) {
        return false;
    }

    const Token* previous = &state->lexer->tokens[previous_token];
    const Token* next     = &state->lexer->tokens[next_token];
    usize        start    = lex_token_end_offset(state->lexer, previous);
    usize        end      = next->offset;
    if (start > end || end > state->lexer->source.source.count) {
        return false;
    }

    const u8* source = state->lexer->source.source.data;
    for (usize i = start; i < end; ++i) {
        if (source[i] == '\n' || source[i] == '\r') {
            return true;
        }
    }
    return false;
}

internal bool
ast_remaining_bind_value_is_type_syntax(const AstParseState* state)
{
    u32 token_index = state->token.token_index;
    if (!ast_skip_type_tokens(state, &token_index)) {
        return false;
    }

    TokenKind next_kind = ast_kind_at_stream_index(state, token_index);
    if (next_kind == TK_RBrace) {
        return false;
    }

    if (next_kind == TK_EOF ||
        (next_kind == TK_Symbol &&
         ast_kind_at_stream_index(state, token_index + 1) == TK_Colon)) {
        return true;
    }

    if (token_index == 0) {
        return false;
    }

    return ast_tokens_cross_line_break(state, token_index - 1, token_index);
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

//------------------------------------------------------------------------------
// Parse an FFI function signature after `ffi "<library>"`.

internal bool ast_parse_ffi_signature(AstParseState* state,
                                      u32*           out_signature_index)
{
    ASSERT(state->token.kind == TK_LParen, "Expected FFI parameter list");

    u32  first_param = (u32)array_count(state->params);
    u32  param_count = 0;
    bool is_varargs  = false;

    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_RParen,
                                         TK_EOF);
    }

    if (state->token.kind != TK_RParen) {
        for (;;) {
            if (state->token.kind == TK_Ellipsis) {
                is_varargs = true;
                break;
            }

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
            ++param_count;

            if (ast_peek_kind_at(state, 0) == TK_Comma) {
                if (!ast_expect_token(state, TK_Comma) ||
                    !ast_next_token(state)) {
                    return error_0201_missing_value(
                        state->token.source,
                        ast_token_span(state, &state->token),
                        TK_RParen);
                }
                if (state->token.kind == TK_Ellipsis) {
                    is_varargs = true;
                    break;
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
    }

    u32 signature_index = (u32)array_count(state->fn_signatures);
    array_push(state->fn_signatures,
               (AstFnSignature){
                   .first_param            = first_param,
                   .param_count            = param_count,
                   .return_type_node_index = return_type,
                   .is_varargs             = is_varargs,
               });
    *out_signature_index = signature_index;
    return true;
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

    if (state->token.kind == TK_LParen) {
        AstToken lparen = state->token;
        if (!ast_next_token(state)) {
            return false;
        }

        u32 first_item = (u32)array_count(state->tuple_items);
        u32 item_count = 0;
        u32 first_type = 0;
        if (!ast_parse_type(state, &first_type)) {
            return false;
        }
        array_push(state->tuple_items, first_type);
        item_count++;

        bool is_tuple = false;
        while (ast_peek_kind_at(state, 0) == TK_Comma) {
            is_tuple = true;
            if (!ast_expect_token(state, TK_Comma)) {
                return false;
            }
            if (ast_peek_kind_at(state, 0) == TK_RParen) {
                break;
            }
            if (!ast_next_token(state)) {
                return false;
            }
            u32 item = 0;
            if (!ast_parse_type(state, &item)) {
                return false;
            }
            array_push(state->tuple_items, item);
            item_count++;
        }

        if (!ast_expect_token(state, TK_RParen)) {
            return false;
        }
        if (!is_tuple) {
            *out_node = first_type;
            return true;
        }
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_TypeTuple,
                                 .token_index = lparen.token_index,
                                 .a           = first_item,
                                 .b           = item_count,
                             },
                             out_node);
    }

    if (state->token.kind == TK_LBracket) {
        AstToken lbracket = state->token;
        if (!ast_next_token(state)) {
            return false;
        }
        if (state->token.kind == TK_RBracket) {
            if (!ast_next_token(state)) {
                return false;
            }
            u32 element_type = 0;
            if (!ast_parse_type(state, &element_type)) {
                return false;
            }
            return ast_emit_node(state,
                                 (AstNode){
                                     .kind        = AK_TypeSlice,
                                     .token_index = lbracket.token_index,
                                     .a           = element_type,
                                 },
                                 out_node);
        }
        if (state->token.kind != TK_Integer) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_Integer,
                state->token.kind);
        }
        AstNode length_node = {
            .kind        = AK_IntegerLiteral,
            .token_index = state->token.token_index,
            .a           = state->token.value.integer_index,
        };
        u32 length_index = 0;
        if (!ast_emit_node(state, length_node, &length_index) ||
            !ast_expect_token(state, TK_RBracket) || !ast_next_token(state)) {
            return false;
        }
        u32 element_type = 0;
        if (!ast_parse_type(state, &element_type)) {
            return false;
        }
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_TypeArray,
                                 .token_index = lbracket.token_index,
                                 .a           = length_index,
                                 .b           = element_type,
                             },
                             out_node);
    }

    if (state->token.kind == TK_Caret) {
        AstToken caret = state->token;
        if (!ast_next_token(state)) {
            return false;
        }
        u32 pointee_type = 0;
        if (!ast_parse_type(state, &pointee_type)) {
            return false;
        }
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_TypePointer,
                                 .token_index = caret.token_index,
                                 .a           = pointee_type,
                             },
                             out_node);
    }

    if (state->token.kind == TK_plex || state->token.kind == TK_union) {
        AstToken type_token = state->token;
        bool     is_union   = state->token.kind == TK_union;
        if (!ast_next_token(state)) {
            return false;
        }

        u32 flags = is_union ? APTF_Union : APTF_None;
        while (!is_union && state->token.kind == TK_Hash) {
            if (!ast_next_token(state) || state->token.kind != TK_Symbol) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &type_token),
                    TK_Symbol,
                    state->token.kind);
            }
            string annotation =
                lex_symbol(state->lexer, state->token.value.symbol_handle);
            if (string_eq(annotation, s("c"))) {
                flags |= APTF_C;
            } else if (string_eq(annotation, s("packed"))) {
                flags |= APTF_C | APTF_Packed;
            } else {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_Symbol,
                    state->token.kind);
            }
            if (!ast_next_token(state)) {
                return false;
            }
        }

        if (state->token.kind != TK_LBrace) {
            return error_0203_expected_token(state->lexer->source,
                                             ast_token_span(state, &type_token),
                                             TK_LBrace,
                                             state->token.kind);
        }
        if (!ast_next_token(state)) {
            return false;
        }

        u32 first_field = (u32)array_count(state->plex_fields);
        u32 field_count = 0;
        while (state->token.kind != TK_RBrace) {
            if (state->token.kind != TK_Symbol) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_Symbol,
                    state->token.kind);
            }
            AstToken field = state->token;
            if (!ast_next_token(state)) {
                return false;
            }
            u32 type_node = 0;
            if (!ast_parse_type(state, &type_node)) {
                return false;
            }
            array_push(state->plex_fields,
                       (AstPlexField){
                           .token_index     = field.token_index,
                           .symbol_handle   = field.value.symbol_handle,
                           .type_node_index = type_node,
                       });
            field_count++;
            if (!ast_next_token(state)) {
                return false;
            }
        }
        u32 plex_type_index = (u32)array_count(state->plex_types);
        array_push(state->plex_types,
                   (AstPlexTypeInfo){
                       .first_field = first_field,
                       .field_count = field_count,
                       .flags       = flags,
                   });
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_TypePlex,
                                 .token_index = type_token.token_index,
                                 .a           = plex_type_index,
                             },
                             out_node);
    }

    if (state->token.kind == TK_enum) {
        AstToken enum_token = state->token;
        if (!ast_next_token(state) || state->token.kind != TK_LBrace) {
            return error_0203_expected_token(state->lexer->source,
                                             ast_token_span(state, &enum_token),
                                             TK_LBrace,
                                             state->token.kind);
        }
        if (!ast_next_token(state)) {
            return false;
        }

        u32 first_variant = (u32)array_count(state->enum_variants);
        u32 variant_count = 0;
        while (state->token.kind != TK_RBrace) {
            if (state->token.kind != TK_Symbol) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_Symbol,
                    state->token.kind);
            }
            AstToken variant_token     = state->token;
            u32      variant_type_node = U32_MAX;
            if (ast_peek_kind_at(state, 0) == TK_LParen) {
                AstToken lparen = state->token;
                if (!ast_expect_token(state, TK_LParen) ||
                    !ast_next_token(state)) {
                    return false;
                }
                u32 first_item = (u32)array_count(state->tuple_items);
                u32 item_count = 0;
                if (state->token.kind != TK_RParen) {
                    for (;;) {
                        u32 item = 0;
                        if (!ast_parse_type(state, &item)) {
                            return false;
                        }
                        array_push(state->tuple_items, item);
                        item_count++;
                        if (ast_peek_kind_at(state, 0) != TK_Comma) {
                            break;
                        }
                        if (!ast_expect_token(state, TK_Comma) ||
                            !ast_next_token(state)) {
                            return false;
                        }
                    }
                }
                if (!ast_expect_token(state, TK_RParen)) {
                    return false;
                }
                if (item_count == 1) {
                    variant_type_node = state->tuple_items[first_item];
                } else {
                    if (!ast_emit_node(state,
                                       (AstNode){
                                           .kind        = AK_TypeTuple,
                                           .token_index = lparen.token_index,
                                           .a           = first_item,
                                           .b           = item_count,
                                       },
                                       &variant_type_node)) {
                        return false;
                    }
                }
            }
            array_push(state->enum_variants,
                       (AstEnumVariant){
                           .token_index     = variant_token.token_index,
                           .symbol_handle   = variant_token.value.symbol_handle,
                           .type_node_index = variant_type_node,
                       });
            variant_count++;
            if (!ast_next_token(state)) {
                return false;
            }
        }
        u32 enum_type_index = (u32)array_count(state->enum_types);
        array_push(state->enum_types,
                   (AstEnumTypeInfo){
                       .first_variant = first_variant,
                       .variant_count = variant_count,
                   });
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_TypeEnum,
                                 .token_index = enum_token.token_index,
                                 .a           = enum_type_index,
                             },
                             out_node);
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
    if (kind == TK_fn || kind == TK_ffi || kind == TK_mod) {
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
internal bool ast_parse_destructure(AstParseState* state, u32* out_node);
internal bool ast_parse_destructure_pattern(AstParseState* state,
                                            u32*           out_pattern);
internal bool ast_parse_use(AstParseState* state, u32* out_node);

internal bool ast_symbol_is_underscore(const Lexer* lexer, u32 symbol_handle)
{
    return string_eq_cstr(lex_symbol(lexer, symbol_handle), "_");
}

internal bool
ast_emit_pattern(AstParseState* state, AstPattern pattern, u32* out_pattern)
{
    *out_pattern = (u32)array_count(state->patterns);
    array_push(state->patterns, pattern);
    return true;
}

internal bool ast_parse_plex_pattern(AstParseState* state, u32* out_pattern)
{
    AstToken open = state->token;
    ASSERT(open.kind == TK_LBrace, "Expected plex pattern");
    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &open),
                                         TK_RBrace,
                                         TK_EOF);
    }

    u32 first_field = (u32)array_count(state->pattern_fields);
    u32 field_count = 0;
    while (state->token.kind != TK_RBrace) {
        if (state->token.kind != TK_Symbol) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_Symbol,
                state->token.kind);
        }
        AstToken field         = state->token;
        u32      field_pattern = U32_MAX;
        if (ast_symbol_is_underscore(state->lexer, field.value.symbol_handle)) {
            if (!ast_emit_pattern(state,
                                  (AstPattern){
                                      .kind        = APK_Ignore,
                                      .token_index = field.token_index,
                                  },
                                  &field_pattern)) {
                return false;
            }
            if (!ast_next_token(state)) {
                return false;
            }
        } else if (ast_peek_kind_at(state, 0) == TK_Colon) {
            if (!ast_next_token(state) || state->token.kind != TK_Colon ||
                !ast_next_token(state)) {
                return false;
            }
            if (!ast_parse_pattern(state, &field_pattern)) {
                return false;
            }
        } else {
            if (!ast_parse_pattern(state, &field_pattern)) {
                return false;
            }
        }
        array_push(state->pattern_fields,
                   (AstPlexPatternField){
                       .token_index   = field.token_index,
                       .symbol_handle = field.value.symbol_handle,
                       .pattern_index = field_pattern,
                   });
        ++field_count;
        if (state->token.kind == TK_Comma) {
            if (state->token_index == state->token.token_index &&
                !ast_next_token(state)) {
                return false;
            }
            if (!ast_next_token(state)) {
                return false;
            }
            if (state->token.kind == TK_RBrace) {
                break;
            }
            continue;
        }
        if (ast_peek_kind_at(state, 0) == TK_Comma) {
            if (!ast_expect_token(state, TK_Comma)) {
                return false;
            }
            if (!ast_next_token(state)) {
                return false;
            }
            if (state->token.kind == TK_RBrace) {
                break;
            }
            continue;
        }
    }

    if (state->token.kind == TK_RBrace &&
        state->token_index == state->token.token_index) {
        if (!ast_next_token(state)) {
            return false;
        }
    } else if (state->token.kind != TK_RBrace &&
               !ast_expect_token(state, TK_RBrace)) {
        return false;
    }
    if (state->token.kind == TK_RBrace &&
        state->token_index != state->token.token_index &&
        !ast_next_token(state)) {
        return false;
    }
    return ast_emit_pattern(state,
                            (AstPattern){
                                .kind        = APK_Plex,
                                .token_index = open.token_index,
                                .a           = first_field,
                                .b           = field_count,
                            },
                            out_pattern);
}

internal bool ast_parse_tuple_pattern(AstParseState* state, u32* out_pattern)
{
    AstToken open = state->token;
    ASSERT(open.kind == TK_LParen, "Expected tuple pattern");
    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &open),
                                         TK_RParen,
                                         TK_EOF);
    }

    u32 first_item = (u32)array_count(state->pattern_items);
    u32 item_count = 0;
    if (state->token.kind != TK_RParen) {
        for (;;) {
            u32 item = U32_MAX;
            if (!ast_parse_pattern(state, &item)) {
                return false;
            }
            array_push(state->pattern_items, item);
            ++item_count;
            if (state->token.kind != TK_Comma &&
                ast_peek_kind_at(state, 0) == TK_Comma) {
                if (!ast_expect_token(state, TK_Comma)) {
                    return false;
                }
            }
            if (state->token.kind != TK_Comma) {
                break;
            }
            if (state->token_index == state->token.token_index &&
                !ast_next_token(state)) {
                return false;
            }
            if (!ast_next_token(state)) {
                return false;
            }
            if (state->token.kind == TK_RParen) {
                break;
            }
        }
    }
    if (state->token.kind == TK_RParen &&
        state->token_index == state->token.token_index) {
        if (!ast_next_token(state)) {
            return false;
        }
    } else if (state->token.kind != TK_RParen &&
               !ast_expect_token(state, TK_RParen)) {
        return false;
    }
    if (state->token.kind == TK_RParen &&
        state->token_index != state->token.token_index &&
        !ast_next_token(state)) {
        return false;
    }
    return ast_emit_pattern(state,
                            (AstPattern){
                                .kind        = APK_Tuple,
                                .token_index = open.token_index,
                                .a           = first_item,
                                .b           = item_count,
                            },
                            out_pattern);
}

internal bool ast_parse_enum_variant_pattern(AstParseState* state,
                                             u32*           out_pattern)
{
    AstToken variant = state->token;
    ASSERT(variant.kind == TK_Symbol, "Expected enum variant pattern");
    if (!ast_expect_token(state, TK_LParen) || !ast_next_token(state)) {
        return false;
    }

    u32 first_pattern = (u32)array_count(state->pattern_items);
    u32 pattern_count = 0;
    if (state->token.kind != TK_RParen) {
        for (;;) {
            u32 item = U32_MAX;
            if (!ast_parse_pattern(state, &item)) {
                return false;
            }
            array_push(state->pattern_items, item);
            pattern_count++;
            if (state->token.kind != TK_Comma &&
                ast_peek_kind_at(state, 0) == TK_Comma) {
                if (!ast_expect_token(state, TK_Comma)) {
                    return false;
                }
            }
            if (state->token.kind != TK_Comma) {
                break;
            }
            if (state->token_index == state->token.token_index &&
                !ast_next_token(state)) {
                return false;
            }
            if (!ast_next_token(state)) {
                return false;
            }
            if (state->token.kind == TK_RParen) {
                break;
            }
        }
    }
    if (state->token.kind == TK_RParen &&
        state->token_index == state->token.token_index) {
        if (!ast_next_token(state)) {
            return false;
        }
    } else if (state->token.kind != TK_RParen &&
               !ast_expect_token(state, TK_RParen)) {
        return false;
    }
    if (state->token.kind == TK_RParen &&
        state->token_index != state->token.token_index &&
        !ast_next_token(state)) {
        return false;
    }

    u32 enum_pattern_index = (u32)array_count(state->enum_patterns);
    array_push(state->enum_patterns,
               (AstEnumPattern){
                   .token_index   = variant.token_index,
                   .symbol_handle = variant.value.symbol_handle,
                   .first_pattern = first_pattern,
                   .pattern_count = pattern_count,
               });
    return ast_emit_pattern(state,
                            (AstPattern){
                                .kind        = APK_EnumVariant,
                                .token_index = variant.token_index,
                                .a           = enum_pattern_index,
                            },
                            out_pattern);
}

internal AstPatternKind ast_parse_comparison_pattern_kind(TokenKind kind)
{
    switch (kind) {
    case TK_EqualEqual:
        return APK_Equal;
    case TK_BangEqual:
        return APK_NotEqual;
    case TK_Less:
        return APK_Less;
    case TK_LessEqual:
        return APK_LessEqual;
    case TK_Greater:
        return APK_Greater;
    case TK_GreaterEqual:
        return APK_GreaterEqual;
    default:
        return APK_Value;
    }
}

bool ast_parse_pattern(AstParseState* state, u32* out_pattern)
{
    if (state->token.kind == TK_as) {
        AstToken as_token = state->token;
        if (!ast_next_token(state)) {
            return error_0203_expected_token(state->lexer->source,
                                             ast_token_span(state, &as_token),
                                             TK_Symbol,
                                             TK_EOF);
        }
        if (state->token.kind != TK_Symbol ||
            ast_symbol_is_underscore(state->lexer,
                                     state->token.value.symbol_handle)) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_Symbol,
                state->token.kind);
        }
        AstToken symbol = state->token;
        if (!ast_next_token(state)) {
            return false;
        }
        return ast_emit_pattern(state,
                                (AstPattern){
                                    .kind        = APK_Bind,
                                    .token_index = symbol.token_index,
                                    .a           = symbol.value.symbol_handle,
                                    .b           = U32_MAX,
                                },
                                out_pattern);
    }

    AstPatternKind comparison_kind =
        ast_parse_comparison_pattern_kind(state->token.kind);
    if (comparison_kind != APK_Value) {
        AstToken comparison = state->token;
        if (!ast_next_token(state)) {
            return error_0201_missing_value(state->token.source,
                                            ast_token_span(state, &comparison),
                                            TK_Integer);
        }

        bool saved_statement_boundary   = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        u32 value_node                  = 0;
        if (!ast_parse_expr_bp(state, 0, &value_node)) {
            state->allow_statement_boundary = saved_statement_boundary;
            return false;
        }
        state->allow_statement_boundary = saved_statement_boundary;

        return ast_emit_pattern(state,
                                (AstPattern){
                                    .kind        = comparison_kind,
                                    .token_index = comparison.token_index,
                                    .a           = value_node,
                                },
                                out_pattern);
    }

    if (state->token.kind == TK_Symbol &&
        ast_symbol_is_underscore(state->lexer,
                                 state->token.value.symbol_handle)) {
        AstToken token = state->token;
        if (state->token_index == state->token.token_index &&
            !ast_next_token(state)) {
            return false;
        }
        if (!ast_next_token(state)) {
            return false;
        }
        return ast_emit_pattern(state,
                                (AstPattern){
                                    .kind        = APK_Ignore,
                                    .token_index = token.token_index,
                                },
                                out_pattern);
    }

    if (state->token.kind == TK_LParen) {
        return ast_parse_tuple_pattern(state, out_pattern);
    }

    if (state->token.kind == TK_LBrace) {
        return ast_parse_plex_pattern(state, out_pattern);
    }

    if (state->token.kind == TK_Symbol &&
        ast_peek_kind_at(state, 0) == TK_LParen) {
        return ast_parse_enum_variant_pattern(state, out_pattern);
    }

    bool saved_statement_boundary   = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    u32 start_node                  = 0;
    if (!ast_parse_expr_bp(state, 0, &start_node)) {
        state->allow_statement_boundary = saved_statement_boundary;
        return false;
    }
    state->allow_statement_boundary = saved_statement_boundary;

    AstPatternKind range_kind       = APK_Value;
    if (state->token.kind == TK_Range) {
        range_kind = APK_RangeExclusive;
    } else if (state->token.kind == TK_RangeInclusive) {
        range_kind = APK_RangeInclusive;
    } else {
        return ast_emit_pattern(
            state,
            (AstPattern){
                .kind        = APK_Value,
                .token_index = state->nodes[start_node].token_index,
                .a           = start_node,
            },
            out_pattern);
    }

    AstToken range_token = state->token;
    if (!ast_next_token(state) || !ast_next_token(state)) {
        return error_0201_missing_value(state->token.source,
                                        ast_token_span(state, &range_token),
                                        TK_Integer);
    }

    saved_statement_boundary        = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    u32 end_node                    = 0;
    if (!ast_parse_expr_bp(state, 0, &end_node)) {
        state->allow_statement_boundary = saved_statement_boundary;
        return false;
    }
    state->allow_statement_boundary = saved_statement_boundary;
    return ast_emit_pattern(state,
                            (AstPattern){
                                .kind        = range_kind,
                                .token_index = range_token.token_index,
                                .a           = start_node,
                                .b           = end_node,
                            },
                            out_pattern);
}

internal bool ast_parse_destructure_tuple_pattern(AstParseState* state,
                                                  u32*           out_pattern)
{
    AstToken open = state->token;
    ASSERT(open.kind == TK_LParen, "Expected tuple destructuring pattern");
    u32 first_item = (u32)array_count(state->pattern_items);
    u32 item_count = 0;

    u32 cursor     = open.token_index + 1;
    while (cursor < array_count(state->lexer->tokens) &&
           state->lexer->tokens[cursor].kind != TK_RParen) {
        if (state->lexer->tokens[cursor].kind != TK_Symbol) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_span_for_token_index(state, cursor),
                TK_Symbol,
                state->lexer->tokens[cursor].kind);
        }
        u32 symbol = ast_symbol_handle_at(state, cursor);
        u32 item   = U32_MAX;
        if (!ast_emit_pattern(
                state,
                (AstPattern){
                    .kind = ast_symbol_is_underscore(state->lexer, symbol)
                                ? APK_Ignore
                                : APK_Bind,
                    .token_index = cursor,
                    .a           = symbol,
                    .b           = U32_MAX,
                },
                &item)) {
            return false;
        }
        array_push(state->pattern_items, item);
        item_count++;
        cursor++;
        if (state->lexer->tokens[cursor].kind == TK_Comma) {
            cursor++;
            if (state->lexer->tokens[cursor].kind == TK_RParen) {
                break;
            }
            continue;
        }
        if (state->lexer->tokens[cursor].kind != TK_RParen) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_span_for_token_index(state, cursor),
                TK_RParen,
                state->lexer->tokens[cursor].kind);
        }
    }
    if (cursor >= array_count(state->lexer->tokens) ||
        state->lexer->tokens[cursor].kind != TK_RParen) {
        return error_0203_expected_token(
            state->lexer->source,
            ast_span_for_token_index(state, open.token_index),
            TK_RParen,
            TK_EOF);
    }
    state->token_index = cursor + 1;
    ast_sync_value_indices(state, state->token_index);
    if (!ast_next_token(state)) {
        return false;
    }
    return ast_emit_pattern(state,
                            (AstPattern){
                                .kind        = APK_Tuple,
                                .token_index = open.token_index,
                                .a           = first_item,
                                .b           = item_count,
                            },
                            out_pattern);
}

internal bool ast_parse_destructure_plex_pattern(AstParseState* state,
                                                 u32*           out_pattern)
{
    AstToken open = state->token;
    ASSERT(open.kind == TK_LBrace, "Expected plex destructuring pattern");
    u32 first_field = (u32)array_count(state->pattern_fields);
    u32 field_count = 0;

    u32 cursor      = open.token_index + 1;
    while (cursor < array_count(state->lexer->tokens) &&
           state->lexer->tokens[cursor].kind != TK_RBrace) {
        if (state->lexer->tokens[cursor].kind != TK_Symbol) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_span_for_token_index(state, cursor),
                TK_Symbol,
                state->lexer->tokens[cursor].kind);
        }
        u32  field_symbol = ast_symbol_handle_at(state, cursor);
        u32  field_token  = cursor;
        u32  bind_symbol  = field_symbol;
        bool ignore = ast_symbol_is_underscore(state->lexer, field_symbol);
        cursor++;
        if (state->lexer->tokens[cursor].kind == TK_Colon) {
            cursor++;
            if (state->lexer->tokens[cursor].kind != TK_Symbol) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_span_for_token_index(state, cursor),
                    TK_Symbol,
                    state->lexer->tokens[cursor].kind);
            }
            bind_symbol = ast_symbol_handle_at(state, cursor);
            ignore      = ast_symbol_is_underscore(state->lexer, bind_symbol);
            cursor++;
        }

        u32 field_pattern = U32_MAX;
        if (!ast_emit_pattern(state,
                              (AstPattern){
                                  .kind        = ignore ? APK_Ignore : APK_Bind,
                                  .token_index = field_token,
                                  .a           = bind_symbol,
                                  .b           = U32_MAX,
                              },
                              &field_pattern)) {
            return false;
        }
        array_push(state->pattern_fields,
                   (AstPlexPatternField){
                       .token_index   = field_token,
                       .symbol_handle = field_symbol,
                       .pattern_index = field_pattern,
                   });
        field_count++;
        if (state->lexer->tokens[cursor].kind == TK_Comma) {
            cursor++;
            if (state->lexer->tokens[cursor].kind == TK_RBrace) {
                break;
            }
            continue;
        }
        if (state->lexer->tokens[cursor].kind != TK_RBrace) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_span_for_token_index(state, cursor),
                TK_RBrace,
                state->lexer->tokens[cursor].kind);
        }
    }
    if (cursor >= array_count(state->lexer->tokens) ||
        state->lexer->tokens[cursor].kind != TK_RBrace) {
        return error_0203_expected_token(
            state->lexer->source,
            ast_span_for_token_index(state, open.token_index),
            TK_RBrace,
            TK_EOF);
    }
    state->token_index = cursor + 1;
    ast_sync_value_indices(state, state->token_index);
    if (!ast_next_token(state)) {
        return false;
    }
    return ast_emit_pattern(state,
                            (AstPattern){
                                .kind        = APK_Plex,
                                .token_index = open.token_index,
                                .a           = first_field,
                                .b           = field_count,
                            },
                            out_pattern);
}

internal bool ast_parse_destructure_pattern(AstParseState* state,
                                            u32*           out_pattern)
{
    if (state->token.kind == TK_Symbol) {
        AstToken token = state->token;
        if (!ast_next_token(state)) {
            return false;
        }
        return ast_emit_pattern(
            state,
            (AstPattern){
                .kind        = ast_symbol_is_underscore(state->lexer,
                                                 token.value.symbol_handle)
                                   ? APK_Ignore
                                   : APK_Bind,
                .token_index = token.token_index,
                .a           = token.value.symbol_handle,
                .b           = U32_MAX,
            },
            out_pattern);
    }
    if (state->token.kind == TK_LParen) {
        return ast_parse_destructure_tuple_pattern(state, out_pattern);
    }
    if (state->token.kind == TK_LBrace) {
        return ast_parse_destructure_plex_pattern(state, out_pattern);
    }
    return ast_parse_pattern(state, out_pattern);
}

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

    if (state->token.kind != terminator &&
        !(terminator == TK_LBrace && state->token.kind == TK_Dollar)) {
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

internal bool ast_for_token_is_body_start(TokenKind kind)
{
    return kind == TK_LBrace || kind == TK_Dollar;
}

internal bool ast_parse_for_body(AstParseState* state,
                                 AstForInfo*    for_info,
                                 u32*           out_body_node)
{
    if (!ast_for_token_is_body_start(state->token.kind) &&
        ast_for_token_is_body_start(ast_cursor_kind(state))) {
        if (!ast_next_token(state)) {
            return false;
        }
    }

    if (state->token.kind == TK_Dollar) {
        if (!ast_next_token(state) || state->token.kind != TK_Symbol) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_Symbol,
                state->token.kind);
        }
        for_info->label_symbol = state->token.value.symbol_handle;
        if (!ast_next_token(state)) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_LBrace,
                state->token.kind);
        }
    }

    if (state->token.kind != TK_LBrace) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_LBrace,
                                         state->token.kind);
    }

    return ast_parse_nested_block(state, out_body_node);
}

bool ast_parse_for(AstParseState* state, u32* out_node)
{
    u32        for_token_index = state->token.token_index;
    u32        for_node        = 0;
    AstForInfo for_info        = {
               .first_init           = U32_MAX,
               .init_count           = 0,
               .condition_node_index = U32_MAX,
               .first_update         = U32_MAX,
               .update_count         = 0,
               .label_symbol         = U32_MAX,
               .else_block_index     = U32_MAX,
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
    if (!ast_for_token_is_body_start(state->token.kind)) {
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

            if (!ast_for_token_is_body_start(ast_cursor_kind(state))) {
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

            if (ast_for_token_is_body_start(state->token.kind)) {
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

                if (!ast_for_token_is_body_start(ast_cursor_kind(state))) {
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
    if (!ast_parse_for_body(state, &for_info, &body_node)) {
        return false;
    }
    if (ast_cursor_kind(state) == TK_else) {
        if (!ast_next_token(state) || !ast_next_token(state)) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_LBrace,
                state->token.kind);
        }
        if (state->token.kind != TK_LBrace) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_LBrace,
                state->token.kind);
        }
        if (!ast_parse_nested_block(state, &for_info.else_block_index)) {
            return false;
        }
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
    if (state->token.kind == TK_ffi) {
        return ast_parse_declaration(state, NULL);
    }

    if (state->token.kind == TK_use) {
        return ast_parse_use(state, NULL);
    }

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
        if (ast_peek_kind_at(state, 0) == TK_RBrace ||
            ast_peek_kind_at(state, 0) == TK_EOF) {
            return ast_emit_node(state,
                                 (AstNode){
                                     .kind        = AK_Return,
                                     .token_index = return_token_index,
                                     .a           = U32_MAX,
                                 },
                                 NULL);
        }
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

    if (state->token.kind == TK_LParen || state->token.kind == TK_LBrace) {
        u32 cursor = state->token.token_index;
        if (ast_skip_balanced_tokens(
                state,
                &cursor,
                state->token.kind,
                state->token.kind == TK_LParen ? TK_RParen : TK_RBrace) &&
            (ast_kind_at_stream_index(state, cursor) == TK_Colon ||
             ast_kind_at_stream_index(state, cursor) == TK_Equal)) {
            return ast_parse_destructure(state, NULL);
        }
    }

    if (state->token.kind == TK_LBrace) {
        return ast_parse_nested_block(state, NULL);
    }

    if (state->token.kind == TK_for) {
        return ast_parse_for(state, NULL);
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

internal bool ast_parse_use(AstParseState* state, u32* out_node)
{
    ASSERT(state->token.kind == TK_use, "Expected 'use' token");
    AstToken use_token = state->token;

    if (!ast_next_token(state)) {
        return error_0205_expected_declaration_or_expression(
            state->lexer->source,
            ast_token_span(state, &use_token),
            TK_EOF,
            "Expected a module expression after 'use', but found end of file");
    }

    u32  module_node                = 0;
    bool previous_boundary          = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    bool ok                         = false;
    if (state->token.kind == TK_Symbol &&
        ast_peek_kind_at(state, 0) == TK_Dot) {
        AstToken path_token   = state->token;
        u32      first_symbol = (u32)array_count(state->module_path_symbols);
        u32      symbol_count = 0;

        for (;;) {
            array_push(state->module_path_symbols,
                       state->token.value.symbol_handle);
            ++symbol_count;
            if (ast_peek_kind_at(state, 0) != TK_Dot) {
                break;
            }
            if (!ast_expect_token(state, TK_Dot) || !ast_next_token(state) ||
                state->token.kind != TK_Symbol) {
                state->allow_statement_boundary = previous_boundary;
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_Symbol,
                    state->token.kind);
            }
        }

        u32 module_path_index = (u32)array_count(state->module_paths);
        array_push(state->module_paths,
                   (AstModulePath){
                       .first_symbol = first_symbol,
                       .symbol_count = symbol_count,
                   });
        ok = ast_emit_node(state,
                           (AstNode){
                               .kind        = AK_ModRef,
                               .token_index = path_token.token_index,
                               .a           = module_path_index,
                           },
                           &module_node);
    } else {
        ok = ast_parse_expr(state, &module_node);
    }
    state->allow_statement_boundary = previous_boundary;
    if (!ok) {
        return false;
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = AK_Use,
                             .token_index = use_token.token_index,
                             .a           = module_node,
                         },
                         out_node);
}

internal bool ast_parse_top_level_item(AstParseState* state);

internal bool ast_parse_top_level_on(AstParseState* state, u32* out_node)
{
    ASSERT(state->token.kind == TK_on, "Expected 'on' token");
    AstToken on_token = state->token;

    if (!ast_next_token(state)) {
        return error_0201_missing_value(
            state->lexer->source, ast_token_span(state, &on_token), TK_on);
    }

    bool is_negated = false;
    if (state->token.kind == TK_Bang) {
        is_negated = true;
        if (!ast_next_token(state)) {
            return error_0203_expected_token(state->lexer->source,
                                             ast_token_span(state, &on_token),
                                             TK_Symbol,
                                             TK_EOF);
        }
    }

    if (state->token.kind != TK_Symbol) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_Symbol,
                                         state->token.kind);
    }
    u32 symbol_handle = state->token.value.symbol_handle;

    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_LBrace,
                                         TK_EOF);
    }
    if (state->token.kind != TK_LBrace) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_LBrace,
                                         state->token.kind);
    }

    u32 top_on_node = 0;
    if (!ast_emit_node(state,
                       (AstNode){
                           .kind        = AK_TopOn,
                           .token_index = on_token.token_index,
                       },
                       &top_on_node)) {
        return false;
    }

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
    u32 first_item = (u32)array_count(state->nodes);

    if (!ast_next_token(state)) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_RBrace,
                                         TK_EOF);
    }

    while (state->token.kind != TK_RBrace) {
        bool previous_boundary          = state->allow_statement_boundary;
        state->allow_statement_boundary = true;
        bool ok                         = ast_parse_top_level_item(state);
        state->allow_statement_boundary = previous_boundary;
        if (!ok) {
            return false;
        }

        if (!ast_next_token(state)) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_RBrace,
                TK_EOF);
        }
    }

    u32 top_on_index = (u32)array_count(state->top_ons);
    array_push(state->top_ons,
               (AstTopOnInfo){
                   .symbol_handle   = symbol_handle,
                   .body_node_index = block_index,
                   .is_negated      = is_negated,
               });
    state->nodes[top_on_node].a = top_on_index;
    state->nodes[block_index].a = first_item;
    state->nodes[block_index].b = (u32)array_count(state->nodes);
    if (out_node) {
        *out_node = top_on_node;
    }
    return true;
}

internal bool ast_parse_top_level_item(AstParseState* state)
{
    bool is_public = false;
    if (state->token.kind == TK_pub) {
        is_public = true;
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected a top-level binding after 'pub', but found end of "
                "file");
        }
    }

    switch (state->token.kind) {
    case TK_ffi:
        if (is_public) {
            return error_0204_unexpected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                state->token.kind,
                "Expected a symbol to start a "
                "public binding");
        }
        return ast_parse_declaration(state, NULL);
    case TK_use:
        if (is_public) {
            return error_0204_unexpected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                state->token.kind,
                "Expected a symbol to start a "
                "public binding");
        }
        return ast_parse_use(state, NULL);
    case TK_on:
        if (is_public) {
            return error_0204_unexpected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                state->token.kind,
                "Expected a symbol to start a "
                "public binding");
        }
        return ast_parse_top_level_on(state, NULL);
    case TK_Symbol:
        if (ast_peek_kind_at(state, 0) != TK_Colon) {
            break;
        }
        if (ast_symbol_starts_bind(state)) {
            u32 bind_index = 0;
            if (!ast_parse_bind(state, &bind_index)) {
                return false;
            }
            if (is_public) {
                ast_set_flag(&state->nodes[bind_index], ANF_Public);
            }
            return true;
        }
        if (is_public) {
            return error_0204_unexpected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                state->token.kind,
                "Expected a symbol to start a "
                "public binding");
        }
        return ast_parse_variable(state, NULL);
    default:
        break;
    }

    if (is_public) {
        return error_0204_unexpected_token(state->lexer->source,
                                           ast_token_span(state, &state->token),
                                           state->token.kind,
                                           "Expected a symbol to start a "
                                           "public binding");
    }

    if (!ast_parse_expr(state, NULL)) {
        return false;
    }

    if (ast_peek_token(state)) {
        error_0204_unexpected_token(state->lexer->source,
                                    ast_token_span(state, &state->token),
                                    state->token.kind,
                                    "Expected a symbol to start a binding");
    } else {
        AstToken eof = {
            .kind        = TK_EOF,
            .source      = state->lexer->source,
            .offset      = state->lexer->source.source.count,
            .token_index = (u32)array_count(state->lexer->tokens),
        };
        error_0204_unexpected_token(state->lexer->source,
                                    ast_token_span(state, &eof),
                                    eof.kind,
                                    "Expected a symbol to start a binding");
    }
    return false;
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
    if (state->token.kind == TK_mod) {
        AstToken mod_token    = state->token;
        u32      first_symbol = (u32)array_count(state->module_path_symbols);
        u32      symbol_count = 0;

        if (!ast_next_token(state) || state->token.kind != TK_Symbol) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_Symbol,
                state->token.kind);
        }

        for (;;) {
            array_push(state->module_path_symbols,
                       state->token.value.symbol_handle);
            ++symbol_count;
            if (ast_peek_kind_at(state, 0) != TK_Dot) {
                break;
            }
            if (!ast_expect_token(state, TK_Dot) || !ast_next_token(state) ||
                state->token.kind != TK_Symbol) {
                return error_0203_expected_token(
                    state->lexer->source,
                    ast_token_span(state, &state->token),
                    TK_Symbol,
                    state->token.kind);
            }
        }

        u32 module_path_index = (u32)array_count(state->module_paths);
        array_push(state->module_paths,
                   (AstModulePath){
                       .first_symbol = first_symbol,
                       .symbol_count = symbol_count,
                   });
        return ast_emit_node(state,
                             (AstNode){
                                 .kind        = AK_ModRef,
                                 .token_index = mod_token.token_index,
                                 .a           = module_path_index,
                             },
                             out_node);
    }

    if (state->token.kind == TK_ffi) {
        AstToken ffi_token = state->token;

        if (!ast_next_token(state)) {
            return error_0201_missing_value(state->lexer->source,
                                            ast_token_span(state, &ffi_token),
                                            TK_ffi);
        }

        bool old_stop_before_ffi_name = state->stop_before_ffi_name;
        state->stop_before_ffi_name   = true;
        u32  library_node_index       = 0;
        bool parsed_library = ast_parse_expr(state, &library_node_index);
        state->stop_before_ffi_name = old_stop_before_ffi_name;
        if (!parsed_library) {
            return false;
        }

        if (state->token.kind != TK_Symbol) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_Symbol,
                state->token.kind);
        }
        u32 symbol_handle = state->token.value.symbol_handle;
        if (state->token_index == state->token.token_index &&
            !ast_next_token(state)) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_LParen,
                TK_EOF);
        }
        if (!ast_next_token(state)) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_LParen,
                TK_EOF);
        }

        if (state->token.kind != TK_LParen) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_LParen,
                state->token.kind);
        }
        if (state->token_index == state->token.token_index &&
            !ast_next_token(state)) {
            return false;
        }

        u32 signature_index = 0;
        if (!ast_parse_ffi_signature(state, &signature_index)) {
            return false;
        }

        u32 ffi_index = (u32)array_count(state->ffi_infos);
        array_push(state->ffi_infos,
                   (AstFfiInfo){.library_node_index = library_node_index,
                                .symbol_handle      = symbol_handle,
                                .signature_index    = signature_index});

        return ast_emit_node(state,
                             (AstNode){.kind        = AK_FfiDef,
                                       .token_index = ffi_token.token_index,
                                       .a           = ffi_index},
                             out_node);
    }

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

internal bool ast_parse_destructure(AstParseState* state, u32* out_node)
{
    ASSERT(state->token.kind == TK_LParen || state->token.kind == TK_LBrace,
           "Expected destructuring pattern");
    AstToken start_token = state->token;
    u32      pattern     = U32_MAX;
    if (!ast_parse_destructure_pattern(state, &pattern)) {
        return false;
    }

    if (state->token.kind != TK_Colon && state->token.kind != TK_Equal) {
        return error_0203_expected_token(state->lexer->source,
                                         ast_token_span(state, &state->token),
                                         TK_Equal,
                                         state->token.kind);
    }

    AstKind node_kind  = AK_DestructureBind;
    u32     type_index = U32_MAX;
    if (state->token.kind == TK_Equal) {
        node_kind = AK_DestructureAssign;
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected destructuring assignment value, but found end of "
                "file");
        }
        goto parse_value;
    }

    if (!ast_next_token(state)) {
        return error_0205_expected_declaration_or_expression(
            state->token.source,
            ast_token_span(state, &state->token),
            TK_EOF,
            "Expected destructuring initializer, but found end of file");
    }

    if (state->token.kind == TK_Colon) {
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected destructuring initializer after '::', but found end "
                "of file");
        }
    } else if (state->token.kind == TK_Equal) {
        node_kind = AK_DestructureVariable;
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected destructuring initializer after ':=', but found end "
                "of file");
        }
    } else {
        node_kind = AK_DestructureVariable;
        if (!ast_parse_type(state, &type_index)) {
            return false;
        }
        if (!ast_peek_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected ':' or '=' after destructuring type annotation, but "
                "found end of file");
        }
        if (state->token.kind != TK_Colon && state->token.kind != TK_Equal) {
            return error_0203_expected_token(
                state->lexer->source,
                ast_token_span(state, &state->token),
                TK_Equal,
                state->token.kind);
        }
        if (state->token.kind == TK_Colon) {
            node_kind = AK_DestructureBind;
        }
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected destructuring initializer after type annotation, "
                "but found end of file");
        }
        if (!ast_next_token(state)) {
            return error_0205_expected_declaration_or_expression(
                state->token.source,
                ast_token_span(state, &state->token),
                TK_EOF,
                "Expected destructuring initializer after type annotation, "
                "but found end of file");
        }
    }

parse_value:
    u32  value_node                 = 0;
    bool previous_boundary          = state->allow_statement_boundary;
    state->allow_statement_boundary = true;
    bool ok                         = ast_parse_expr(state, &value_node);
    state->allow_statement_boundary = previous_boundary;
    if (!ok) {
        return false;
    }

    if (type_index != U32_MAX) {
        AstNode annotated = {
            .kind        = AK_AnnotatedValue,
            .token_index = start_token.token_index,
            .a           = type_index,
            .b           = value_node,
        };
        if (!ast_emit_node(state, annotated, &value_node)) {
            return false;
        }
    }

    return ast_emit_node(state,
                         (AstNode){
                             .kind        = node_kind,
                             .token_index = start_token.token_index,
                             .a           = pattern,
                             .b           = value_node,
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

        if (!ast_parse_top_level_item(&state)) {
            goto error;
        }
    }

    return (Ast){
        .nodes               = state.nodes,
        .params              = state.params,
        .fn_signatures       = state.fn_signatures,
        .ffi_infos           = state.ffi_infos,
        .module_paths        = state.module_paths,
        .module_path_symbols = state.module_path_symbols,
        .call_args           = state.call_args,
        .tuple_items         = state.tuple_items,
        .calls               = state.calls,
        .slices              = state.slices,
        .plex_fields         = state.plex_fields,
        .plex_types          = state.plex_types,
        .enum_variants       = state.enum_variants,
        .enum_types          = state.enum_types,
        .plex_literal_fields = state.plex_literal_fields,
        .plex_literals       = state.plex_literals,
        .patterns            = state.patterns,
        .pattern_items       = state.pattern_items,
        .pattern_fields      = state.pattern_fields,
        .enum_patterns       = state.enum_patterns,
        .on_branches         = state.on_branches,
        .ons                 = state.ons,
        .top_ons             = state.top_ons,
        .for_items           = state.for_items,
        .fors                = state.fors,
    };

error:
    ast_done(&(Ast){.nodes               = state.nodes,
                    .params              = state.params,
                    .fn_signatures       = state.fn_signatures,
                    .ffi_infos           = state.ffi_infos,
                    .module_paths        = state.module_paths,
                    .module_path_symbols = state.module_path_symbols,
                    .call_args           = state.call_args,
                    .tuple_items         = state.tuple_items,
                    .calls               = state.calls,
                    .slices              = state.slices,
                    .plex_fields         = state.plex_fields,
                    .plex_types          = state.plex_types,
                    .enum_variants       = state.enum_variants,
                    .enum_types          = state.enum_types,
                    .plex_literal_fields = state.plex_literal_fields,
                    .plex_literals       = state.plex_literals,
                    .patterns            = state.patterns,
                    .pattern_items       = state.pattern_items,
                    .pattern_fields      = state.pattern_fields,
                    .enum_patterns       = state.enum_patterns,
                    .on_branches         = state.on_branches,
                    .ons                 = state.ons,
                    .top_ons             = state.top_ons,
                    .for_items           = state.for_items,
                    .fors                = state.fors});
    return (Ast){0};
}

//------------------------------------------------------------------------------
// Free the parser's AST node table.

void ast_done(Ast* ast)
{
    array_free(ast->nodes);
    array_free(ast->params);
    array_free(ast->fn_signatures);
    array_free(ast->ffi_infos);
    array_free(ast->module_paths);
    array_free(ast->module_path_symbols);
    array_free(ast->call_args);
    array_free(ast->tuple_items);
    array_free(ast->calls);
    array_free(ast->slices);
    array_free(ast->plex_fields);
    array_free(ast->plex_types);
    array_free(ast->enum_variants);
    array_free(ast->enum_types);
    array_free(ast->plex_literal_fields);
    array_free(ast->plex_literals);
    array_free(ast->patterns);
    array_free(ast->pattern_items);
    array_free(ast->pattern_fields);
    array_free(ast->enum_patterns);
    array_free(ast->on_branches);
    array_free(ast->ons);
    array_free(ast->top_ons);
    array_free(ast->for_items);
    array_free(ast->fors);
    *ast = (Ast){0};
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
