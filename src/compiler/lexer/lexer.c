//------------------------------------------------------------------------------
// Lexical Analysis API implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <compiler/lexer/lexer.h>

#define LEXER_ARRAY_INIT_CAPACITY 256

//------------------------------------------------------------------------------

bool lex_with_config(NerdSource source, const LexerConfig* config, Lexer* lexer)
{
    string source_code = source.source;

    *lexer             = (Lexer){0};
    lexer->source      = source;
    lexer->mode        = config ? config->mode : LEXER_MODE_NORMAL;

    if (source_code.count >= (1u << 24)) {
        return error_0102_file_too_large(source);
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
            usize start = i;
            i += 2;
            while (i < source_code.count && source_code.data[i] != '\n') {
                i++;
            }
            if (lexer->mode == LEXER_MODE_FORMAT) {
                if (lexer->comment_arena.data == NULL) {
                    arena_init(&lexer->comment_arena);
                }

                string comment_text =
                    string_from(source_code.data + start + 2, i - start - 2);
                u8* copied =
                    (u8*)arena_alloc(&lexer->comment_arena, comment_text.count);
                if (comment_text.count > 0) {
                    memcpy(copied, comment_text.data, comment_text.count);
                }

                array_push(lexer->comment_indices,
                           array_count(lexer->comments));
                array_push(lexer->comments,
                           (LexerComment){
                               .offset      = (u32)start,
                               .end_offset  = (u32)i,
                               .token_index = (u32)array_count(lexer->tokens),
                               .text = string_from(copied, comment_text.count),
                           });
            }
            continue;
        }

        // Check for `¬` and signify end of compilation.  In unicode, the
        // character is 0x00ac, but in UTF-8 it is the byte sequence:
        // 0xc2, 0xac.
        if (c == 0xc2 && i + 1 < source_code.count &&
            source_code.data[i + 1] == 0xac) {
            lexer->source.source = string_from(source_code.data, i);
            break;
        }

        //
        // Integers
        //

        if (c >= '0' && c <= '9') {
            usize start      = i;
            u64   total      = 0;
            u64   last_total = 0;

            while (i < source_code.count && source_code.data[i] >= '0' &&
                   source_code.data[i] <= '9') {
                last_total = total;
                total      = total * 10 + (source_code.data[i] - '0');
                if (total < last_total) {
                    ErrorSpan span = {.start = start, .end = i + 1};
                    return error_0101_integer_literal_too_large(source, span);
                }

                i++;
            }

            array_push(lexer->tokens,
                       (Token){.kind = TK_Integer, .offset = (u32)start});
            array_push(lexer->integers, total);

            //
            // Check to make sure we don't have an alpha character following
            //

            if (i < source_code.count &&
                ((source_code.data[i] >= 'a' && source_code.data[i] <= 'z') ||
                 (source_code.data[i] >= 'A' && source_code.data[i] <= 'Z'))) {
                return error_0103_invalid_number_literal(
                    source,
                    (ErrorSpan){.start = start, .end = i + 1},
                    source_code.data[i]);
            }
        }

        //
        // Symbols and keywords
        //

        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
            usize start = i;
            while (
                i < source_code.count &&
                ((source_code.data[i] >= 'a' && source_code.data[i] <= 'z') ||
                 (source_code.data[i] >= 'A' && source_code.data[i] <= 'Z') ||
                 (source_code.data[i] >= '0' && source_code.data[i] <= '9') ||
                 source_code.data[i] == '_')) {
                i++;
            }
            string str = string_from(source_code.data + start, i - start);

            //
            // Check for keyword
            //

            struct {
                cstr      name;
                u8        length;
                TokenKind kind;
            } keywords[]    = {{"fn", 2, TK_fn}, {NULL, 0, 0}};

            bool is_keyword = false;
            for (usize k = 0; keywords[k].name != NULL; k++) {
                if (str.count == keywords[k].length &&
                    memcmp(str.data, keywords[k].name, str.count) == 0) {
                    array_push(lexer->tokens,
                               (Token){.kind   = keywords[k].kind,
                                       .offset = (u32)start});
                    is_keyword = true;
                    break;
                }
            }
            if (is_keyword) {
                continue;
            }

            //
            // Handle symbol
            //

            InternAddResult added_result;
            u32             handle = lex_add_symbol(lexer, str, &added_result);
            switch (added_result) {
            case INTERN_ADD_IS_NEW:
            case INTERN_ADD_ALREADY_EXISTS:
                break;
            case INTERN_ADD_TOO_MANY_STRINGS:
                return error_0105_too_many_symbols(source);
            case INTERN_ADD_STRING_TOO_LONG:
                return error_0104_symbol_too_long(
                    source, (ErrorSpan){.start = start, .end = i});
            }

            array_push(lexer->symbol_handles, handle);
            array_push(lexer->tokens,
                       (Token){.kind = TK_Symbol, .offset = (u32)start});

        } else if (c == '=') {
            if (i + 1 < source_code.count && source_code.data[i + 1] == '>') {
                array_push(lexer->tokens,
                           (Token){.kind = TK_FatArrow, .offset = (u32)i});
                i += 2;
            } else {
                return error_0100_unexpected_character(source, i, (char)c);
            }
        } else {
#if COMPILER_CLANG || COMPILER_GCC
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Winitializer-overrides"
#endif
            static TokenKind token_lookup[128] = {
                // we'll override the ones we care about
                [0 ... 127] = TK_EOF, // Default to TK_EOF for all characters,

                ['+']       = TK_Plus,
                ['-']       = TK_Minus,
                ['*']       = TK_Star,
                ['/']       = TK_Slash,
                ['%']       = TK_Percent,
                ['(']       = TK_LParen,
                [')']       = TK_RParen,
                [':']       = TK_Colon,
            };
#if COMPILER_CLANG || COMPILER_GCC
#    pragma GCC diagnostic pop
#endif

            if (c >= 128 || token_lookup[c] == TK_EOF) {
                return error_0100_unexpected_character(source, i, (char)c);
            } else {
                array_push(lexer->tokens,
                           (Token){.kind = token_lookup[c], .offset = (u32)i});
            }
            i++;
        }
    }
    return true;
}

