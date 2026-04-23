//------------------------------------------------------------------------------
// Lexical Analysis API implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
#include <errno.h>
#include <compiler/lexer/lexer.h>

#define LEXER_ARRAY_INIT_CAPACITY 256

internal bool
lexer_emit_string_token(Lexer* lexer, usize offset, const u8* text, usize count)
{
    if (lexer->string_arena.data == NULL) {
        arena_init(&lexer->string_arena);
    }

    u8* buffer = (u8*)arena_alloc(&lexer->string_arena, count);
    if (count > 0) {
        memcpy(buffer, text, count);
    }

    array_push(lexer->tokens,
               (Token){.kind = TK_String, .offset = (u32)offset});
    array_push(lexer->strings, string_from(buffer, count));
    return true;
}

internal bool lexer_lex_one_token(NerdSource source,
                                  string     source_code,
                                  usize*     io_index,
                                  Lexer*     lexer,
                                  bool       allow_interpolation_start);

internal bool lexer_lex_string_literal(NerdSource source,
                                       string     source_code,
                                       usize*     io_index,
                                       Lexer*     lexer,
                                       TokenKind  token_kind)
{
    usize start = *io_index;
    usize i     = start + 1;

    if (lexer->string_arena.data == NULL) {
        arena_init(&lexer->string_arena);
    }

    usize capacity = source_code.count - start;
    u8*   buffer   = (u8*)arena_alloc(&lexer->string_arena, capacity);
    usize length   = 0;
    bool  closed   = false;

    while (i < source_code.count) {
        u8 ch = source_code.data[i++];
        if (ch == '"') {
            closed = true;
            break;
        }

        if (ch == '\\' && i < source_code.count) {
            u8 escaped = source_code.data[i++];
            switch (escaped) {
            case '"':
                ch = '"';
                break;
            case '\\':
                ch = '\\';
                break;
            case 'n':
                ch = '\n';
                break;
            case 'r':
                ch = '\r';
                break;
            case 't':
                ch = '\t';
                break;
            default:
                ch = escaped;
                break;
            }
        }

        buffer[length++] = ch;
    }

    if (!closed) {
        return error_0106_unterminated_string_literal(
            source, (ErrorSpan){.start = start, .end = i});
    }

    array_push(lexer->tokens,
               (Token){.kind = token_kind, .offset = (u32)start});
    array_push(lexer->strings, string_from(buffer, length));
    *io_index = i;
    return true;
}

internal bool lexer_lex_interpolated_text(NerdSource source,
                                          string     source_code,
                                          usize      interpolation_start,
                                          usize*     io_index,
                                          Lexer*     lexer);

internal bool lexer_lex_interpolated_expr(NerdSource source,
                                          string     source_code,
                                          usize      interpolation_start,
                                          usize*     io_index,
                                          Lexer*     lexer,
                                          u32        brace_depth)
{
    usize i     = *io_index;
    u32   depth = brace_depth;

    while (i < source_code.count) {
        if (source_code.data[i] == '{') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_LBrace, .offset = (u32)i});
            ++depth;
            ++i;
            continue;
        }

        if (source_code.data[i] == '}') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_RBrace, .offset = (u32)i});
            --depth;
            ++i;
            if (depth == 0) {
                *io_index = i;
                return lexer_lex_interpolated_text(
                    source, source_code, interpolation_start, io_index, lexer);
            }
            continue;
        }

        *io_index = i;
        if (!lexer_lex_one_token(source, source_code, io_index, lexer, true)) {
            return false;
        }
        i = *io_index;
    }

    return error_0106_unterminated_string_literal(
        source,
        (ErrorSpan){.start = interpolation_start, .end = source_code.count});
}

