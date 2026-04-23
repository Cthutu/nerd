//------------------------------------------------------------------------------
// C generation file save
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cgen/cgen.h>
#include <compiler/error/error.h>
#include <stdio.h>

//------------------------------------------------------------------------------

string cgen_render(const CGen* cgen, Arena* arena)
{
    string result = {0};
    if (cgen->arena.cursor == 0) {
        return result;
    }

    u8* copy = (u8*)arena_alloc(arena, cgen->arena.cursor);
    memcpy(copy, cgen->arena.data, cgen->arena.cursor);
    result.data  = copy;
    result.count = cgen->arena.cursor;
    return result;
}

string cgen_render_generated(const CGen* cgen, Arena* arena)
{
    string result = {0};
    if (cgen->generated_end <= cgen->generated_start) {
        return result;
    }

    usize count = cgen->generated_end - cgen->generated_start;
    u8*   copy  = (u8*)arena_alloc(arena, count);
    memcpy(copy, cgen->arena.data + cgen->generated_start, count);
    result.data  = copy;
    result.count = count;
    return result;
}

bool cgen_save(const CGen* cgen, const char* path)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize len     = cgen->arena.cursor;
    usize written = fwrite(cgen->arena.data, 1, len, file);
    if (written != len) {
        fclose(file);
        return error_runtime("Failed to write generated C file: %s", path);
    }

    fclose(file);
    return true;
}
