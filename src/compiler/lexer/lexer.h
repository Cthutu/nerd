//------------------------------------------------------------------------------
// Lexical Analysis module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/source.h>

//------------------------------------------------------------------------------

typedef enum { TK_Integer } TokenKind;

typedef struct {
    TokenKind kind : 8;
    u32       offset : 24;
} Token;

typedef struct {
    NerdSource source;
    Array(Token) tokens;
    Array(u64) integers;
} Lexer;

bool   lex(NerdSource source, Lexer* lexer);
void   lex_done(Lexer* lexer);
void   lex_dump(const Lexer* lexer);
Token* lex_find(const Lexer* lexer, usize offset, u32* token_end);
bool   lex_offset_to_line_col(NerdSource source,
                              usize      offset,
                              u32*       out_line,
                              u32*       out_col);
bool   lex_line_col_to_offset(NerdSource source,
                              u32        line,
                              u32        col,
                              usize*     out_offset);

string token_kind_to_string(TokenKind token);
