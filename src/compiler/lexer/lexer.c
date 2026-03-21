//------------------------------------------------------------------------------
// Lexical Analysis API implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/lexer/lexer.h>

#define LEXER_ARRAY_INIT_CAPACITY 256

//------------------------------------------------------------------------------

bool lex(string source_code, Lexer* lexer)
{
    *lexer             = (Lexer){0};
    lexer->source_code = source_code;

    if (source_code.count >= (1u << 24)) {
        return error_0102_file_too_many_tokens(source_code);
    }

    // array_requires_capacity(lexer.tokens, LEXER_ARRAY_INIT_CAPACITY);
    // array_requires_capacity(lexer.integers, LEXER_ARRAY_INIT_CAPACITY);

    for (usize i = 0; i < source_code.count;) {
        u8 c = source_code.data[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            i++;
            continue;
        }

        if (c == '-' && i + 1 < source_code.count &&
            source_code.data[i + 1] == '-') {
            // Skip comment until end of line
            i += 2;
            while (i < source_code.count && source_code.data[i] != '\n') {
                i++;
            }
            continue;
        }

        if (c >= '0' && c <= '9') {
            usize start      = i;
            u64   total      = 0;
            u64   last_total = 0;

            while (i < source_code.count && source_code.data[i] >= '0' &&
                   source_code.data[i] <= '9') {
                last_total = total;
                total      = total * 10 + (source_code.data[i] - '0');
                if (total < last_total) {
                    return error_0101_integer_literal_too_large(source_code,
                                                                start);
                }

                i++;
            }

            array_push(lexer->tokens,
                       (Token){.kind = TK_Integer, .offset = (u32)start});
            array_push(lexer->integers, total);
        } else {
            return error_0100_unexpected_character(source_code, i, (char)c);
        }
    }

    return true;
}

//------------------------------------------------------------------------------

void lex_done(Lexer* lexer)
{
    array_free(lexer->tokens);
    array_free(lexer->integers);
}

//------------------------------------------------------------------------------

internal usize token_end_offset(const Lexer* lexer, Token* token)
{
    switch (token->kind) {
    case TK_Integer:
        {
            usize index = token->offset;
            while (index < lexer->source_code.count &&
                   lexer->source_code.data[index] >= '0' &&
                   lexer->source_code.data[index] <= '9') {
                index++;
            }
            return index;
        }
    default:
        error_ice("Unknown token kind: %d", token->kind);
        return token->offset + 1; // Fallback to prevent infinite loops
    }
}

//------------------------------------------------------------------------------

Token* lex_find(const Lexer* lexer, usize offset, u32* token_end)
{
    // Binary search for the token that spans the offset
    isize left  = 0;
    isize right = (isize)array_count(lexer->tokens) - 1;

    while (left <= right) {
        isize  mid   = left + (right - left) / 2;
        Token* token = &lexer->tokens[mid];
        usize  start = token->offset;
        usize  end   = token_end_offset(lexer, token);

        if (offset < start) {
            right = mid - 1;
        } else if (offset >= end) {
            left = mid + 1;
        } else {
            *token_end = (u32)end;
            return token; // Found the token that spans the offset
        }
    }

    return NULL; // No token found at the given offset
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
