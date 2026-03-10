//------------------------------------------------------------------------------
// Object System module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <object/object.h>

#include <ctype.h>

//------------------------------------------------------------------------------

typedef struct {
    Arena*    arena;
    const u8* cur;
    const u8* end;
    const u8* start;
} JsonParser;

internal JsonValue* json_parser_value(JsonParser* parser, JsonParseResult* result);

internal u64 json_hash_string(string value)
{
    u64 hash = 14695981039346656037ull;
    for (usize i = 0; i < value.count; i++) {
        hash ^= value.data[i];
        hash *= 1099511628211ull;
    }
    return hash;
}

internal bool json_string_eq(string a, string b)
{
    return a.count == b.count && (a.count == 0 || memcmp(a.data, b.data, a.count) == 0);
}

internal void json_set_error(JsonParseResult* result,
                             const JsonParser* parser,
                             cstr              message)
{
    if (!result || !result->ok) {
        return;
    }
    result->ok           = false;
    result->error_offset = (usize)(parser->cur - parser->start);
    result->error_message = message;
}

internal void json_skip_ws(JsonParser* parser)
{
    while (parser->cur < parser->end) {
        u8 c = *parser->cur;
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            parser->cur++;
            continue;
        }
        break;
    }
}

internal bool json_match_literal(JsonParser* parser, cstr literal)
{
    usize len = strlen(literal);
    if ((usize)(parser->end - parser->cur) < len) {
        return false;
    }
    if (memcmp(parser->cur, literal, len) != 0) {
        return false;
    }
    parser->cur += len;
    return true;
}

internal JsonValue* json_alloc_value(Arena* arena, JsonKind kind)
{
    JsonValue* value = (JsonValue*)arena_alloc_align(arena, sizeof(JsonValue), alignof(JsonValue));
    memset(value, 0, sizeof(*value));
    value->kind = kind;
    return value;
}

internal u32 json_hex4_to_u32(const u8* data, bool* ok)
{
    u32 out = 0;
    for (usize i = 0; i < 4; i++) {
        u8  c     = data[i];
        u32 digit = 0;
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        } else {
            *ok = false;
            return 0;
        }
        out = (out << 4) | digit;
    }
    *ok = true;
    return out;
}

internal void json_append_utf8(ArenaSession* session, u32 cp)
{
    if (cp <= 0x7F) {
        u8* out = (u8*)arena_session_alloc(session, 1);
        out[0]  = (u8)cp;
    } else if (cp <= 0x7FF) {
        u8* out = (u8*)arena_session_alloc(session, 2);
        out[0]  = (u8)(0xC0 | (cp >> 6));
        out[1]  = (u8)(0x80 | (cp & 0x3F));
    } else {
        u8* out = (u8*)arena_session_alloc(session, 3);
        out[0]  = (u8)(0xE0 | (cp >> 12));
        out[1]  = (u8)(0x80 | ((cp >> 6) & 0x3F));
        out[2]  = (u8)(0x80 | (cp & 0x3F));
    }
}