internal bool lexer_lex_interpolated_text(NerdSource source,
                                          string     source_code,
                                          usize      interpolation_start,
                                          usize*     io_index,
                                          Lexer*     lexer)
{
    usize chunk_start = *io_index;
    usize i           = *io_index;

    if (lexer->string_arena.data == NULL) {
        arena_init(&lexer->string_arena);
    }

    usize capacity = source_code.count - chunk_start;
    u8*   buffer   = (u8*)arena_alloc(&lexer->string_arena, capacity);
    usize length   = 0;

    while (i < source_code.count) {
        u8 ch = source_code.data[i++];

        if (ch == '"') {
            if (length > 0) {
                lexer_emit_string_token(lexer, chunk_start, buffer, length);
            }
            array_push(lexer->tokens,
                       (Token){.kind   = TK_InterpolatedStringEnd,
                               .offset = (u32)(i - 1)});
            *io_index = i;
            return true;
        }

        if (ch == '{') {
            if (length > 0) {
                lexer_emit_string_token(lexer, chunk_start, buffer, length);
            }
            array_push(lexer->tokens,
                       (Token){.kind = TK_LBrace, .offset = (u32)(i - 1)});
            *io_index = i;
            return lexer_lex_interpolated_expr(
                source, source_code, interpolation_start, io_index, lexer, 1);
        }

        if (ch == '\\' && i < source_code.count) {
            u8 escaped = source_code.data[i++];
            switch (escaped) {
            case '"':
                ch = '"';
                break;
            case '\\':
                ch = '\\';
                break;
            case 'n':
                ch = '\n';
                break;
            case 'r':
                ch = '\r';
                break;
            case 't':
                ch = '\t';
                break;
            default:
                ch = escaped;
                break;
            }
        }

        buffer[length++] = ch;
    }

    return error_0106_unterminated_string_literal(
        source,
        (ErrorSpan){.start = interpolation_start, .end = source_code.count});
}