//------------------------------------------------------------------------------

bool lex(NerdSource source, Lexer* lexer)
{
    return lex_with_config(
        source, &(LexerConfig){.mode = LEXER_MODE_NORMAL}, lexer);
}

//------------------------------------------------------------------------------

void lex_done(Lexer* lexer)
{
    array_free(lexer->tokens);
    array_free(lexer->integers);
    array_free(lexer->symbol_handles);
    array_free(lexer->comments);
    array_free(lexer->comment_indices);
    if (lexer->comment_arena.data != NULL) {
        arena_done(&lexer->comment_arena);
    }
    if (lexer->symbols.intern_arena.data != NULL) {
        intern_done(&lexer->symbols);
    }
}

//------------------------------------------------------------------------------

usize lex_token_end_offset(const Lexer* lexer, const Token* token)
{
    switch (token->kind) {
    case TK_Integer:
        {
            usize index = token->offset;
            while (index < lexer->source.source.count &&
                   lexer->source.source.data[index] >= '0' &&
                   lexer->source.source.data[index] <= '9') {
                index++;
            }
            return index;
        }
    case TK_Plus:
    case TK_Minus:
    case TK_Star:
    case TK_Slash:
    case TK_Percent:
    case TK_LParen:
    case TK_RParen:
    case TK_Colon:
        return token->offset + 1;

    case TK_FatArrow:
        return token->offset + 2;

    case TK_Symbol:
    case TK_fn:
        {
            usize index = token->offset;
            while (index < lexer->source.source.count &&
                   ((lexer->source.source.data[index] >= 'a' &&
                     lexer->source.source.data[index] <= 'z') ||
                    (lexer->source.source.data[index] >= 'A' &&
                     lexer->source.source.data[index] <= 'Z') ||
                    (lexer->source.source.data[index] >= '0' &&
                     lexer->source.source.data[index] <= '9') ||
                    lexer->source.source.data[index] == '_')) {
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
        usize  end   = lex_token_end_offset(lexer, token);

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
// Line and column to/from offset conversions

bool lex_offset_to_line_col(NerdSource source,
                            usize      offset,
                            u32*       out_line,
                            u32*       out_col)
{
    if (offset > source.source.count) {
        return false;
    }

    u32 line = 0;
    u32 col  = 0;
    for (usize i = 0; i < offset; i++) {
        if (source.source.data[i] == '\n') {
            line++;
            col = 0;
        } else {
            col++;
        }
    }

    *out_line = line;
    *out_col  = col;
    return true;
}

bool lex_line_col_to_offset(NerdSource source,
                            u32        line,
                            u32        col,
                            usize*     out_offset)
{
    usize offset     = 0;
    bool  found_line = false;

    for (usize i = 0; i < source.source.count; i++) {
        if (source.source.data[i] == '\n') {
            if (line == 0) {
                // We found the end of the correct line, so the line/col pair is
                // invalid.
                break;
            }
            line--;
        } else if (line == 0) {
            if (col == 0) {
                found_line = true;
                break;
            }
            col--;
        }
        offset++;
    }

    if (found_line) {
        *out_offset = offset;
    }
    return found_line;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