internal bool json_parser_string_raw(JsonParser*       parser,
                                     JsonParseResult*  result,
                                     string*           out_string)
{
    if (parser->cur >= parser->end || *parser->cur != '"') {
        json_set_error(result, parser, "Expected string");
        return false;
    }
    parser->cur++;

    ArenaSession session = {0};
    arena_session_init(&session, parser->arena, 1, sizeof(u8));

    while (parser->cur < parser->end) {
        u8 c = *parser->cur++;
        if (c == '"') {
            out_string->data  = (u8*)arena_session_address(&session);
            out_string->count = arena_session_count(&session);
            return true;
        }

        if (c < 0x20) {
            json_set_error(result, parser, "Control character in string");
            return false;
        }

        if (c != '\\') {
            u8* out = (u8*)arena_session_alloc(&session, 1);
            out[0]  = c;
            continue;
        }

        if (parser->cur >= parser->end) {
            json_set_error(result, parser, "Unexpected end of string escape");
            return false;
        }

        u8 esc = *parser->cur++;
        switch (esc) {
        case '"':
        case '\\':
        case '/': {
            u8* out = (u8*)arena_session_alloc(&session, 1);
            out[0]  = esc;
        } break;
        case 'b': {
            u8* out = (u8*)arena_session_alloc(&session, 1);
            out[0]  = '\b';
        } break;
        case 'f': {
            u8* out = (u8*)arena_session_alloc(&session, 1);
            out[0]  = '\f';
        } break;
        case 'n': {
            u8* out = (u8*)arena_session_alloc(&session, 1);
            out[0]  = '\n';
        } break;
        case 'r': {
            u8* out = (u8*)arena_session_alloc(&session, 1);
            out[0]  = '\r';
        } break;
        case 't': {
            u8* out = (u8*)arena_session_alloc(&session, 1);
            out[0]  = '\t';
        } break;
        case 'u': {
            if ((usize)(parser->end - parser->cur) < 4) {
                json_set_error(result, parser, "Invalid unicode escape");
                return false;
            }

            bool ok = false;
            u32  cp = json_hex4_to_u32(parser->cur, &ok);
            if (!ok) {
                json_set_error(result, parser, "Invalid unicode escape");
                return false;
            }
            parser->cur += 4;
            json_append_utf8(&session, cp);
        } break;
        default:
            json_set_error(result, parser, "Invalid string escape");
            return false;
        }
    }

    json_set_error(result, parser, "Unterminated string");
    return false;
}

internal JsonValue* json_parser_number(JsonParser* parser, JsonParseResult* result)
{
    const u8* start = parser->cur;

    if (parser->cur < parser->end && *parser->cur == '-') {
        parser->cur++;
    }

    if (parser->cur >= parser->end) {
        json_set_error(result, parser, "Invalid number");
        return NULL;
    }

    if (*parser->cur == '0') {
        parser->cur++;
    } else {
        if (!isdigit(*parser->cur)) {
            json_set_error(result, parser, "Invalid number");
            return NULL;
        }
        while (parser->cur < parser->end && isdigit(*parser->cur)) {
            parser->cur++;
        }
    }

    if (parser->cur < parser->end && *parser->cur == '.') {
        parser->cur++;
        if (parser->cur >= parser->end || !isdigit(*parser->cur)) {
            json_set_error(result, parser, "Invalid number fraction");
            return NULL;
        }
        while (parser->cur < parser->end && isdigit(*parser->cur)) {
            parser->cur++;
        }
    }

    if (parser->cur < parser->end && (*parser->cur == 'e' || *parser->cur == 'E')) {
        parser->cur++;
        if (parser->cur < parser->end && (*parser->cur == '+' || *parser->cur == '-')) {
            parser->cur++;
        }
        if (parser->cur >= parser->end || !isdigit(*parser->cur)) {
            json_set_error(result, parser, "Invalid exponent");
            return NULL;
        }
        while (parser->cur < parser->end && isdigit(*parser->cur)) {
            parser->cur++;
        }
    }

    usize len = (usize)(parser->cur - start);
    char* tmp = (char*)arena_alloc(parser->arena, len + 1);
    memcpy(tmp, start, len);
    tmp[len] = '\0';

    JsonValue* value = json_alloc_value(parser->arena, JSON_NUMBER);
    value->number    = strtod(tmp, NULL);
    return value;
}

internal JsonValue* json_parser_array(JsonParser* parser, JsonParseResult* result)
{
    ASSERT(parser->cur < parser->end && *parser->cur == '[', "Expected array start");
    parser->cur++;

    JsonValue* value = json_new_array(parser->arena);
    json_skip_ws(parser);

    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return value;
    }

    for (;;) {
        json_skip_ws(parser);
        JsonValue* item = json_parser_value(parser, result);
        if (!item) {
            return NULL;
        }
        array_push(value->array.values, item);

        json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            json_set_error(result, parser, "Unterminated array");
            return NULL;
        }

        if (*parser->cur == ']') {
            parser->cur++;
            break;
        }
        if (*parser->cur != ',') {
            json_set_error(result, parser, "Expected ',' or ']'");
            return NULL;
        }
        parser->cur++;
    }

    return value;
}

