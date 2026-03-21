//------------------------------------------------------------------------------
// Path helpers
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>
#include <stdio.h>

#if OS_POSIX
#    include <sys/stat.h>
#endif

//------------------------------------------------------------------------------

internal bool path_is_separator(char c) { return c == '/' || c == '\\'; }

bool path_exists(cstr path)
{
#if OS_WINDOWS
    DWORD attrs = GetFileAttributesA(path);
    return attrs != INVALID_FILE_ATTRIBUTES;
#elif OS_POSIX
    struct stat st = {0};
    return stat(path, &st) == 0;
#endif
}

bool path_is_directory(cstr path)
{
#if OS_WINDOWS
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif OS_POSIX
    struct stat st = {0};
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
#endif
}

bool path_has_extension(string path, cstr extension)
{
    usize ext_len = strlen(extension);
    if (path.count < ext_len) {
        return false;
    }

    return memcmp(path.data + path.count - ext_len, extension, ext_len) == 0;
}

string path_filename(string path)
{
    usize start = 0;
    for (usize i = 0; i < path.count; i++) {
        if (path_is_separator((char)path.data[i])) {
            start = i + 1;
        }
    }

    return string_from(path.data + start, path.count - start);
}

string path_stem(string path)
{
    string filename = path_filename(path);
    usize  stem_len = filename.count;

    for (usize i = filename.count; i > 0; i--) {
        if (filename.data[i - 1] == '.') {
            stem_len = i - 1;
            break;
        }
    }

    return string_from(filename.data, stem_len);
}

cstr path_join(Arena* arena, cstr left, cstr right)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_cstr(&sb, left);

    usize left_len = strlen(left);
    if (left_len > 0 && !path_is_separator(left[left_len - 1])) {
#if OS_WINDOWS
        sb_append_char(&sb, '\\');
#else
        sb_append_char(&sb, '/');
#endif
    }

    sb_append_cstr(&sb, right);
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

cstr path_replace_extension(Arena* arena, cstr path, cstr extension)
{
    const char* last_separator = NULL;
    const char* last_dot       = NULL;
    for (const char* p = path; *p != '\0'; p++) {
        if (path_is_separator(*p)) {
            last_separator = p;
            last_dot       = NULL;
        } else if (*p == '.') {
            last_dot = p;
        }
    }

    usize stem_len = last_dot ? (usize)(last_dot - path) : strlen(path);
    if (last_separator && last_dot && last_dot < last_separator) {
        stem_len = strlen(path);
    }

    StringBuilder sb = {0};
    sb_init(&sb, arena);
    sb_append_string(&sb, string_from((u8*)path, stem_len));
    sb_append_cstr(&sb, extension);
    sb_append_null(&sb);
    return (cstr)sb_to_string(&sb).data;
}

bool path_remove(cstr path)
{
    if (!path_exists(path)) {
        return true;
    }

    return remove(path) == 0;
}

//------------------------------------------------------------------------------
