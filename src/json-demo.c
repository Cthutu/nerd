//------------------------------------------------------------------------------
// JSON API demo
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core object

#include <object/object.h>

//------------------------------------------------------------------------------

internal void dump_json(cstr label, string text)
{
    prn("%s%s%s", ANSI_BOLD_CYAN, label, ANSI_RESET);
    prn(STRINGP, STRINGV(text));
    prn("");
}

int run(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    Arena arena = {0};
    arena_init(&arena);

    JsonParseResult parse_result = {0};
    JsonValue* parsed = json_parse_cstr(
        &arena,
        "{\"name\":\"nerd\",\"version\":1,\"features\":[\"json\",\"arena\"],\"fast\":true}",
        &parse_result);

    if (!parsed || !parse_result.ok) {
        kill("JSON parse failed at byte %zu: %s",
             parse_result.error_offset,
             parse_result.error_message ? parse_result.error_message : "unknown error");
    }

    JsonValue* name     = json_object_get_cstr(parsed, "name");
    JsonValue* version  = json_object_get_cstr(parsed, "version");
    JsonValue* features = json_object_get_cstr(parsed, "features");
    JsonValue* fast     = json_object_get_cstr(parsed, "fast");

    ASSERT(name && name->kind == JSON_STRING, "Expected string field 'name'");
    ASSERT(version && version->kind == JSON_NUMBER, "Expected number field 'version'");
    ASSERT(features && features->kind == JSON_ARRAY, "Expected array field 'features'");
    ASSERT(fast && fast->kind == JSON_BOOL, "Expected bool field 'fast'");

    prn("%sParsed values%s", ANSI_BOLD_GREEN, ANSI_RESET);
    prn("name     : " STRINGP, STRINGV(name->string));
    prn("version  : %.0f", version->number);
    prn("features : %zu", array_count(features->array.values));
    prn("fast     : %s", fast->boolean ? "true" : "false");
    prn("");

    JsonValue* root = json_new_object(&arena);
    json_object_set_cstr(root, "project", json_new_string(&arena, s("json-demo")));
    json_object_set_cstr(root, "passes", json_new_bool(&arena, true));
    json_object_set_cstr(root, "score", json_new_number(&arena, 99.5));
    json_object_set_cstr(root, "notes", json_new_null(&arena));

    JsonValue* list = json_new_array(&arena);
    json_array_push(list, json_new_string(&arena, s("parse")));
    json_array_push(list, json_new_string(&arena, s("build")));
    json_array_push(list, json_new_string(&arena, s("stringify")));
    json_object_set_cstr(root, "steps", list);
    json_object_set_cstr(root, "source", parsed);

    dump_json("Compact JSON", json_stringify(&arena, root));
    dump_json("Pretty JSON", json_stringify(&arena, root, .pretty = true, .indent = 2));

    json_done(root);
    arena_done(&arena);
    return 0;
}