internal JsonValue* json_parser_object(JsonParser* parser, JsonParseResult* result)
{
    ASSERT(parser->cur < parser->end && *parser->cur == '{', "Expected object start");
    parser->cur++;

    JsonValue* value = json_new_object(parser->arena);
    json_skip_ws(parser);

    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return value;
    }

    for (;;) {
        json_skip_ws(parser);

        string key = {0};
        if (!json_parser_string_raw(parser, result, &key)) {
            return NULL;
        }

        json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            json_set_error(result, parser, "Expected ':'");
            return NULL;
        }
        parser->cur++;

        json_skip_ws(parser);
        JsonValue* item = json_parser_value(parser, result);
        if (!item) {
            return NULL;
        }

        array_push(value->object.entries,
                   (JsonObjectEntry){
                       .key = key,
                       .key_hash = json_hash_string(key),
                       .value = item,
                   });

        json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            json_set_error(result, parser, "Unterminated object");
            return NULL;
        }

        if (*parser->cur == '}') {
            parser->cur++;
            break;
        }
        if (*parser->cur != ',') {
            json_set_error(result, parser, "Expected ',' or '}'");
            return NULL;
        }
        parser->cur++;
    }

    return value;
}

internal JsonValue* json_parser_value(JsonParser* parser, JsonParseResult* result)
{
    json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        json_set_error(result, parser, "Unexpected end of input");
        return NULL;
    }

    u8 c = *parser->cur;
    switch (c) {
    case '{':
        return json_parser_object(parser, result);
    case '[':
        return json_parser_array(parser, result);
    case '"': {
        string str = {0};
        if (!json_parser_string_raw(parser, result, &str)) {
            return NULL;
        }
        JsonValue* value = json_alloc_value(parser->arena, JSON_STRING);
        value->string    = str;
        return value;
    }
    case 't':
        if (json_match_literal(parser, "true")) {
            return json_new_bool(parser->arena, true);
        }
        break;
    case 'f':
        if (json_match_literal(parser, "false")) {
            return json_new_bool(parser->arena, false);
        }
        break;
    case 'n':
        if (json_match_literal(parser, "null")) {
            return json_new_null(parser->arena);
        }
        break;
    default:
        if (c == '-' || isdigit(c)) {
            return json_parser_number(parser, result);
        }
        break;
    }

    json_set_error(result, parser, "Invalid JSON value");
    return NULL;
}

internal string json_copy_string(Arena* arena, string value)
{
    if (value.count == 0) {
        return (string){0};
    }

    u8* copy = (u8*)arena_alloc(arena, value.count);
    memcpy(copy, value.data, value.count);
    return (string){
        .data = copy,
        .count = value.count,
    };
}

JsonValue* json_new_null(Arena* arena)
{
    return json_alloc_value(arena, JSON_NULL);
}

JsonValue* json_new_bool(Arena* arena, bool value)
{
    JsonValue* v = json_alloc_value(arena, JSON_BOOL);
    v->boolean   = value;
    return v;
}

JsonValue* json_new_number(Arena* arena, f64 value)
{
    JsonValue* v = json_alloc_value(arena, JSON_NUMBER);
    v->number    = value;
    return v;
}

JsonValue* json_new_string(Arena* arena, string value)
{
    JsonValue* v = json_alloc_value(arena, JSON_STRING);
    v->string    = json_copy_string(arena, value);
    return v;
}

JsonValue* json_new_array(Arena* arena)
{
    UNUSED(arena);
    return json_alloc_value(arena, JSON_ARRAY);
}

JsonValue* json_new_object(Arena* arena)
{
    UNUSED(arena);
    return json_alloc_value(arena, JSON_OBJECT);
}

void json_done(JsonValue* value)
{
    if (!value) {
        return;
    }

    switch (value->kind) {
    case JSON_ARRAY:
        for (usize i = 0; i < array_count(value->array.values); i++) {
            json_done(value->array.values[i]);
        }
        array_free(value->array.values);
        break;
    case JSON_OBJECT:
        for (usize i = 0; i < array_count(value->object.entries); i++) {
            json_done(value->object.entries[i].value);
        }
        array_free(value->object.entries);
        break;
    case JSON_NULL:
    case JSON_BOOL:
    case JSON_NUMBER:
    case JSON_STRING:
        break;
    default:
        ASSERT(false, "Invalid JsonKind");
    }
}