internal bool lexer_lex_one_token(NerdSource source,
                                  string     source_code,
                                  usize*     io_index,
                                  Lexer*     lexer,
                                  bool       allow_interpolation_start)
{
    usize i = *io_index;
    u8    c = source_code.data[i];

    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        *io_index = i + 1;
        return true;
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

            array_push(lexer->comment_indices, array_count(lexer->comments));
            array_push(lexer->comments,
                       (LexerComment){
                           .offset      = (u32)start,
                           .end_offset  = (u32)i,
                           .token_index = (u32)array_count(lexer->tokens),
                           .text = string_from(copied, comment_text.count),
                       });
        }
        *io_index = i;
        return true;
    }

    if (c == 0xc2 && i + 1 < source_code.count &&
        source_code.data[i + 1] == 0xac) {
        lexer->source.source = string_from(source_code.data, i);
        *io_index            = source_code.count;
        return true;
    }

    if (allow_interpolation_start && c == '$' && i + 1 < source_code.count &&
        source_code.data[i + 1] == '"') {
        array_push(
            lexer->tokens,
            (Token){.kind = TK_InterpolatedStringStart, .offset = (u32)i});
        i += 2;
        *io_index = i;
        return lexer_lex_interpolated_text(
            source, source_code, i - 2, io_index, lexer);
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
                ErrorSpan span = {.start = start, .end = i + 1};
                return error_0101_integer_literal_too_large(source, span);
            }

            i++;
        }

        bool is_float = i + 1 < source_code.count && source_code.data[i] == '.' &&
                        source_code.data[i + 1] >= '0' &&
                        source_code.data[i + 1] <= '9';
        if (is_float) {
            i += 2;
            while (i < source_code.count && source_code.data[i] >= '0' &&
                   source_code.data[i] <= '9') {
                i++;
            }

            usize literal_len = i - start;
            if (lexer->string_arena.data == NULL) {
                arena_init(&lexer->string_arena);
            }

            char* buffer =
                (char*)arena_alloc(&lexer->string_arena, literal_len + 1);
            memcpy(buffer, source_code.data + start, literal_len);
            buffer[literal_len] = '\0';

            errno     = 0;
            f64 value = strtod(buffer, NULL);
            if (errno == ERANGE) {
                return error_0103_invalid_number_literal(
                    source,
                    (ErrorSpan){.start = start, .end = i},
                    buffer[literal_len - 1]);
            }

            array_push(lexer->tokens,
                       (Token){.kind = TK_Float, .offset = (u32)start});
            array_push(lexer->floats, value);
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Integer, .offset = (u32)start});
            array_push(lexer->integers, total);
        }

        if (i < source_code.count &&
            ((source_code.data[i] >= 'a' && source_code.data[i] <= 'z') ||
             (source_code.data[i] >= 'A' && source_code.data[i] <= 'Z'))) {
            return error_0103_invalid_number_literal(
                source,
                (ErrorSpan){.start = start, .end = i + 1},
                source_code.data[i]);
        }

        *io_index = i;
        return true;
    }

    if (c == '"') {
        return lexer_lex_string_literal(
            source, source_code, io_index, lexer, TK_String);
    }

    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        usize start = i;
        while (i < source_code.count &&
               ((source_code.data[i] >= 'a' && source_code.data[i] <= 'z') ||
                (source_code.data[i] >= 'A' && source_code.data[i] <= 'Z') ||
                (source_code.data[i] >= '0' && source_code.data[i] <= '9') ||
                source_code.data[i] == '_')) {
            i++;
        }
        string str = string_from(source_code.data + start, i - start);

        struct {
            cstr      name;
            u8        length;
            TokenKind kind;
        } keywords[] = {
            {"fn", 2, TK_fn},
            {"on", 2, TK_on},
            {"else", 4, TK_else},
            {"return", 6, TK_return},
            {"true", 4, TK_true},
            {"false", 5, TK_false},
            {NULL, 0, 0},
        };

        bool is_keyword = false;
        for (usize k = 0; keywords[k].name != NULL; k++) {
            if (str.count == keywords[k].length &&
                memcmp(str.data, keywords[k].name, str.count) == 0) {
                array_push(
                    lexer->tokens,
                    (Token){.kind = keywords[k].kind, .offset = (u32)start});
                is_keyword = true;
                break;
            }
        }
        if (!is_keyword) {
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
        }

        *io_index = i;
        return true;
    }

    if (c == '=') {
        if (i + 1 < source_code.count && source_code.data[i + 1] == '=') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_EqualEqual, .offset = (u32)i});
            *io_index = i + 2;
        } else if (i + 1 < source_code.count && source_code.data[i + 1] == '>') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_FatArrow, .offset = (u32)i});
            *io_index = i + 2;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Equal, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

    if (c == '!') {
        if (i + 1 < source_code.count && source_code.data[i + 1] == '=') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_BangEqual, .offset = (u32)i});
            *io_index = i + 2;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Bang, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

    if (c == '-') {
        if (i + 1 < source_code.count && source_code.data[i + 1] == '>') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_ThinArrow, .offset = (u32)i});
            *io_index = i + 2;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Minus, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

    if (c == '.') {
        if (i + 2 < source_code.count && source_code.data[i + 1] == '.' &&
            source_code.data[i + 2] == '<') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_RangeExclusive, .offset = (u32)i});
            *io_index = i + 3;
        } else if (i + 2 < source_code.count &&
                   source_code.data[i + 1] == '.' &&
                   source_code.data[i + 2] == '=') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_RangeInclusive, .offset = (u32)i});
            *io_index = i + 3;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Dot, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

    if (c == '&') {
        if (i + 1 < source_code.count && source_code.data[i + 1] == '&') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_AmpAmp, .offset = (u32)i});
            *io_index = i + 2;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Amp, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

    if (c == '|') {
        if (i + 1 < source_code.count && source_code.data[i + 1] == '|') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_PipePipe, .offset = (u32)i});
            *io_index = i + 2;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Pipe, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

    if (c == '<') {
        if (i + 1 < source_code.count && source_code.data[i + 1] == '=') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_LessEqual, .offset = (u32)i});
            *io_index = i + 2;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Less, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

    if (c == '>') {
        if (i + 1 < source_code.count && source_code.data[i + 1] == '=') {
            array_push(lexer->tokens,
                       (Token){.kind = TK_GreaterEqual, .offset = (u32)i});
            *io_index = i + 2;
        } else {
            array_push(lexer->tokens,
                       (Token){.kind = TK_Greater, .offset = (u32)i});
            *io_index = i + 1;
        }
        return true;
    }

#if COMPILER_CLANG || COMPILER_GCC
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Winitializer-overrides"
#endif
    static TokenKind token_lookup[128] = {
        [0 ... 127] = TK_EOF,
        ['+']       = TK_Plus,
        ['-']       = TK_Minus,
        ['*']       = TK_Star,
        ['/']       = TK_Slash,
        ['%']       = TK_Percent,
        ['(']       = TK_LParen,
        [')']       = TK_RParen,
        [',']       = TK_Comma,
        ['{']       = TK_LBrace,
        ['}']       = TK_RBrace,
        ['.']       = TK_Dot,
        [':']       = TK_Colon,
        ['=']       = TK_Equal,
        ['!']       = TK_Bang,
        ['&']       = TK_Amp,
        ['|']       = TK_Pipe,
        ['^']       = TK_Caret,
        ['<']       = TK_Less,
        ['>']       = TK_Greater,
    };
