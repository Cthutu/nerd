//------------------------------------------------------------------------------
// String Interner
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/lexer/lexer.h>

//------------------------------------------------------------------------------

internal inline u32 lexer_symbol_handle(Lexer* lexer, string str)
{
    InternedString* info = intern_get_info(str);
    ASSERT(info != NULL, "Expected interned string");
    return arena_offset(&lexer->symbols.intern_arena, info);
}

u32 lex_add_symbol(Lexer* lexer, string str, InternAddResult* out_result)
{
    if (lexer->symbols.intern_arena.data == NULL) {
        intern_init(&lexer->symbols);
    }

    string added = intern_add(&lexer->symbols, str, out_result);
    if (added.data == NULL) {
        return 0;
    }
    return lexer_symbol_handle(lexer, added);
}

string lex_symbol(const Lexer* lexer, u32 handle)
{
    InternedString* info =
        (InternedString*)(lexer->symbols.intern_arena.data + handle);
    return (string){.data = info->str, .count = info->len};
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