void json_array_push(JsonValue* array_value, JsonValue* value)
{
    ASSERT(array_value && array_value->kind == JSON_ARRAY,
           "json_array_push expects JSON_ARRAY");
    array_push(array_value->array.values, value);
}

void json_object_set(JsonValue* object_value, string key, JsonValue* value)
{
    ASSERT(object_value && object_value->kind == JSON_OBJECT,
           "json_object_set expects JSON_OBJECT");

    u64 hash = json_hash_string(key);
    for (usize i = 0; i < array_count(object_value->object.entries); i++) {
        JsonObjectEntry* entry = &object_value->object.entries[i];
        if (entry->key_hash == hash && json_string_eq(entry->key, key)) {
            entry->value = value;
            return;
        }
    }

    array_push(object_value->object.entries,
               (JsonObjectEntry){
                   .key = key,
                   .key_hash = hash,
                   .value = value,
               });
}

void json_object_set_cstr(JsonValue* object_value, cstr key, JsonValue* value)
{
    json_object_set(object_value, s(key), value);
}

JsonValue* json_array_get(const JsonValue* array_value, usize index)
{
    if (!array_value || array_value->kind != JSON_ARRAY) {
        return NULL;
    }
    if (index >= array_count(array_value->array.values)) {
        return NULL;
    }
    return array_value->array.values[index];
}

JsonValue* json_object_get(const JsonValue* object_value, string key)
{
    if (!object_value || object_value->kind != JSON_OBJECT) {
        return NULL;
    }

    u64 hash = json_hash_string(key);
    for (usize i = 0; i < array_count(object_value->object.entries); i++) {
        JsonObjectEntry* entry = &object_value->object.entries[i];
        if (entry->key_hash == hash && json_string_eq(entry->key, key)) {
            return entry->value;
        }
    }
    return NULL;
}

JsonValue* json_object_get_cstr(const JsonValue* object_value, cstr key)
{
    return json_object_get(object_value, s(key));
}

JsonValue* json_get(const JsonValue* value, string path)
{
    if (!value) {
        return NULL;
    }

    JsonValue* current = (JsonValue*)value;
    usize      start   = 0;

    while (start < path.count) {
        usize end = start;
        while (end < path.count && path.data[end] != '.') {
            end++;
        }

        if (end == start) {
            return NULL;
        }

        if (!current || current->kind != JSON_OBJECT) {
            return NULL;
        }

        current = json_object_get(
            current, (string){.data = path.data + start, .count = end - start});
        if (!current) {
            return NULL;
        }

        start = end + 1;
    }

    return current;
}

JsonValue* json_get_cstr(const JsonValue* value, cstr path)
{
    return json_get(value, s(path));
}

JsonValue* json_parse(Arena* arena, string json, JsonParseResult* out_result)
{
    JsonParseResult local_result = {
        .ok = true,
        .error_offset = 0,
        .error_message = NULL,
    };
    JsonParseResult* result = out_result ? out_result : &local_result;
    *result                 = local_result;

    JsonParser parser = {
        .arena = arena,
        .cur = json.data,
        .end = json.data + json.count,
        .start = json.data,
    };

    JsonValue* value = json_parser_value(&parser, result);
    if (!value) {
        return NULL;
    }

    json_skip_ws(&parser);
    if (parser.cur != parser.end) {
        json_set_error(result, &parser, "Trailing data after JSON value");
        return NULL;
    }

    result->ok = true;
    return value;
}

JsonValue* json_parse_cstr(Arena* arena, cstr json, JsonParseResult* out_result)
{
    return json_parse(arena, s(json), out_result);
}

internal void json_sb_append_indent(StringBuilder* sb, u32 level, u32 indent)
{
    for (u32 i = 0; i < level * indent; i++) {
        sb_append_char(sb, ' ');
    }
}

