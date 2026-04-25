//------------------------------------------------------------------------------
// Lexical Analysis module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: intern

#pragma once

#include <compiler/compiler.h>
#include <intern/intern.h>

// When adding a new lexer token, make changes to:
// - The TokenKind enum (compiler/lexer/lexer.h)
// - The token_lookup table in lex() if single-char punctuation
//   (compiler/lexer/lexer.c)
// - The token_kind_to_string() function (compiler/lexer/lexer.c)
// - The lex_token_end_offset() function (compiler/lexer/dump.c)

typedef enum {
    TK_EOF, // Not used in lexer, but used in AST

    // Values
    TK_Integer,
    TK_Float,
    TK_String,
    TK_InterpolatedStringStart,
    TK_InterpolatedStringEnd,
    TK_Symbol,

    // Operators & punctuation
    TK_Plus,
    TK_PlusEqual,
    TK_Minus,
    TK_MinusEqual,
    TK_Star,
    TK_StarEqual,
    TK_Slash,
    TK_SlashEqual,
    TK_Percent,
    TK_PercentEqual,
    TK_LParen,
    TK_RParen,
    TK_LBracket,
    TK_RBracket,
    TK_Comma,
    TK_Semicolon,
    TK_LBrace,
    TK_RBrace,
    TK_Dot,
    TK_At,
    TK_Dollar,
    TK_Hash,
    TK_Ellipsis,
    TK_Range,
    TK_RangeInclusive,
    TK_Colon,
    TK_Equal,
    TK_EqualEqual,
    TK_Bang,
    TK_BangEqual,
    TK_Amp,
    TK_AmpEqual,
    TK_AmpAmp,
    TK_AmpAmpEqual,
    TK_Pipe,
    TK_PipeEqual,
    TK_PipePipe,
    TK_PipePipeEqual,
    TK_Caret,
    TK_CaretEqual,
    TK_Less,
    TK_LessEqual,
    TK_Greater,
    TK_GreaterEqual,
    TK_FatArrow,
    TK_ThinArrow,

    // Keywords
    TK_fn,
    TK_for,
    TK_on,
    TK_else,
    TK_break,
    TK_continue,
    TK_return,
    TK_plex,
    TK_union,
    TK_enum,
    TK_ffi,
    TK_mod,
    TK_use,
    TK_pub,
    TK_with,
    TK_as,
    TK_yes,
    TK_no,
} TokenKind;

typedef struct {
    TokenKind kind : 8;
    u32       offset : 24;
} Token;

typedef enum {
    LEXER_MODE_NORMAL,
    LEXER_MODE_FORMAT,
} LexerMode;

typedef struct {
    LexerMode mode;
} LexerConfig;

typedef struct {
    u32    offset;
    u32    end_offset;
    u32    token_index;
    string text;
} LexerComment;

typedef struct {
    NerdSource source;
    LexerMode  mode;
    Array(Token) tokens;
    Array(u64) integers;
    Array(f64) floats;
    Array(string) strings;
    Array(u32) symbol_handles;
    Arena string_arena;
    Arena comment_arena;
    Array(LexerComment) comments;
    Array(u32) comment_indices;
    Interner symbols;
} Lexer;

bool   lex(NerdSource source, Lexer* lexer);
bool   lex_with_config(NerdSource         source,
                       const LexerConfig* config,
                       Lexer*             lexer);
void   lex_done(Lexer* lexer);
void   lex_dump(const Lexer* lexer);
u32    lex_add_symbol(Lexer* lexer, string str, InternAddResult* out_result);
string lex_symbol(const Lexer* lexer, u32 handle);
usize  lex_token_end_offset(const Lexer* lexer, const Token* token);
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