#if COMPILER_CLANG || COMPILER_GCC
#    pragma GCC diagnostic pop
#endif

    if (c >= 128 || token_lookup[c] == TK_EOF) {
        return error_0100_unexpected_character(source, i, (char)c);
    }

    array_push(lexer->tokens,
               (Token){.kind = token_lookup[c], .offset = (u32)i});
    *io_index = i + 1;
    return true;
}

bool lex_with_config(NerdSource source, const LexerConfig* config, Lexer* lexer)
{
    string source_code = source.source;

    *lexer             = (Lexer){0};
    lexer->source      = source;
    lexer->mode        = config ? config->mode : LEXER_MODE_NORMAL;

    if (source_code.count >= (1u << 24)) {
        return error_0102_file_too_large(source);
    }

    for (usize i = 0; i < source_code.count;) {
        if (!lexer_lex_one_token(source, source_code, &i, lexer, true)) {
            return false;
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
    array_free(lexer->floats);
    array_free(lexer->strings);
    array_free(lexer->symbol_handles);
    array_free(lexer->comments);
    array_free(lexer->comment_indices);
    if (lexer->string_arena.data != NULL) {
        arena_done(&lexer->string_arena);
    }
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
    case TK_Float:
        {
            usize index = token->offset;
            while (index < lexer->source.source.count &&
                   lexer->source.source.data[index] >= '0' &&
                   lexer->source.source.data[index] <= '9') {
                index++;
            }
            if (token->kind == TK_Float && index < lexer->source.source.count &&
                lexer->source.source.data[index] == '.') {
                index++;
                while (index < lexer->source.source.count &&
                       lexer->source.source.data[index] >= '0' &&
                       lexer->source.source.data[index] <= '9') {
                    index++;
                }
            }
            return index;
        }
    case TK_String:
        {
            usize index = token->offset + 1;
            while (index < lexer->source.source.count) {
                if (lexer->source.source.data[index] == '\\') {
                    index += 2;
                    continue;
                }
                if (lexer->source.source.data[index] == '"') {
                    return index + 1;
                }
                if (lexer->source.source.data[index] == '{') {
                    return index;
                }
                index++;
            }
            return index;
        }
    case TK_InterpolatedStringStart:
        return token->offset + 2;
    case TK_InterpolatedStringEnd:
        return token->offset + 1;
    case TK_Symbol:
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
    case TK_FatArrow:
    case TK_ThinArrow:
    case TK_EqualEqual:
    case TK_BangEqual:
    case TK_AmpAmp:
    case TK_PipePipe:
    case TK_LessEqual:
    case TK_GreaterEqual:
        return token->offset + 2;
    case TK_RangeExclusive:
    case TK_RangeInclusive:
        return token->offset + 3;
    case TK_fn:
    case TK_on:
        return token->offset + 2;
    case TK_else:
    case TK_true:
        return token->offset + 4;
    case TK_false:
        return token->offset + 5;
    case TK_return:
        return token->offset + 6;
    default:
        return token->offset + 1;
    }
}

//------------------------------------------------------------------------------

Token* lex_find(const Lexer* lexer, usize offset, u32* token_end)
{
    for (u32 i = 0; i < array_count(lexer->tokens); ++i) {
        Token* token = &lexer->tokens[i];
        usize  end   = lex_token_end_offset(lexer, token);
        if (offset >= token->offset && offset < end) {
            if (token_end) {
                *token_end = (u32)end;
            }
            return token;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------

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
    for (usize i = 0; i < offset; ++i) {
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
    u32 current_line = 0;
    u32 current_col  = 0;
    for (usize i = 0; i < source.source.count; ++i) {
        if (current_line == line && current_col == col) {
            *out_offset = i;
            return true;
        }

        if (source.source.data[i] == '\n') {
            current_line++;
            current_col = 0;
        } else {
            current_col++;
        }
    }

    if (current_line == line && current_col == col) {
        *out_offset = source.source.count;
        return true;
    }

    return false;
}
