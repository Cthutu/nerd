//------------------------------------------------------------------------------
// Lexical Analysis module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------

typedef enum { TK_Number } TokenKind;

typedef struct {
    TokenKind kind : 8;
    u32       offset : 24;
} Token;

typedef struct {
    string source_code;
    Array(Token) tokens;
    Array(u64) integers;
} Lexer;

Lexer lex(string source_code);
void  lex_done(Lexer* lexer);
void  lex_dump(Lexer* lexer);
