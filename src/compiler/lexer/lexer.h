//------------------------------------------------------------------------------
// Lexical Analysis module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------

typedef enum { TK_Integer } TokenKind;

typedef struct {
    TokenKind kind : 8;
    u32       offset : 24;
} Token;

typedef struct {
    string source_code;
    Array(Token) tokens;
    Array(u64) integers;
} Lexer;

bool   lex(string source_code, Lexer* lexer);
void   lex_done(Lexer* lexer);
void   lex_dump(const Lexer* lexer);
Token* lex_find(const Lexer* lexer, usize offset, u32* token_end);

string token_kind_to_string(TokenKind token);