internal void json_sb_append_escaped_string(StringBuilder* sb, string value)
{
    static const char HEX[] = "0123456789ABCDEF";

    sb_append_char(sb, '"');
    for (usize i = 0; i < value.count; i++) {
        u8 c = value.data[i];
        switch (c) {
        case '"':
            sb_append_cstr(sb, "\\\"");
            break;
        case '\\':
            sb_append_cstr(sb, "\\\\");
            break;
        case '\b':
            sb_append_cstr(sb, "\\b");
            break;
        case '\f':
            sb_append_cstr(sb, "\\f");
            break;
        case '\n':
            sb_append_cstr(sb, "\\n");
            break;
        case '\r':
            sb_append_cstr(sb, "\\r");
            break;
        case '\t':
            sb_append_cstr(sb, "\\t");
            break;
        default:
            if (c < 0x20) {
                char escaped[6] = {
                    '\\',
                    'u',
                    '0',
                    '0',
                    HEX[(c >> 4) & 0x0F],
                    HEX[c & 0x0F],
                };
                sb_append_string(sb, string_from((u8*)escaped, sizeof(escaped)));
            } else {
                sb_append_char(sb, (char)c);
            }
            break;
        }
    }
    sb_append_char(sb, '"');
}

internal void json_stringify_value(StringBuilder* sb,
                                   const JsonValue* value,
                                   bool             pretty,
                                   u32              indent,
                                   u32              level)
{
    switch (value->kind) {
    case JSON_NULL:
        sb_append_cstr(sb, "null");
        break;
    case JSON_BOOL:
        sb_append_cstr(sb, value->boolean ? "true" : "false");
        break;
    case JSON_NUMBER:
        sb_format(sb, "%.17g", value->number);
        break;
    case JSON_STRING:
        json_sb_append_escaped_string(sb, value->string);
        break;
    case JSON_ARRAY: {
        usize count = array_count(value->array.values);
        if (count == 0) {
            sb_append_cstr(sb, "[]");
            break;
        }

        sb_append_char(sb, '[');
        if (pretty) {
            sb_append_char(sb, '\n');
        }
        for (usize i = 0; i < count; i++) {
            if (pretty) {
                json_sb_append_indent(sb, level + 1, indent);
            }
            json_stringify_value(sb, value->array.values[i], pretty, indent, level + 1);
            if (i + 1 < count) {
                sb_append_char(sb, ',');
            }
            if (pretty) {
                sb_append_char(sb, '\n');
            }
        }
        if (pretty) {
            json_sb_append_indent(sb, level, indent);
        }
        sb_append_char(sb, ']');
    } break;
    case JSON_OBJECT: {
        usize count = array_count(value->object.entries);
        if (count == 0) {
            sb_append_cstr(sb, "{}");
            break;
        }

        sb_append_char(sb, '{');
        if (pretty) {
            sb_append_char(sb, '\n');
        }
        for (usize i = 0; i < count; i++) {
            JsonObjectEntry* entry = &value->object.entries[i];
            if (pretty) {
                json_sb_append_indent(sb, level + 1, indent);
            }
            json_sb_append_escaped_string(sb, entry->key);
            sb_append_char(sb, ':');
            if (pretty) {
                sb_append_char(sb, ' ');
            }
            json_stringify_value(sb, entry->value, pretty, indent, level + 1);
            if (i + 1 < count) {
                sb_append_char(sb, ',');
            }
            if (pretty) {
                sb_append_char(sb, '\n');
            }
        }
        if (pretty) {
            json_sb_append_indent(sb, level, indent);
        }
        sb_append_char(sb, '}');
    } break;
    default:
        ASSERT(false, "Invalid JsonKind");
    }
}

string _json_stringify(Arena* arena, const JsonValue* value, JsonStringifyParams params)
{
    ASSERT(value != NULL, "json_stringify expects a non-NULL value");

    StringBuilder sb = {0};
    sb_init(&sb, arena);

    u32 indent = params.indent;
    if (params.pretty && indent == 0) {
        indent = 2;
    }

    json_stringify_value(&sb, value, params.pretty, indent, 0);
    return sb_to_string(&sb);
}

//------------------------------------------------------------------------------
