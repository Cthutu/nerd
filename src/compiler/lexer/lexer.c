//------------------------------------------------------------------------------
// Lexical Analysis API implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/lexer/lexer.h>

#define LEXER_ARRAY_INIT_CAPACITY 256

//------------------------------------------------------------------------------

internal void lex_error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    eprn(ANSI_BOLD_RED "Lexing error: " ANSI_RESET);
    eprv(format, args);
    eprn("");
    va_end(args);
    exit(1);
}

//------------------------------------------------------------------------------

Lexer lex(string source_code)
{
    Lexer lexer       = {0};
    lexer.source_code = source_code;

    // array_requires_capacity(lexer.tokens, LEXER_ARRAY_INIT_CAPACITY);
    // array_requires_capacity(lexer.integers, LEXER_ARRAY_INIT_CAPACITY);

    for (usize i = 0; i < source_code.count; i++) {
        u8 c = source_code.data[i];
        if (c >= '0' && c <= '9') {
            usize start = i;
            u64   total = 0;

            while (i < source_code.count && source_code.data[i] >= '0' &&
                   source_code.data[i] <= '9') {
                total = total * 10 + (source_code.data[i] - '0');
                i++;
            }

            array_push(lexer.tokens,
                       (Token){.kind = TK_Number, .offset = (u32)start});
            array_push(lexer.integers, total);
        } else {
            lex_error("Unexpected character '%c' at offset %zu", c, i);
        }
    }

    // Lexing logic goes here. For now, we'll just return an empty lexer.
    return lexer;
}

//------------------------------------------------------------------------------

void lex_done(Lexer* lexer)
{
    array_free(lexer->tokens);
    array_free(lexer->integers);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
