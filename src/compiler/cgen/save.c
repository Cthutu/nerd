//------------------------------------------------------------------------------
// C generation file save
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cgen/cgen.h>
#include <stdio.h>

//------------------------------------------------------------------------------

void cgen_save(const CGen* cgen, const char* path)
{
    const char* last_dot = NULL;
    const char* p        = path;
    while (*p != '\0') {
        if (*p == '.') {
            last_dot = p;
        }
        p++;
    }

    usize stem_len = last_dot ? (usize)(last_dot - path) : (usize)(p - path);

    StringBuilder sb = {0};
    sb_init(&sb, &temp_arena);
    sb_append_string(&sb, string_from((u8*)path, stem_len));
    sb_append_cstr(&sb, ".c");
    sb_append_null(&sb);

    cstr output_path = (cstr)sb_to_string(&sb).data;
    FILE* file = fopen(output_path, "wb");
    if (!file) {
        eprn("Failed to open file for writing: %s", output_path);
        return;
    }

    usize len     = cgen->arena.cursor;
    usize written = fwrite(cgen->arena.data, 1, len, file);
    if (written != len) {
        eprn("Failed to write generated C file: %s", output_path);
    }

    fclose(file);
}
