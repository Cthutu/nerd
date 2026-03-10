//------------------------------------------------------------------------------
// Object System module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------
// Data structures

typedef enum : u8 {
    JSON_NULL = 0,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
} JsonKind;

typedef struct JsonValue JsonValue;

typedef struct {
    Array(JsonValue*) values;
} JsonArray;

typedef struct {
    string     key;
    u64        key_hash;
    JsonValue* value;
} JsonObjectEntry;

typedef struct {
    Array(JsonObjectEntry) entries;
} JsonObject;

struct JsonValue {
    JsonKind kind;
    union {
        bool       boolean;
        f64        number;
        string     string;
        JsonArray  array;
        JsonObject object;
    };
};

typedef struct {
    bool  ok;
    usize error_offset;
    cstr  error_message;
} JsonParseResult;

typedef struct {
    bool pretty;
    u32  indent;
} JsonStringifyParams;

//------------------------------------------------------------------------------
// JSON-style object API

JsonValue* json_new_null(Arena* arena);
JsonValue* json_new_bool(Arena* arena, bool value);
JsonValue* json_new_number(Arena* arena, f64 value);
JsonValue* json_new_string(Arena* arena, string value);
JsonValue* json_new_array(Arena* arena);
JsonValue* json_new_object(Arena* arena);
void       json_done(JsonValue* value);

void json_array_push(JsonValue* array_value, JsonValue* value);

void json_object_set(JsonValue* object_value, string key, JsonValue* value);
void json_object_set_cstr(JsonValue* object_value, cstr key, JsonValue* value);

JsonValue* json_array_get(const JsonValue* array_value, usize index);
JsonValue* json_object_get(const JsonValue* object_value, string key);
JsonValue* json_object_get_cstr(const JsonValue* object_value, cstr key);

//------------------------------------------------------------------------------
// JSON serialisation/deserialisation API

JsonValue* json_parse(Arena* arena, string json, JsonParseResult* out_result);
JsonValue* json_parse_cstr(Arena* arena, cstr json, JsonParseResult* out_result);

string _json_stringify(Arena*              arena,
                       const JsonValue*    value,
                       JsonStringifyParams params);

#define json_stringify(arena, value, ...)                                      \
    _json_stringify((arena), (value), (JsonStringifyParams){__VA_ARGS__})

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
